#include <iostream>
#include <vector>
#include <chrono>
#include <boost/thread.hpp>
//харитоноа матрешка
using namespace std;

long long fibonacci(int n) {
    if (n <= 1) {
        return n;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

void fibonacciTask(int m, long long& result) {
    result = fibonacci(m);
}

double runSequential(int numTasks, int fibIndex, vector<long long>& results) {
    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < numTasks; ++i) {
        results[i] = fibonacci(fibIndex);
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    return duration.count();
}

double runParallel(int numThreads, int fibIndex, vector<long long>& results) {
    vector<boost::thread> threads;

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(fibonacciTask, fibIndex, std::ref(results[i]));
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    return duration.count();
}

int main() {

    int fibIndex = 40;
    vector<int> threadCounts = {2, 4, 8};


    for (int numThreads : threadCounts) {
        vector<long long> seqResults(numThreads);
        vector<long long> parResults(numThreads);

        double seqTime = runSequential(numThreads, fibIndex, seqResults);
        double parTime = runParallel(numThreads, fibIndex, parResults);

        cout << "Количество задач/потоков: " << numThreads << "\n";
        cout << "Последовательное время: " << seqTime << " сек\n";
        cout << "Многопоточное время (Boost.Thread): " << parTime << " сек\n";

        cout << "Результаты:\n";
        for (int i = 0; i < numThreads; ++i) {
            cout << "  Поток " << i + 1 << ": Fib(" << fibIndex << ") = " << parResults[i] << "\n";
        }

        if (parTime > 0) {
            cout << "Ускорение: " << seqTime / parTime << "\n";
        }


    }

    return 0;
}