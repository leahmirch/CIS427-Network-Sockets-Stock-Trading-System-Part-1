#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include "sqlite3.h"
#include <unistd.h>
#include <sstream>
#include <algorithm>
#include <iomanip>

#define SERVER_PORT 2025
#define BUFFER_SIZE 1024

sqlite3* db; // Declare this globally at the top of server.cpp file

void initializeDatabase() {
    char *errMsg = 0;
    int rc = sqlite3_open("stock_trading.db", &db); // Use the global db variable directly
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    // Create Users table
    const char *createUsersTableSQL = 
        "CREATE TABLE IF NOT EXISTS Users ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "first_name TEXT, "
        "last_name TEXT, "
        "user_name TEXT NOT NULL, "
        "password TEXT, "
        "usd_balance DOUBLE NOT NULL);";

    // Create Stocks table
    const char *createStocksTableSQL = 
        "CREATE TABLE IF NOT EXISTS Stocks ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "stock_symbol VARCHAR(4) NOT NULL, "
        "stock_name VARCHAR(20) NOT NULL, "
        "stock_balance DOUBLE, "
        "user_id INTEGER, "
        "FOREIGN KEY(user_id) REFERENCES Users(ID));";

    // Execute SQL for table creation
    sqlite3_exec(db, createUsersTableSQL, 0, 0, &errMsg);
    sqlite3_exec(db, createStocksTableSQL, 0, 0, &errMsg);

    if (errMsg != 0) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

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

    if (bytesReceived > 0) {
        std::cout << "Server response: " << std::string(buffer, bytesReceived) << std::endl;
    } 
    else if (bytesReceived <= 0) {
        std::cout << "Client disconnected." << std::endl;
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
            std::cout << "Invalid command response sent. Ready for next command." << std::endl; // Debugging line
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
int fetchBalanceCallback(void *data, int argc, char **argv, char **azColName) {
    double* balance = reinterpret_cast<double*>(data);
    if (argc > 0 && argv[0]) {
        *balance = std::stod(argv[0]);
    }
    return 0;
}

struct CallbackData {
    double stockAmount;
    double pricePerStock;
    int userId;
    std::string stockSymbol;
    std::string* response; // Point to a string to store the response message
};

int sellCallback(void *data, int argc, char **argv, char **azColName) {
    CallbackData* cbData = static_cast<CallbackData*>(data);
    // Now use cbData->stockAmount, cbData->pricePerStock, etc., inside  callback
    return 0;
}

static int stockBalanceCallback(void *data, int argc, char **argv, char **azColName) {
    if (argc > 0 && argv[0]) {
        *(double *)data = atof(argv[0]);
    }
    return 0;
}




// process commands
void processBuyCommand(int clientSocket, const std::string& command) { // working: ex. input BUY MSFT 3.4 1.35 1 (Where 3.4 is the amount of stocks to buy, $1.35 price per stock, 1 is the user id.
    std::istringstream iss(command);
    std::string cmd, stockSymbol;
    double stockAmount, pricePerStock;
    int userId;
    iss >> cmd >> stockSymbol >> stockAmount >> pricePerStock >> userId;

    double totalCost = stockAmount * pricePerStock;
    double currentBalance = 0;

    // Prepare the SQL query to fetch the user's balance
    std::string balanceQuery = "SELECT usd_balance FROM Users WHERE ID = " + std::to_string(userId) + ";";

    // Use the specific callback for fetching balance
    executeSQLWithCallback(balanceQuery, fetchBalanceCallback, &currentBalance);

    if (currentBalance >= totalCost) {
        // Proceed with updating the user's balance and stocks
        std::string updateBalanceSql = "UPDATE Users SET usd_balance = usd_balance - " + std::to_string(totalCost) + " WHERE ID = " + std::to_string(userId) + ";";
        executeSQL(updateBalanceSql);

        // Check if the stock exists for the user, update or insert accordingly
        std::string stockUpdateSql = "INSERT INTO Stocks (stock_symbol, stock_name, stock_balance, user_id) VALUES ('" + stockSymbol + "', 'Unknown', " + std::to_string(stockAmount) + ", " + std::to_string(userId) + ") ON CONFLICT(stock_symbol) DO UPDATE SET stock_balance = stock_balance + excluded.stock_balance;";
        executeSQL(stockUpdateSql);

        sendMessage(clientSocket, "200 OK\nBOUGHT: " + std::to_string(stockAmount) + " of " + stockSymbol + ". New USD balance: $" + std::to_string(currentBalance - totalCost) + "\n");
    } else {
        sendMessage(clientSocket, "Not enough balance\n");
    }
}

void processSellCommand(int clientSocket, const std::string& command) { // working ex. SELL APPL 2 1.45 1
    std::istringstream iss(command);
    std::string cmd, stockSymbol;
    double stockAmount, pricePerStock;
    int userId;
    iss >> cmd >> stockSymbol >> stockAmount >> pricePerStock >> userId;

    // Calculate the total sale value
    double saleValue = stockAmount * pricePerStock;

    // Update the user's USD balance directly without checking stock balance
    std::string updateBalanceSql = "UPDATE Users SET usd_balance = usd_balance + " + std::to_string(saleValue) + " WHERE ID = " + std::to_string(userId) + ";";
    
    // Execute the update balance SQL command
    char *errMsg = nullptr;
    if (sqlite3_exec(db, updateBalanceSql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sendMessage(clientSocket, "Failed to update balance.\n");
        return;
    }

    // Fetch the new balance to send back to the client
    std::string fetchBalanceSql = "SELECT usd_balance FROM Users WHERE ID = " + std::to_string(userId) + ";";
    double newBalance = 0.0;
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, fetchBalanceSql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            newBalance = sqlite3_column_double(stmt, 0);
        }
        sqlite3_finalize(stmt);
    } else {
        sendMessage(clientSocket, "Error fetching new balance.\n");
        return;
    }

    // Send the new balance to the client
    std::string response = "200 OK\nSOLD: " + std::to_string(stockAmount) + " of " + stockSymbol + ". USD $" + std::to_string(newBalance);
    sendMessage(clientSocket, response + "\n");
}

void processListCommand(int clientSocket) { // working
    // Updated SQL query to include user_id and order by ID for enumeration
    std::string sqlListStocks = "SELECT ID, stock_symbol, stock_balance, user_id FROM Stocks ORDER BY ID;";
    char* errMsg = nullptr;

    std::string response = "200 OK\nThe list of records in the Stocks database:\n";

    // Define the callback function directly (if using C++11 or later)
    auto callback = [](void* data, int argc, char** argv, char** azColName) -> int {
        std::string* response = static_cast<std::string*>(data);
        for (int i = 0; i < argc; i++) {
            // Append each column value followed by a space (or newline for the last column)
            *response += std::string(argv[i] ? argv[i] : "NULL") + " ";
        }
        *response += "\n"; // Separate records by a newline
        return 0;
    };

    // Execute the SQL query and use the callback to format the response
    if (sqlite3_exec(db, sqlListStocks.c_str(), callback, &response, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        sendMessage(clientSocket, response);
    }
}

void processBalanceCommand(int clientSocket, const std::string& command) { // working- enter as "BALANCE 1" for the balance of userID = 1
    std::istringstream iss(command);
    std::string cmd, userIdStr;
    iss >> cmd >> userIdStr;

    std::string sql = "SELECT first_name, last_name, usd_balance FROM Users WHERE ID = " + userIdStr + ";";
    std::string balanceResponse = ""; // Initialize empty to be filled by the callback
    executeSQLWithCallback(sql, fetchBalanceCallback, &balanceResponse);

    if (balanceResponse.empty()) {
        balanceResponse = "User not found or balance unavailable.";
    }
    sendMessage(clientSocket, "200 OK\n" + balanceResponse);
}

void processShutdownCommand(int clientSocket) { // working
    sendMessage(clientSocket, "200 OK\nServer shutting down\n");
    // Close the database connection
    if (db) {
        sqlite3_close(db);
    }
    // Signal or directly close the server socket and exit the server application as needed
    exit(0); // Use with caution; this will terminate the server process
}

void processQuitCommand(int clientSocket) { // working
    sendMessage(clientSocket, "200 OK\n");
    usleep(100000);
    // No further logic required for QUIT, as the connection will be closed afterward
}




int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize;

    // Initialize SQLite database
    initializeDatabase();

    // Create socket
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to port
    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    // Listen on the port
    listen(serverSocket, 5);
    std::cout << "Server listening on port " << SERVER_PORT << "." << std::endl;

    while (true) {
        // Accept client connections
        addrSize = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);
        std::cout << "Client connected." << std::endl;

        // Handle client in a separate function or thread
        handleClient(clientSocket);

        // Close client socket
        close(clientSocket);
    }

    // Close server socket and cleanup
    close(serverSocket);
    return 0;
}