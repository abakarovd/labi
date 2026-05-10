#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

class TaskTimer {
public:
    void start() { start_ = std::chrono::high_resolution_clock::now(); }
    void stop() { end_ = std::chrono::high_resolution_clock::now(); }

    long long getDurationMs() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(end_ - start_).count();
    }

    long long measureSorting(std::vector<int> data) {
        start();
        std::sort(data.begin(), data.end());
        stop();
        return getDurationMs();
    }

private:
    std::chrono::high_resolution_clock::time_point start_{};
    std::chrono::high_resolution_clock::time_point end_{};
};

std::vector<int> makeRandomVector(std::size_t n) {
    std::vector<int> v(n);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 1000000);
    for (int& x : v) x = dist(gen);
    return v;
}

int main() {
    TaskTimer timer;

    auto a = makeRandomVector(100000);
    std::cout << "Время выполнения сортировки: " << timer.measureSorting(a) << " миллисекунд\n";

    auto b = makeRandomVector(100000);
    std::cout << "Время выполнения сортировки: " << timer.measureSorting(b) << " миллисекунд\n";
}
