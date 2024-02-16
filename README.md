### Student Names and Emails
- Leah Mirch (lmirch@umich.edu)
- Dominic Baughman (baughboy@umich.edu)

### Introduction
This project is a simple stock trading simulation system built using C++ and SQLite for database management. It is designed to run on Unix-based systems, leveraging network programming techniques for client-server communication. The system allows users to buy and sell stocks, check their balance, and list available stocks.

### Running Instructions
#### Compiling the Server Locally
```bash
g++ -g -o server server.cpp sqlite3.o -lpthread -ldl -std=c++11
```
#### Running the Server
```bash
./server
```
#### Compiling the Client
```bash
g++ -o client client.cpp -std=c++11
```
#### Running the Client Locally
```bash
./client 127.0.0.1
```
#### Inserting Data into the Database
```bash
sqlite3 stock_trading.db
INSERT INTO Users (first_name, last_name, user_name, password, usd_balance) VALUES ('Leah', 'Mirch', 'lmirch', 'Password!', 100.0); 
SELECT * FROM Users;
INSERT INTO Stocks (stock_symbol, stock_name, stock_balance, user_id) VALUES ('GMCA', 'General Motors', 15, 5);
SELECT * FROM Stocks;
.exit
```

### Each Student's Role
- Leah Mirch: Implemented the server and client communication, database interactions, and the handling of all commands (BUY, SELL, LIST, BALANCE, SHUTDOWN, QUIT). Created the README. Establishing a base for local connectivity. Implimented requirements for the code. Added code comments, added in an established user and stock within the databse. 

### Bugs in the Code
- Database locks may occur if multiple clients try to access the database simultaneously. This is because SQLite's default configuration is not fully optimized for high concurrency. Database allows for multiple clients to access the server, but not to all do commands at once. 

### Commands Implemented
- **BUY**: Purchase stocks, specifying the stock symbol, quantity, price, and user ID.
- **SELL**: Sell stocks, specifying the stock symbol, quantity, price, and user ID.
- **LIST**: List all available stocks in the system.
- **BALANCE**: Check the current balance and stock holdings of a specified user ID.
- **SHUTDOWN**: Shut down the server gracefully.
- **QUIT**: Disconnect the client from the server.

### Sample Run
The output of a sample run can be captured in a recorded video or detailed in a step-by-step text format showing the client-server interaction for each command. For instance, after running the server and client as per the instructions, demonstrate each command's input and the corresponding output from the server.