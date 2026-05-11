#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

template <typename T>
class MatrixProcessor {
public:
    MatrixProcessor(std::vector<std::vector<T>> matrix, std::size_t threads)
        : matrix_(std::move(matrix)), n_threads_(threads ? threads : 1) {}

    void apply(const std::function<T(T)>& func) {
        std::mutex m;
        std::condition_variable cv;
        std::size_t done = 0;
        std::size_t rows = matrix_.size();
        std::size_t chunk = (rows + n_threads_ - 1) / n_threads_;

        for (std::size_t t = 0; t < n_threads_; ++t) {
            std::size_t l = t * chunk;
            std::size_t r = std::min(rows, l + chunk);
            std::thread([&, l, r] {
                if (l < r) std::cout << "thread " << std::this_thread::get_id() << " start\n";
                for (std::size_t i = l; i < r; ++i) {
                    for (std::size_t j = 0; j < matrix_[i].size(); ++j) {
                        matrix_[i][j] = func(matrix_[i][j]);
                        if (j % 8 == 0) std::this_thread::yield();
                    }
                }
                if (l < r) std::cout << "thread " << std::this_thread::get_id() << " end\n";
                {
                    std::lock_guard<std::mutex> lock(m);
                    ++done;
                }
                cv.notify_one();
            }).detach();
        }

        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&] { return done == n_threads_; });
    }

    void print() const {
        for (const auto& row : matrix_) {
            for (T v : row) std::cout << v << ' ';
            std::cout << '\n';
        }
    }

private:
    std::vector<std::vector<T>> matrix_;
    std::size_t n_threads_;
};

int main() {
    MatrixProcessor<int> mp({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, 2);
    mp.apply([](int x) { return x * x; });
    mp.print();
}
