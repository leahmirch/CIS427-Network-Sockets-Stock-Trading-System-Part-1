# not working currently


# Define compiler and flags
CXX=g++
CXXFLAGS=-std=c++11 -Wall
LDFLAGS=-lpthread -ldl

# Define targets
all: server client

server: server.cpp
	$(CXX) $(CXXFLAGS) -o server server.cpp sqlite3.o $(LDFLAGS)

client: client.cpp
	$(CXX) $(CXXFLAGS) -o client client.cpp

.PHONY: clean

clean:
	rm -f server client
