#include <condition_variable>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

template <typename T>
class Buffer {
public:
    explicit Buffer(std::size_t capacity) : capacity_(capacity) {}

    void produce(T value) {
        std::unique_lock<std::mutex> lock(m_);
        while (buffer_.size() >= capacity_) {
            lock.unlock();
            std::this_thread::yield();
            lock.lock();
            cv_.wait_for(lock, std::chrono::milliseconds(1));
        }
        buffer_.push_back(value);
        std::cout << "thread " << std::this_thread::get_id() << " produced " << value << "\n";
        cv_.notify_all();
    }

    T consume() {
        std::unique_lock<std::mutex> lock(m_);
        while (buffer_.empty()) {
            lock.unlock();
            std::this_thread::yield();
            lock.lock();
            cv_.wait_for(lock, std::chrono::milliseconds(1));
        }
        T v = buffer_.front();
        buffer_.erase(buffer_.begin());
        std::cout << "thread " << std::this_thread::get_id() << " consumed " << v << "\n";
        cv_.notify_all();
        return v;
    }

private:
    std::vector<T> buffer_;
    std::size_t capacity_;
    std::mutex m_;
    std::condition_variable cv_;
};

int main() {
    Buffer<int> buf(4);
    std::mutex wm;
    std::condition_variable wcv;
    int done = 0;

    auto producer = [&](int base) {
        for (int i = 0; i < 5; ++i) buf.produce(base + i);
        { std::lock_guard<std::mutex> g(wm); ++done; }
        wcv.notify_one();
    };
    auto consumer = [&] {
        for (int i = 0; i < 5; ++i) (void)buf.consume();
        { std::lock_guard<std::mutex> g(wm); ++done; }
        wcv.notify_one();
    };

    std::thread(producer, 100).detach();
    std::thread(producer, 200).detach();
    std::thread(consumer).detach();
    std::thread(consumer).detach();

    std::unique_lock<std::mutex> lk(wm);
    wcv.wait(lk, [&] { return done == 4; });
}
