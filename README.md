## CIS427 Winter 2024 Programming Assignment 1 : Network Sockets - Stock Trading System

### Student Names and Emails
- Leah Mirch (lmirch@umich.edu)
- Dominic Baughman (baughboy@umich.edu)

### Introduction
In this first project for CIS 421, we integrated a Stock Trading System within the netowrk socket. We did this by using C++ for the main code and SQLite for all the database related components. This program uses network programming techniques and and a Unix based system to allow for the client and server to work together. The actions this system can take are BUY, SELL, LIST, BALANCE, SHUTDOWN, and QUIT. 

### Running Instructions
#### Running the Server on the UMD server:
- connect to umd vpn
- connect using SSH client using "login.umd.umich.edu"
```bash
ssh lmirch@login.umd.umich.edu
```

- Enter password
- Duo authenticate

```bash
cd /home/l/lmirch/Private/pa1LeahMirch/ *this is assuming the files are placed here*
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
- connect using SSH client using "login.umd.umich.edu"
```bash
ssh lmirch@login.umd.umich.edu
```

- Enter password
- Duo authenticate

```bash
cd /home/l/lmirch/Private/pa1LeahMirch/ *this is assuming the files are placed here*
```
```bash
make runclient
```

#### Inserting Data into the Database
- connect to umd vpn
- connect using SSH client using "login.umd.umich.edu"
```bash
ssh lmirch@login.umd.umich.edu
```

- Enter password
- Duo authenticate

```bash
cd /home/l/lmirch/Private/pa1LeahMirch/ *this is assuming the files are placed here*
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
- Leah Mirch: Implemented the server and client communication, database interactions, and the handling of all commands (BUY, SELL, LIST, BALANCE, SHUTDOWN, QUIT). Created the README. Establishing a base for local connectivity. Implimented requirements for the code. Added code comments, added in an established user and stock within the databse. Figured out and established connection to the UMD servers (both client.cpp and server.cpp). Created the makefile, updated the readme with proper configuration settings.
- Dominic Baughman: Made changes to the readme, designed a base for the makefile. 

### Commands Implemented
- **BUY**: Purchase stocks, specifying the stock symbol, quantity, price, and user ID.
- **SELL**: Sell stocks, specifying the stock symbol, quantity, price, and user ID.
- **LIST**: List all available stocks in the system.
- **BALANCE**: Check the current balance and stock holdings of a specified user ID.
- **SHUTDOWN**: Shut down the server gracefully.
- **QUIT**: Disconnect the client from the server.

### Additional Information
Below is the gitub link used within the project. This is so that we can have blame, show progress, and have an established location to transfer code between each other.

https://github.com/leahmirch/code-base