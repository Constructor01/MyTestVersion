#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); // Port for communication
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error binding" << std::endl;
        return 1;
    }

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error listening" << std::endl;
        return 1;
    }

    std::cout << "Waiting for Program #1 to connect..." << std::endl;

    while (true) {
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == -1) {
            std::cerr << "Error accepting connection" << std::endl;
            continue;
        }

        char buffer[1024];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            close(clientSocket);
            continue;
        }

        std::string receivedData(buffer, bytesRead);
        close(clientSocket);

        if (receivedData.length() > 2 && std::stoi(receivedData) % 32 == 0) {
            std::cout << "Received valid data: " << receivedData << std::endl;
        }
        else {
            std::cout << "Received invalid data: " << receivedData << std::endl;
        }
    }

    close(serverSocket);
    return 0;
}
