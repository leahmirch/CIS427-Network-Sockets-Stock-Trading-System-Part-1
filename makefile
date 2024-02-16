# Compiler settings - Can change gcc to g++ for C++ setup
CC = g++
CFLAGS = -g -std=c++11
LIBS = -lpthread -ldl

# Project files
SERVER_SRC = server.cpp
CLIENT_SRC = client.cpp
SQLITE_OBJ = sqlite3.o

# Executable names
SERVER_EXE = server
CLIENT_EXE = client
IP = login.umd.umich.edu

# Compile SQLite3 object file
$(SQLITE_OBJ): sqlite3.c
	gcc -c sqlite3.c -o $(SQLITE_OBJ) $(LIBS)

# Default target
all: $(SERVER_EXE) $(CLIENT_EXE)

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
	./$(CLIENT_EXE) IP

# Clean the build
clean:
	rm -f $(SERVER_EXE) $(CLIENT_EXE) $(SQLITE_OBJ)
