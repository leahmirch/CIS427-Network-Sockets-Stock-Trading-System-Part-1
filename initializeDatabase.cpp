#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include "sqlite3.h"

#define SERVER_PORT 2025

void initializeDatabase() {
    sqlite3 *db;
    char *errMsg = 0;
    int rc;

    rc = sqlite3_open("stock_trading.db", &db);
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

    // Close database
    sqlite3_close(db);
}
