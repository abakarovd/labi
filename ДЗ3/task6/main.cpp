#include <cstdint>
#include <iostream>
#include <vector>

class VirtualThread {
public:
    VirtualThread(std::vector<int> nums, int id) : numbers_(std::move(nums)), id_(id) {}

    bool hasWork() const { return index_ < numbers_.size(); }

    void run() {
        if (!hasWork()) return;
        int n = numbers_[index_++];
        std::uint64_t f = factorial(n);
        std::cout << "Виртуальный поток " << id_ << " вычисляет " << n << "! = " << f << "\n";
    }

private:
    static std::uint64_t factorial(int n) {
        std::uint64_t r = 1;
        for (int i = 2; i <= n; ++i) r *= static_cast<std::uint64_t>(i);
        return r;
    }

    std::vector<int> numbers_;
    std::size_t index_ = 0;
    int id_;
};

class HyperThreadingSimulator {
public:
    HyperThreadingSimulator(VirtualThread t1, VirtualThread t2) : t1_(std::move(t1)), t2_(std::move(t2)) {}

    void execute() {
        while (t1_.hasWork() || t2_.hasWork()) {
            if (t1_.hasWork()) t1_.run();
            if (t2_.hasWork()) t2_.run();
        }
    }

private:
    VirtualThread t1_;
    VirtualThread t2_;
};

int main() {
    VirtualThread vt1({5, 10}, 1);
    VirtualThread vt2({7, 12}, 2);
    HyperThreadingSimulator sim(std::move(vt1), std::move(vt2));
    sim.execute();
}
