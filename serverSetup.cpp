#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include "sqlite3.h"
#include <unistd.h>
#include "clientSetup.cpp"
#include "initializeDatabase.cpp"
#include "handleClient.cpp"


#define SERVER_PORT 2025

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
    std::cout << "Server listening on port " << SERVER_PORT << std::endl;

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
