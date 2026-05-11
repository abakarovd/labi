#include <condition_variable>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>

template <typename Key, typename Value>
class Cache {
public:
    inline void set(const Key& k, const Value& v) {
        {
            std::lock_guard<std::mutex> lock(m_);
            data_[k] = v;
            std::cout << "thread " << std::this_thread::get_id() << " set " << k << "=" << v << "\n";
        }
        cv_.notify_all();
    }

    inline Value get(const Key& k) {
        std::unique_lock<std::mutex> lock(m_);
        while (!data_.count(k)) {
            lock.unlock();
            std::this_thread::yield();
            lock.lock();
            cv_.wait_for(lock, std::chrono::milliseconds(1));
        }
        std::cout << "thread " << std::this_thread::get_id() << " get " << k << "=" << data_[k] << "\n";
        return data_[k];
    }

    void printAll() {
        std::lock_guard<std::mutex> lock(m_);
        for (const auto& [k, v] : data_) std::cout << k << " => " << v << "\n";
    }

private:
    std::map<Key, Value> data_;
    std::mutex m_;
    std::condition_variable cv_;
};

int main() {
    Cache<int, int> cache;
    std::mutex wm;
    std::condition_variable wcv;
    int done = 0;

    std::thread([&] { (void)cache.get(1); { std::lock_guard<std::mutex> g(wm); ++done; } wcv.notify_one(); }).detach();
    std::thread([&] { std::this_thread::sleep_for(std::chrono::milliseconds(30)); cache.set(1, 42); { std::lock_guard<std::mutex> g(wm); ++done; } wcv.notify_one(); }).detach();

    std::unique_lock<std::mutex> lk(wm);
    wcv.wait(lk, [&] { return done == 2; });
    cache.printAll();
}
