#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <semaphore>
#include <thread>

class ParkingLot {
public:
    explicit ParkingLot(int capacity)
        : capacity_(capacity), occupied_(0), sem_(capacity) {}

    void park(bool isVIP, int timeout_ms) {
        auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
        bool ok = false;
        while (std::chrono::steady_clock::now() < deadline) {
            if (sem_.try_acquire()) {
                {
                    std::lock_guard<std::mutex> lk(m_);
                    ++occupied_;
                    std::cout << "thread " << std::this_thread::get_id() << " " << (isVIP ? "VIP" : "regular")
                              << " parked. occupied=" << occupied_.load() << " free=" << capacity_ - occupied_.load() << "\n";
                }
                ok = true;
                break;
            }
            std::this_thread::yield();
        }
        if (!ok) {
            std::lock_guard<std::mutex> lk(m_);
            std::cout << "thread " << std::this_thread::get_id() << " " << (isVIP ? "VIP" : "regular") << " timeout\n";
        }
    }

    void leave() {
        {
            std::lock_guard<std::mutex> lk(m_);
            if (occupied_ > 0) --occupied_;
            std::cout << "thread " << std::this_thread::get_id() << " leave. occupied=" << occupied_.load()
                      << " free=" << capacity_ - occupied_.load() << "\n";
        }
        sem_.release();
        cv_.notify_all();
    }

private:
    int capacity_;
    std::atomic<int> occupied_;
    std::counting_semaphore<1024> sem_;
    std::mutex m_;
    std::condition_variable cv_;
};

int main() {
    ParkingLot lot(2);
    std::mutex wm;
    std::condition_variable wcv;
    int done = 0;

    auto car = [&](bool vip, int wait_ms, int park_ms) {
        lot.park(vip, wait_ms);
        std::this_thread::sleep_for(std::chrono::milliseconds(park_ms));
        lot.leave();
        {
            std::lock_guard<std::mutex> g(wm);
            ++done;
        }
        wcv.notify_one();
    };

    std::thread(car, true, 100, 40).detach();
    std::thread(car, false, 100, 30).detach();
    std::thread(car, false, 30, 10).detach();

    std::unique_lock<std::mutex> lk(wm);
    wcv.wait(lk, [&] { return done == 3; });
}
