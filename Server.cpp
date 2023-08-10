#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <algorithm>

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
    std::string shared_buffer;
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

                for (char& c : input) {
                    if (c % 2 == 0) {
                        c = 'K';  // Replace even digits with 'K'
                    }
                }
                std::replace(input.begin(), input.end(), 'K', 'B');

                std::unique_lock<std::mutex> lock(mtx);
                shared_buffer = input;
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

            std::string data = shared_buffer;
            data_ready = false;
            lock.unlock();

            std::cout << "Processed data: " << data << std::endl;

            int sum = 0;
            for (char c : data) {
                if (isdigit(c)) {
                    sum += c - '0';
                }
            }

            // Simulate passing the sum to Program #2
            std::cout << "Sum of numerical values: " << sum << std::endl;

            // You can place the code here to pass the sum to Program #2

            // Note: You might want to handle the connection to Program #2 here, so it's not affected
            // by Program #2 crashes or restarts.

            // In this example, we'll just print a message to simulate the transfer.
            std::cout << "Sum sent to Program #2." << std::endl;
        }
    }
};

int main() {
    ProgramFirst program;
    program.start();

    return 0;
}
