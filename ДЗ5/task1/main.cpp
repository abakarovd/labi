#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <semaphore>
#include <thread>
#include <vector>

template <typename T>
class ResourcePool {
public:
    explicit ResourcePool(std::vector<T> resources)
        : resources_(std::move(resources)), sem_(static_cast<std::ptrdiff_t>(resources_.size())) {}

    std::optional<T> acquire(int priority, int timeout_ms) {
        auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
        while (std::chrono::steady_clock::now() < deadline) {
            if (sem_.try_acquire()) {
                std::lock_guard<std::mutex> lk(m_);
                if (!resources_.empty()) {
                    T r = resources_.back();
                    resources_.pop_back();
                    std::cout << "thread " << std::this_thread::get_id() << " pr=" << priority << " acquire\n";
                    return r;
                }
                sem_.release();
            }
            std::this_thread::yield();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        ++failed_attempts_;
        return std::nullopt;
    }

    void release(T res) {
        {
            std::lock_guard<std::mutex> lk(m_);
            resources_.push_back(std::move(res));
            std::cout << "thread " << std::this_thread::get_id() << " release\n";
        }
        sem_.release();
        cv_.notify_all();
    }

    void add_resource(T res) {
        std::lock_guard<std::mutex> lk(m_);
        resources_.push_back(std::move(res));
        sem_.release();
    }

    int failed_attempts() const { return failed_attempts_.load(); }

private:
    std::vector<T> resources_;
    std::counting_semaphore<1024> sem_;
    std::mutex m_;
    std::condition_variable cv_;
    std::atomic<int> failed_attempts_{0};
};

int main() {
    ResourcePool<int> pool({1, 2});
    std::mutex wm;
    std::condition_variable wcv;
    int done = 0;

    auto worker = [&](int priority) {
        auto r = pool.acquire(priority, 50);
        if (r) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            pool.release(*r);
        }
        {
            std::lock_guard<std::mutex> g(wm);
            ++done;
        }
        wcv.notify_one();
    };

    std::thread(worker, 10).detach();
    std::thread(worker, 5).detach();
    std::thread(worker, 1).detach();

    std::unique_lock<std::mutex> lk(wm);
    wcv.wait(lk, [&] { return done == 3; });
    std::cout << "failed_attempts=" << pool.failed_attempts() << "\n";
}
