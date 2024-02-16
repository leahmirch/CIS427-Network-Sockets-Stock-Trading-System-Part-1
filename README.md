### Student Names and Emails
- Leah Mirch (lmirch@umich.edu)
- Dominic Baughman (baughboy@umich.edu)

### Introduction
This project is a simple stock trading simulation system built using C++ and SQLite for database management. It is designed to run on Unix-based systems, leveraging network programming techniques for client-server communication. The system allows users to buy and sell stocks, check their balance, and list available stocks.

### Running Instructions
#### Running the Server on the UMD server:
- connect to umd vpn
```bash
ssh lmirch@login.umd.umich.edu
```

- Enter password
- Duo authenticate

```bash
cd /home/l/lmirch/Private/code\ base/ *this is assuming you’ve placed the files here*
```
```bash
gcc -c sqlite3.c -o sqlite3.o -lpthread -ldl
```
```bash
g++ -g -o server server.cpp sqlite3.o -lpthread -ldl -std=c++11
```
```bash
./server
```

#### Running the Client on the UMD Server
- connect to umd vpn
```bash
ssh lmirch@login.umd.umich.edu
```

    - Enter password
    - Duo authenticate

```bash
cd /home/l/lmirch/Private/code\ base/ *this is assuming you’ve placed the files here*
```
```bash
gcc -c sqlite3.c -o sqlite3.o -lpthread -ldl
```
```bash
g++ -o client client.cpp -std=c++11
```
```bash
./server
```

#### Inserting Data into the Database
```bash
sqlite3 stock_trading.db
```
```bash
INSERT INTO Users (first_name, last_name, user_name, password, usd_balance) VALUES ('Leah', 'Mirch', 'lmirch', 
```
```bash
'Password!', 100.0); 
```
```bash
SELECT * FROM Users;
```
```bash
INSERT INTO Stocks (stock_symbol, stock_name, stock_balance, user_id) VALUES ('GMCA', 'General Motors', 15, 5);
```
```bash
SELECT * FROM Stocks;
```
```bash
.exit
```

### Each Student's Role
- Leah Mirch: Implemented the server and client communication, database interactions, and the handling of all commands (BUY, SELL, LIST, BALANCE, SHUTDOWN, QUIT). Created the README. Establishing a base for local connectivity. Implimented requirements for the code. Added code comments, added in an established user and stock within the databse. Figured out and established connection to the UMD servers (both client.cpp and server.cpp).

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

