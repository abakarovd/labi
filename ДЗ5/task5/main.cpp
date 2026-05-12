#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <semaphore>
#include <thread>
#include <vector>

struct Task {
    int id;
    int required_slots;
    int duration_ms;
    int priority;

    void execute() const {
        std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
    }

    bool operator<(const Task& other) const {
        return priority < other.priority;
    }
};

class TaskScheduler {
public:
    explicit TaskScheduler(int slots)
        : resource_semaphore_(slots) {}

    void submit(Task task) {
        std::lock_guard<std::mutex> lk(queue_mutex_);
        queue_.push(task);
    }

    void worker() {
        while (true) {
            Task task;
            {
                std::lock_guard<std::mutex> lk(queue_mutex_);
                if (queue_.empty()) return;
                task = queue_.top();
                queue_.pop();
            }

            for (int i = 0; i < task.required_slots; ++i) resource_semaphore_.acquire();
            execute_task(task);
            for (int i = 0; i < task.required_slots; ++i) resource_semaphore_.release();

            ++completed_tasks_;
            std::this_thread::yield();
        }
    }

    inline void execute_task(Task& task) {
        std::cout << "thread " << std::this_thread::get_id() << " task=" << task.id
                  << " slots=" << task.required_slots << " start\n";
        task.execute();
        std::cout << "thread " << std::this_thread::get_id() << " task=" << task.id << " done\n";
    }

    int completed() const { return completed_tasks_.load(); }

private:
    std::priority_queue<Task> queue_;
    std::counting_semaphore<1024> resource_semaphore_;
    std::mutex queue_mutex_;
    std::atomic<int> completed_tasks_{0};
};

int main() {
    TaskScheduler scheduler(3);
    scheduler.submit({1, 1, 20, 5});
    scheduler.submit({2, 2, 40, 9});
    scheduler.submit({3, 1, 10, 1});

    std::mutex wm;
    std::condition_variable wcv;
    int done = 0;

    auto run_worker = [&] {
        scheduler.worker();
        { std::lock_guard<std::mutex> g(wm); ++done; }
        wcv.notify_one();
    };

    std::thread(run_worker).detach();
    std::thread(run_worker).detach();

    std::unique_lock<std::mutex> lk(wm);
    wcv.wait(lk, [&] { return done == 2; });
    std::cout << "completed=" << scheduler.completed() << "\n";
}
