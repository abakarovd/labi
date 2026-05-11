#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

template <typename T>
class Account {
public:
    explicit Account(T balance = 0) : balance_(balance) {}

    T get_balance_unsafe() const { return balance_; }
    void deposit(T amount) { balance_ += amount; }
    bool withdraw(T amount) {
        if (balance_ < amount) return false;
        balance_ -= amount;
        return true;
    }

private:
    T balance_{};
    template <typename U>
    friend class Bank;
};

template <typename T>
class Bank {
public:
    explicit Bank(std::vector<T> initial) {
        for (T b : initial) accounts_.emplace_back(b);
    }

    void transfer(int from, int to, T amount) {
        auto inline_transfer = [&](int f, int t, T a) {
            std::unique_lock<std::mutex> lock(m_);
            cv_.wait(lock, [&] { return accounts_[f].balance_ >= a; });
            accounts_[f].withdraw(a);
            accounts_[t].deposit(a);
            std::cout << "thread " << std::this_thread::get_id() << " transfer " << a << " from " << f << " to " << t << "\n";
            cv_.notify_all();
        };
        inline_transfer(from, to, amount);
    }

    T total() {
        std::lock_guard<std::mutex> lock(m_);
        T s{};
        for (const auto& a : accounts_) s += a.balance_;
        return s;
    }

private:
    std::vector<Account<T>> accounts_;
    std::mutex m_;
    std::condition_variable cv_;
};

int main() {
    Bank<int> bank({100, 100, 100});
    const int initial = bank.total();

    std::mutex wait_m;
    std::condition_variable wait_cv;
    int done = 0;

    auto client = [&](int from, int to, int amount) {
        bank.transfer(from, to, amount);
        {
            std::lock_guard<std::mutex> lg(wait_m);
            ++done;
        }
        wait_cv.notify_one();
    };

    std::thread(client, 0, 1, 30).detach();
    std::thread(client, 1, 2, 50).detach();
    std::thread(client, 2, 0, 20).detach();

    std::unique_lock<std::mutex> lk(wait_m);
    wait_cv.wait(lk, [&] { return done == 3; });

    std::cout << "Initial total=" << initial << " Final total=" << bank.total() << "\n";
}
