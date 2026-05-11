#include <condition_variable>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

template <typename T>
class ParallelSum {
public:
    ParallelSum(std::vector<T> data, std::size_t n_threads)
        : data_(std::move(data)), n_threads_(n_threads ? n_threads : 1) {}

    T compute_sum() {
        std::mutex m;
        std::condition_variable cv;
        std::size_t done = 0;
        T total{};

        auto inline_sum = [](const std::vector<T>& v, std::size_t l, std::size_t r) {
            T s{};
            for (std::size_t i = l; i < r; ++i) {
                s += v[i];
                if (i % 128 == 0) std::this_thread::yield();
            }
            return s;
        };

        std::size_t chunk = (data_.size() + n_threads_ - 1) / n_threads_;
        for (std::size_t t = 0; t < n_threads_; ++t) {
            std::size_t l = t * chunk;
            std::size_t r = std::min(data_.size(), l + chunk);
            std::thread([&, l, r] {
                T part = inline_sum(data_, l, r);
                {
                    std::lock_guard<std::mutex> lock(m);
                    total += part;
                    ++done;
                    std::cout << "thread " << std::this_thread::get_id() << " part=" << part << "\n";
                }
                cv.notify_one();
            }).detach();
        }

        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, [&] { return done == n_threads_; });
        return total;
    }

private:
    std::vector<T> data_;
    std::size_t n_threads_;
};

int main() {
    std::vector<double> data(10000, 1.5);
    ParallelSum<double> ps(data, 4);
    std::cout << "Total=" << ps.compute_sum() << "\n";
}
