#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "sqlite3.h"
#include <unistd.h>
#define SERVER_PORT 2025 // Define the server port to connect to
int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << "141.215.93.223" << std::endl;
        return 1;
    }
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[1024]; // Buffer for server responses

    // Create and setup the client socket
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serverAddr, 0, sizeof(serverAddr)); // Initialize server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, argv[1], &serverAddr.sin_addr); // Convert IP address from text to binary form
    // Connect to server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection Failed." << std::endl;
        return 1;
    }
    std::cout << "Connected to the server." << std::endl;
    // Main loop to send commands to the server
    while (true) {
        std::cout << "Enter command: ";
        std::string cmd;
        std::getline(std::cin, cmd); // Read command from user
        if (cmd == "QUIT") {
            break; // Exit loop if QUIT command is entered
        }
        send(clientSocket, cmd.c_str(), cmd.length(), 0); // Send command to server
        // Receive and display response from server
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            std::cout << "Server response: " << buffer << std::endl;
        }
    }
    close(clientSocket); // Close the client socket
    return 0;
}