#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <semaphore>
#include <string>
#include <thread>

class PrinterQueue {
public:
    explicit PrinterQueue(int n_printers)
        : sem_(n_printers) {}

    void printJob(std::string doc, int priority, int timeout_ms) {
        auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
        while (std::chrono::steady_clock::now() < deadline) {
            if (sem_.try_acquire()) {
                std::cout << "thread " << std::this_thread::get_id() << " pr=" << priority << " printing " << doc << "\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                sem_.release();
                return;
            }
            std::this_thread::yield();
        }
        std::lock_guard<std::mutex> lk(m_);
        waiting_.push({priority, std::move(doc)});
        std::cout << "thread " << std::this_thread::get_id() << " pr=" << priority << " interrupted\n";
    }

private:
    std::counting_semaphore<1024> sem_;
    std::mutex m_;
    std::priority_queue<std::pair<int, std::string>> waiting_;
};

int main() {
    PrinterQueue pq(2);
    std::mutex wm;
    std::condition_variable wcv;
    int done = 0;

    auto submit = [&](std::string d, int p) {
        pq.printJob(std::move(d), p, 50);
        { std::lock_guard<std::mutex> g(wm); ++done; }
        wcv.notify_one();
    };

    std::thread(submit, "docA", 1).detach();
    std::thread(submit, "docB", 5).detach();
    std::thread(submit, "docC", 10).detach();

    std::unique_lock<std::mutex> lk(wm);
    wcv.wait(lk, [&] { return done == 3; });
}
