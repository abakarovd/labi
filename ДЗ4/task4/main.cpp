#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

template <typename T>
class Logger {
public:
    explicit Logger(const std::string& path) : log_file_(path, std::ios::app) {}

    void log(const T& message) {
        inlineWrite(message);
    }

private:
    inline void inlineWrite(const T& message) {
        std::ostringstream oss;
        oss << message;
        std::lock_guard<std::mutex> lock(m_);
        std::string line = "thread " + toString(std::this_thread::get_id()) + " | " + oss.str();
        std::cout << line << '\n';
        log_file_ << line << '\n';
    }

    static std::string toString(std::thread::id id) {
        std::ostringstream oss;
        oss << id;
        return oss.str();
    }

    std::ofstream log_file_;
    std::mutex m_;
};

int main() {
    Logger<std::string> logger("log.txt");
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&, i] { logger.log("message " + std::to_string(i)); });
    }
    for (auto& t : threads) t.join();
}
