#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <algorithm>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

class ProgramFirst {
public:
    ProgramFirst() : data_ready(false) {}

    void start() {
        t1 = std::thread(&ProgramFirst::thread1, this);
        t2 = std::thread(&ProgramFirst::thread2, this);

        t1.join();
        t2.join();
    }

private:
    std::mutex mtx;
    std::condition_variable cv;
    bool data_ready;
    int sum;
    std::thread t1, t2;

    bool is_all_digits(const std::string& str) {
        return str.find_first_not_of("0123456789") == std::string::npos;
    }

    void thread1() {
        while (true) {
            std::string input;
            std::cout << "Enter a string of digits (up to 64 characters): ";
            std::cin >> input;

            if (input.length() <= 64 && is_all_digits(input)) {
                std::sort(input.begin(), input.end(), std::greater<char>());
                
                std::string pre_str;
                for (char& c : input) {
                    if ((c - '0') % 2 == 0) {
                        pre_str.push_back("BK");
                        continue;
                    }
                    pre_str.push_back(c);
                }
                input = move(pre_str);

                sum = calculate_sum(input);

                std::unique_lock<std::mutex> lock(mtx);
                data_ready = true;
                cv.notify_one();
            }
            else {
                std::cout << "Invalid input. Please enter a valid string of digits." << std::endl;
            }
        }
    }

    void thread2() {
        while (true) {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this] { return data_ready; });

            send_sum_to_program2(sum);

            data_ready = false;
        }
    }

    int calculate_sum(const std::string& data) {
        int sum = 0;
        for (char c : data) {
            if (isdigit(c)) {
                sum += c - '0';
            }
        }
        return sum;
    }

    void send_sum_to_program2(int sum) {
        int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == -1) {
            std::cerr << "Error creating socket" << std::endl;
            return;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(12345); // Port used by Program #2
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP address of Program #2

        if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            //std::cerr << "Error connecting to Program #2" << std::endl;
            close(clientSocket);
            return;
        }

        // Send the sum to Program #2
        send(clientSocket, &sum, sizeof(sum), 0);

        close(clientSocket);
    }
};

int main() {
    ProgramFirst program;
    program.start();

    return 0;
}
