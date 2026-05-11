#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

template <typename T>
class PriorityQueue {
public:
    void push(T value, int priority) {
        {
            std::lock_guard<std::mutex> lock(m_);
            q_.push_back({std::move(value), priority});
            std::cout << "thread " << std::this_thread::get_id() << " push p=" << priority << "\n";
        }
        cv_.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(m_);
        cv_.wait(lock, [&] { return !q_.empty(); });
        std::size_t idx = 0;
        for (std::size_t i = 1; i < q_.size(); ++i) if (q_[i].priority > q_[idx].priority) idx = i;
        auto node = q_[idx];
        q_.erase(q_.begin() + static_cast<long>(idx));
        std::cout << "thread " << std::this_thread::get_id() << " pop p=" << node.priority << "\n";
        return node.value;
    }

private:
    struct Node { T value; int priority; };
    std::vector<Node> q_;
    std::mutex m_;
    std::condition_variable cv_;
};

int main() {
    PriorityQueue<std::string> pq;
    std::thread([&] { pq.push("A", 2); }).detach();
    std::thread([&] { pq.push("B", 5); }).detach();
    std::thread([&] { pq.push("C", 1); }).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    std::cout << pq.pop() << '\n';
    std::cout << pq.pop() << '\n';
    std::cout << pq.pop() << '\n';
}
