#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <vector>

void bubbleSort(std::vector<int>& a) {
    const std::size_t n = a.size();
    for (std::size_t i = 0; i < n; ++i) {
        bool swapped = false;
        for (std::size_t j = 1; j < n - i; ++j) {
            if (a[j - 1] > a[j]) {
                std::swap(a[j - 1], a[j]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

void insertionSort(std::vector<int>& a) {
    for (std::size_t i = 1; i < a.size(); ++i) {
        int key = a[i];
        std::size_t j = i;
        while (j > 0 && a[j - 1] > key) {
            a[j] = a[j - 1];
            --j;
        }
        a[j] = key;
    }
}

void merge(std::vector<int>& a, int l, int m, int r) {
    std::vector<int> left(a.begin() + l, a.begin() + m + 1);
    std::vector<int> right(a.begin() + m + 1, a.begin() + r + 1);
    int i = 0, j = 0, k = l;
    while (i < static_cast<int>(left.size()) && j < static_cast<int>(right.size())) {
        if (left[i] <= right[j]) a[k++] = left[i++];
        else a[k++] = right[j++];
    }
    while (i < static_cast<int>(left.size())) a[k++] = left[i++];
    while (j < static_cast<int>(right.size())) a[k++] = right[j++];
}

void mergeSortRec(std::vector<int>& a, int l, int r) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    mergeSortRec(a, l, m);
    mergeSortRec(a, m + 1, r);
    merge(a, l, m, r);
}

void mergeSort(std::vector<int>& a) {
    if (!a.empty()) mergeSortRec(a, 0, static_cast<int>(a.size()) - 1);
}

int partition(std::vector<int>& a, int low, int high) {
    int pivot = a[high];
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        if (a[j] < pivot) {
            ++i;
            std::swap(a[i], a[j]);
        }
    }
    std::swap(a[i + 1], a[high]);
    return i + 1;
}

void quickSortRec(std::vector<int>& a, int low, int high) {
    if (low < high) {
        int pi = partition(a, low, high);
        quickSortRec(a, low, pi - 1);
        quickSortRec(a, pi + 1, high);
    }
}

void quickSort(std::vector<int>& a) {
    if (!a.empty()) quickSortRec(a, 0, static_cast<int>(a.size()) - 1);
}

std::vector<int> makeRandomArray(std::size_t n) {
    std::vector<int> data(n);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 1000000);
    for (int& x : data) x = dist(gen);
    return data;
}

long long measureMs(const std::vector<int>& base, const std::function<void(std::vector<int>&)>& sorter) {
    auto copy = base;
    const auto start = std::chrono::high_resolution_clock::now();
    sorter(copy);
    const auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

int main() {
    const std::size_t n = 100000;
    const auto base = makeRandomArray(n);

    std::cout << "N = " << n << "\n";
    std::cout << "Bubble Sort: " << measureMs(base, bubbleSort) << " ms\n";
    std::cout << "Insertion Sort: " << measureMs(base, insertionSort) << " ms\n";
    std::cout << "Merge Sort: " << measureMs(base, mergeSort) << " ms\n";
    std::cout << "Quick Sort: " << measureMs(base, quickSort) << " ms\n";
    std::cout << "std::sort: " << measureMs(base, [](std::vector<int>& v) { std::sort(v.begin(), v.end()); }) << " ms\n";
}
