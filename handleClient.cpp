#include <iostream>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include "sqlite3.h"
#include <iomanip> // For std::setprecision
#include <sys/socket.h>


#define BUFFER_SIZE 1024

sqlite3* db;

// Helper functions
bool startsWith(const std::string& fullString, const std::string& prefix) {
    if (fullString.length() < prefix.length()) {
        return false;
    }
    return std::equal(prefix.begin(), prefix.end(), fullString.begin());
}

// Callback function for SQLite queries
int callback(void *data, int argc, char **argv, char **azColName) {
    int i;
    for (i = 0; i < argc; i++) {
        std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    return 0;
}

void executeSQL(const std::string& sql, int (*callback)(void*, int, char**, char**) = nullptr, void* data = nullptr) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), callback, data, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

// Command processing functions
void processBuyCommand(int clientSocket, const std::string& command);
void processSellCommand(int clientSocket, const std::string& command);
void processListCommand(int clientSocket);
void processBalanceCommand(int clientSocket, const std::string& command);
void processShutdownCommand(int clientSocket);
void processQuitCommand(int clientSocket);

void handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE];
    bool isRunning = true;

    while (isRunning) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytesReceived = read(clientSocket, buffer, BUFFER_SIZE - 1);

        if (bytesReceived < 1) {
            std::cout << "Client disconnected or read error" << std::endl;
            break;
        }

        std::string command(buffer);
        std::cout << "Received command: " << command << std::endl;

        if (startsWith(command, "BUY")) {
            processBuyCommand(clientSocket, command);
        } else if (startsWith(command, "SELL")) {
            processSellCommand(clientSocket, command);
        } else if (startsWith(command, "LIST")) {
            processListCommand(clientSocket);
        } else if (startsWith(command, "BALANCE")) {
            processBalanceCommand(clientSocket, command);
        } else if (startsWith(command, "SHUTDOWN")) {
            processShutdownCommand(clientSocket);
            isRunning = false; // To stop the server, adjust as needed
        } else if (startsWith(command, "QUIT")) {
            processQuitCommand(clientSocket);
            break; // Exit the loop to end the client session
        } else {
            std::string errorMsg = "400 invalid command\n";
            write(clientSocket, errorMsg.c_str(), errorMsg.length());
        }
    }

    close(clientSocket);
}


// Utility function to send messages to the client
void sendMessage(int clientSocket, const std::string& message) {
    send(clientSocket, message.c_str(), message.size(), 0);
}

// Utility function to execute SQL and handle callback
int executeSQLWithCallback(const std::string& sql, int (*callback)(void*, int, char**, char**), void* data) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), callback, data, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    return rc;
}

// Callback function to fetch balance or stock information
int fetchInfoCallback(void *data, int argc, char **argv, char **azColName) {
    std::string* response = reinterpret_cast<std::string*>(data);
    for (int i = 0; i < argc; i++) {
        if (argv[i]) *response += std::string(azColName[i]) + ": " + argv[i] + "\n";
    }
    return 0;
}



void processBuyCommand(int clientSocket, const std::string& command) {
    std::istringstream iss(command);
    std::string cmd, stockSymbol, userIdStr;
    double stockAmount, pricePerStock;
    int userId;
    iss >> cmd >> stockSymbol >> stockAmount >> pricePerStock >> userId;

    // Calculate total cost
    double totalCost = stockAmount * pricePerStock;

    // Check if user has enough balance
    std::string balanceQuery = "SELECT usd_balance FROM Users WHERE ID = " + std::to_string(userId) + ";";
    std::string balanceResponse;
    executeSQLWithCallback(balanceQuery, fetchInfoCallback, &balanceResponse);
    
    std::istringstream balanceStream(balanceResponse.substr(balanceResponse.find(":") + 1));
    double currentBalance;
    balanceStream >> currentBalance;

    if (currentBalance >= totalCost) {
        // Deduct total cost from user's balance
        std::string deductBalanceSql = "UPDATE Users SET usd_balance = usd_balance - " + std::to_string(totalCost) + " WHERE ID = " + std::to_string(userId) + ";";
        executeSQL(deductBalanceSql);

        // Update or insert stock record
        std::string stockSql = "INSERT INTO Stocks (stock_symbol, stock_name, stock_balance, user_id) VALUES ('" + stockSymbol + "', '', " + std::to_string(stockAmount) + ", " + std::to_string(userId) + ") ON CONFLICT(stock_symbol) DO UPDATE SET stock_balance = stock_balance + " + std::to_string(stockAmount) + ";";
        executeSQL(stockSql);

        // Fetch updated balance and stock amount
        std::string updatedBalanceResponse;
        executeSQLWithCallback(balanceQuery, fetchInfoCallback, &updatedBalanceResponse);
        std::string response = "200 OK\nBOUGHT: New balance: " + updatedBalanceResponse;
        sendMessage(clientSocket, response);
    } else {
        sendMessage(clientSocket, "Not enough balance\n");
    }
}

void processSellCommand(int clientSocket, const std::string& command) {
    std::istringstream iss(command);
    std::string cmd, stockSymbol, userIdStr, stockAmountStr, pricePerStockStr;
    iss >> cmd >> stockSymbol >> stockAmountStr >> pricePerStockStr >> userIdStr;

    double stockAmount = std::stod(stockAmountStr);
    double pricePerStock = std::stod(pricePerStockStr);
    int userId = std::stoi(userIdStr);
    double totalCredit = stockAmount * pricePerStock;

    std::string sqlCheckStock = "SELECT stock_balance FROM Stocks WHERE stock_symbol = '" + stockSymbol + "' AND user_id = " + userIdStr + ";";
    std::string stockResponse;
    executeSQL(sqlCheckStock, fetchInfoCallback, &stockResponse);

    if (!stockResponse.empty()) {
        double currentStockAmount;
        std::istringstream(stockResponse.substr(stockResponse.find(":") + 1)) >> currentStockAmount;

        if (currentStockAmount >= stockAmount) {
            std::string sqlUpdateStock = "UPDATE Stocks SET stock_balance = stock_balance - " + std::to_string(stockAmount) + " WHERE stock_symbol = '" + stockSymbol + "' AND user_id = " + userIdStr + ";";
            executeSQL(sqlUpdateStock);

            std::string sqlCreditBalance = "UPDATE Users SET usd_balance = usd_balance + " + std::to_string(totalCredit) + " WHERE ID = " + userIdStr + ";";
            executeSQL(sqlCreditBalance);

            sendMessage(clientSocket, "200 OK\nSOLD: " + stockAmountStr + " " + stockSymbol + ". New USD balance updated.\n");
        } else {
            sendMessage(clientSocket, "Not enough stock balance\n");
        }
    } else {
        sendMessage(clientSocket, "Stock record not found\n");
    }
}

void processListCommand(int clientSocket) {
    std::string sqlListStocks = "SELECT stock_symbol, stock_balance FROM Stocks;";
    std::string listResponse = "Stocks Owned:\n";
    executeSQL(sqlListStocks, fetchInfoCallback, &listResponse);
    sendMessage(clientSocket, "200 OK\n" + listResponse);
}

void processBalanceCommand(int clientSocket, const std::string& command) {
    std::istringstream iss(command);
    std::string cmd, userIdStr;
    iss >> cmd >> userIdStr;

    std::string sql = "SELECT usd_balance FROM Users WHERE ID = " + userIdStr + ";";
    std::string balanceResponse = "USD Balance:\n";
    executeSQL(sql, fetchInfoCallback, &balanceResponse);
    sendMessage(clientSocket, "200 OK\n" + balanceResponse);
}

void processShutdownCommand(int clientSocket) {
    sendMessage(clientSocket, "200 OK\nServer shutting down\n");
    // Close the database connection
    if (db) {
        sqlite3_close(db);
    }
    // Signal or directly close the server socket and exit the server application as needed
    exit(0); // Use with caution; this will terminate the server process
}

void processQuitCommand(int clientSocket) {
    sendMessage(clientSocket, "200 OK\n");
    // No further logic required for QUIT, as the connection will be closed afterward
}
