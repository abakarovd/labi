#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<double> avg = {2.9, 4.6, 3.7, 4.8, 2.4, 4.5};
    double threshold = 3.0;

    avg.erase(std::remove_if(avg.begin(), avg.end(), [threshold](double v) { return v < threshold; }), avg.end());

    int excellent = static_cast<int>(std::count_if(avg.begin(), avg.end(), [](double v) { return v >= 4.5; }));
    int risk = static_cast<int>(std::count_if(avg.begin(), avg.end(), [](double v) { return v < 3.0; }));

    std::cout << "После фильтрации: ";
    for (double v : avg) std::cout << v << ' ';
    std::cout << "\nОтличники: " << excellent << "\nРиск отчисления: " << risk << "\n";
}
