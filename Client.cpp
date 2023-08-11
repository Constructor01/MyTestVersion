#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>

class ProgramSecond {
public:
    ProgramSecond() {}

    void startListening() {
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            std::cerr << "Error creating socket" << std::endl;
            return;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(12345); // Port for communication
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            std::cerr << "Error binding" << std::endl;
            return;
        }

        if (listen(serverSocket, 5) == -1) {
            std::cerr << "Error listening" << std::endl;
            return;
        }

        std::cout << "Waiting for Program #1 to connect..." << std::endl;

        while (true) {
            int clientSocket = accept(serverSocket, NULL, NULL);
            if (clientSocket == -1) {
                std::cerr << "Error accepting connection" << std::endl;
                continue;
            }

            int receivedSum;
            int bytesRead = recv(clientSocket, &receivedSum, sizeof(receivedSum), 0);
            if (bytesRead != sizeof(receivedSum)) {
                close(clientSocket);
                continue;
            }

            close(clientSocket);

            if (isValidSum(receivedSum)) {
                std::cout << "Received valid sum: " << receivedSum << std::endl;
            }
            else {
                std::cout << "Received invalid sum: " << receivedSum << std::endl;
            }
        }

        close(serverSocket);
    }

private:
    bool isValidSum(int sum) {
        return sum > 9 && sum % 32 == 0;
    }
};

int main() {
    ProgramSecond program2;
    program2.startListening();

    return 0;
}
