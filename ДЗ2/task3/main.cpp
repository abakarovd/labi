#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

int main() {
    std::vector<double> avg = {4.2, 4.9, 4.9, 3.8, 4.5};
    std::vector<std::pair<int, double>> order;
    for (int i = 0; i < static_cast<int>(avg.size()); ++i) order.push_back({i, avg[i]});

    std::sort(order.begin(), order.end(), [](const auto& a, const auto& b) {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first;
    });

    for (const auto& [idx, val] : order) std::cout << idx << ": " << val << "\n";

    int shift = 1;
    auto byValueCapture = [shift](double x) { return x + shift; };
    auto byRefCapture = [&shift](double x) { return x + (++shift); };
    std::cout << byValueCapture(1.0) << " " << byRefCapture(1.0) << "\n";
}
