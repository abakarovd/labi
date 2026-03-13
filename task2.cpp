
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <chrono>

using namespace std;

const int INCREMENTS = 1000000;

int unsafeCounter = 0;

void unsafeIncrement() {
    for (int i = 0; i < INCREMENTS; ++i) {
        unsafeCounter++;
    }
}

double testUnsafe(int numThreads) {
    unsafeCounter = 0;
    vector<thread> threads;

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(unsafeIncrement);
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << "[Без синхронизации]\n";
    cout << "Итоговый счётчик: " << unsafeCounter << "\n";
    cout << "Ожидаемое значение: " << numThreads * INCREMENTS << "\n";
    cout << "Время: " << duration.count() << " сек\n\n";

    return duration.count();
}

atomic<int> atomicCounter(0);

void atomicIncrement() {
    for (int i = 0; i < INCREMENTS; ++i) {
        atomicCounter++;
    }
}

double testAtomic(int numThreads) {
    atomicCounter = 0;
    vector<thread> threads;

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(atomicIncrement);
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << "[std::atomic]\n";
    cout << "Итоговый счётчик: " << atomicCounter.load() << "\n";
    cout << "Ожидаемое значение: " << numThreads * INCREMENTS << "\n";
    cout << "Время: " << duration.count() << " сек\n\n";

    return duration.count();
}

int mutexCounter = 0;
mutex mtx;

void mutexIncrement() {
    for (int i = 0; i < INCREMENTS; ++i) {
        lock_guard<mutex> lock(mtx);
        mutexCounter++;
    }
}

double testMutex(int numThreads) {
    mutexCounter = 0;
    vector<thread> threads;

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(mutexIncrement);
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << "[std::mutex]\n";
    cout << "Итоговый счётчик: " << mutexCounter << "\n";
    cout << "Ожидаемое значение: " << numThreads * INCREMENTS << "\n";
    cout << "Время: " << duration.count() << " сек\n\n";

    return duration.count();
}

int main() {
    setlocale(LC_ALL, "Russian");

    vector<int> threadCounts = {2, 4, 8};

    cout << "=== Задача 2. Конкурентный счётчик ===\n\n";

    for (int numThreads : threadCounts) {
        cout << "Количество потоков: " << numThreads << "\n";

        double t1 = testUnsafe(numThreads);
        double t2 = testAtomic(numThreads);
        double t3 = testMutex(numThreads);

        cout << "Сравнение времени:\n";
        cout << "Без синхронизации: " << t1 << " сек\n";
        cout << "std::atomic:       " << t2 << " сек\n";
        cout << "std::mutex:        " << t3 << " сек\n";
    }

    return 0;
}