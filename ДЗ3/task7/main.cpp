#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

class VirtualThread {
public:
    VirtualThread(std::vector<std::string> tasks, int id) : tasks_(std::move(tasks)), id_(id) {}

    bool hasWork() const { return index_ < tasks_.size(); }

    void runNextTask() {
        if (!hasWork()) return;
        const std::string& task = tasks_[index_++];
        std::cout << "Виртуальный поток " << id_ << " начал " << task << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::cout << "Виртуальный поток " << id_ << " закончил " << task << "\n";
    }

private:
    std::vector<std::string> tasks_;
    std::size_t index_ = 0;
    int id_;
};

class HyperThreadingSimulator {
public:
    HyperThreadingSimulator(VirtualThread a, VirtualThread b) : t1_(std::move(a)), t2_(std::move(b)) {}

    void execute() {
        while (t1_.hasWork() || t2_.hasWork()) {
            if (t1_.hasWork()) t1_.runNextTask();
            if (t2_.hasWork()) t2_.runNextTask();
        }
    }

private:
    VirtualThread t1_;
    VirtualThread t2_;
};

int main() {
    VirtualThread t1({"Задача A", "Задача C"}, 1);
    VirtualThread t2({"Задача B", "Задача D"}, 2);
    HyperThreadingSimulator simulator(std::move(t1), std::move(t2));
    simulator.execute();
}
