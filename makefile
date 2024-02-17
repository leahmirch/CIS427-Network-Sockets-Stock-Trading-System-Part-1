# Compiler settings - Can change gcc to g++ for C++ setup
CC = g++
CFLAGS = -g -std=c++11
LIBS = -lpthread -ldl

# Project files
SERVER_SRC = server.cpp
CLIENT_SRC = client.cpp
SQLITE_OBJ = sqlite3.o
SQLITE_URL = https://www.sqlite.org/2022/sqlite-amalgamation-3400100.zip
SQLITE_ZIP = sqlite-amalgamation-3400100.zip

# Executable names
SERVER_EXE = server
CLIENT_EXE = client
IP = login.umd.umich.edu

# Download and unzip SQLite source, then compile the object file
$(SQLITE_OBJ): 
	curl -O $(SQLITE_URL)
	unzip $(SQLITE_ZIP) && mv sqlite-amalgamation-3400100/sqlite3.c .
	gcc -c sqlite3.c -o $(SQLITE_OBJ) $(LIBS)
	rm -rf sqlite3.c sqlite-amalgamation-3400100 $(SQLITE_ZIP)

# Default target
all: sqlite $(SERVER_EXE) $(CLIENT_EXE)

sqlite: $(SQLITE_OBJ)

# Compile server
$(SERVER_EXE): $(SERVER_SRC) $(SQLITE_OBJ)
	$(CC) $(CFLAGS) -o $(SERVER_EXE) $(SERVER_SRC) $(SQLITE_OBJ) $(LIBS)

# Compile client
$(CLIENT_EXE): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT_EXE) $(CLIENT_SRC) $(LIBS)

# Run the server executable
runserver: $(SERVER_EXE)
	./$(SERVER_EXE)

runclient: $(CLIENT_EXE)
	./$(CLIENT_EXE) $(IP)

# Clean the build
clean:
	rm -f $(SERVER_EXE) $(CLIENT_EXE) $(SQLITE_OBJ) *.o *~ core
