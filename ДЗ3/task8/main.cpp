#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

struct Task {
    int value = 0;
    int priority = 0;
    int durationMs = 0;
    int steps = 0;
    int currentStep = 0;
};

class VirtualThread {
public:
    VirtualThread(std::vector<Task> tasks) : tasks_(std::move(tasks)) {}

    bool hasTasks() const { return !tasks_.empty(); }

    void runStep(int threadId) {
        if (tasks_.empty()) return;

        auto it = std::max_element(tasks_.begin(), tasks_.end(), [](const Task& a, const Task& b) {
            return a.priority < b.priority;
        });

        Task& t = *it;
        ++t.currentStep;

        std::cout << "Виртуальный поток " << threadId
                  << " выполняет шаг " << t.currentStep << '/' << t.steps
                  << " задачи " << t.value
                  << " с приоритетом " << t.priority << "\n";

        const int stepDelay = std::max(1, t.durationMs / t.steps);
        std::this_thread::sleep_for(std::chrono::milliseconds(stepDelay));

        if (t.currentStep >= t.steps) {
            const int result = static_cast<int>(t.value * t.value);
            std::cout << "Виртуальный поток " << threadId
                      << " завершил задачу " << t.value
                      << ": результат = " << result << "\n";
            tasks_.erase(it);
        }
    }

private:
    std::vector<Task> tasks_;
};

class HyperThreadingSimulator {
public:
    HyperThreadingSimulator(VirtualThread t1, VirtualThread t2)
        : vt1_(std::move(t1)), vt2_(std::move(t2)) {}

    void execute() {
        while (vt1_.hasTasks() || vt2_.hasTasks()) {
            if (vt1_.hasTasks()) vt1_.runStep(1);
            if (vt2_.hasTasks()) vt2_.runStep(2);
        }
    }

private:
    VirtualThread vt1_;
    VirtualThread vt2_;
};

std::vector<Task> generateTasks(int count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> valueDist(1, 50);
    std::uniform_int_distribution<> durationDist(200, 1000);
    std::uniform_int_distribution<> priorityDist(1, 10);
    std::uniform_int_distribution<> stepsDist(2, 5);

    std::vector<Task> tasks;
    tasks.reserve(count);
    for (int i = 0; i < count; ++i) {
        tasks.push_back(Task{valueDist(gen), priorityDist(gen), durationDist(gen), stepsDist(gen), 0});
    }
    return tasks;
}

int main() {
    VirtualThread t1(generateTasks(4));
    VirtualThread t2(generateTasks(4));
    HyperThreadingSimulator simulator(std::move(t1), std::move(t2));
    simulator.execute();
}
