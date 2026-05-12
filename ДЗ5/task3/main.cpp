#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <optional>
#include <random>
#include <semaphore>
#include <thread>
#include <vector>

template <typename T>
class SemaphoreBuffer {
public:
    SemaphoreBuffer(int k, int per_buffer_capacity)
        : buffers_(k), mtx_(k), empty_(), full_(), timeouts_(0) {
        for (int i = 0; i < k; ++i) {
            empty_.push_back(std::make_unique<std::counting_semaphore<1024>>(per_buffer_capacity));
            full_.push_back(std::make_unique<std::counting_semaphore<1024>>(0));
        }
    }

    void produce(T value, int idx, int timeout_ms) {
        auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
        while (std::chrono::steady_clock::now() < deadline) {
            if (empty_[idx]->try_acquire()) {
                {
                    std::lock_guard<std::mutex> lk(mtx_[idx]);
                    buffers_[idx].push_back(value);
                    std::cout << "thread " << std::this_thread::get_id() << " buffer " << idx << " produce " << value << "\n";
                }
                full_[idx]->release();
                return;
            }
            std::this_thread::yield();
        }
        ++timeouts_;
    }

    std::optional<T> consume(int idx, int timeout_ms) {
        auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
        while (std::chrono::steady_clock::now() < deadline) {
            if (full_[idx]->try_acquire()) {
                T v;
                {
                    std::lock_guard<std::mutex> lk(mtx_[idx]);
                    v = buffers_[idx].back();
                    buffers_[idx].pop_back();
                    std::cout << "thread " << std::this_thread::get_id() << " buffer " << idx << " consume " << v << "\n";
                }
                empty_[idx]->release();
                return v;
            }
            std::this_thread::yield();
        }
        ++timeouts_;
        return std::nullopt;
    }

    int timeouts() const { return timeouts_.load(); }

private:
    std::vector<std::vector<T>> buffers_;
    std::vector<std::mutex> mtx_;
    std::vector<std::unique_ptr<std::counting_semaphore<1024>>> empty_;
    std::vector<std::unique_ptr<std::counting_semaphore<1024>>> full_;
    std::atomic<int> timeouts_;
};

int main() {
    SemaphoreBuffer<int> sb(2, 2);
    std::mutex wm;
    std::condition_variable wcv;
    int done = 0;

    auto producer = [&](int base) {
        for (int i = 0; i < 3; ++i) sb.produce(base + i, i % 2, 100);
        { std::lock_guard<std::mutex> g(wm); ++done; }
        wcv.notify_one();
    };
    auto consumer = [&] {
        for (int i = 0; i < 3; ++i) (void)sb.consume(i % 2, 100);
        { std::lock_guard<std::mutex> g(wm); ++done; }
        wcv.notify_one();
    };

    std::thread(producer, 100).detach();
    std::thread(producer, 200).detach();
    std::thread(consumer).detach();
    std::thread(consumer).detach();

    std::unique_lock<std::mutex> lk(wm);
    wcv.wait(lk, [&] { return done == 4; });
    std::cout << "timeouts=" << sb.timeouts() << "\n";
}
