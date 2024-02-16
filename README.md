## CIS427 Winter 2024 Programming Assignment 1
## Network Sockets - Stock Trading System

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
make clean
```
```bash
make
```
```bash
make runserver
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
make runclient
```

#### Inserting Data into the Database
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
sqlite3 stock_trading.db
```

to populate the users table:

```bash
INSERT INTO Users (first_name, last_name, user_name, password, usd_balance) VALUES ('Leah', 'Mirch', 'lmirch', 'Password!', 100.0); 
```

to show all users within the users table:

```bash
SELECT * FROM Users;
```

to populate the stocks table:

```bash
INSERT INTO Stocks (stock_symbol, stock_name, stock_balance, user_id) VALUES ('GMCA', 'General Motors', 15, 5);
```

to show all stocks within the stocks table:

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


### Additional Information
Below is the gitub link used within the project. This is so that we can have blame, show progress, and have an established location to transfer code between each other.

https://github.com/leahmirch/code-base