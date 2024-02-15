#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 2025

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <Server IP Address>" << std::endl;
        return 1;
    }

    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[1024];

    // Create socket
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, argv[1], &serverAddr.sin_addr);

    // Connect to server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection Failed." << std::endl;
        return 1;
    }

    std::cout << "Connected to the server." << std::endl;

    while (true) {
        std::cout << "Enter command: ";
        std::string cmd;
        std::getline(std::cin, cmd);
        if (cmd == "QUIT") {
            break;
        }

        // Send command to server
        send(clientSocket, cmd.c_str(), cmd.length(), 0);

        // Receive response from server
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            std::cout << "Server response: " << buffer << std::endl;
        }
    }

    // Close the socket
    close(clientSocket);

    return 0;
}
