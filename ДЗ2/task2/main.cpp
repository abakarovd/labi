#include <iostream>
#include <vector>

bool validGrade(double g) { return g >= 0.0 && g <= 5.0; }

double rowAvg(const std::vector<double>& row) {
    if (row.empty()) return 0.0;
    double s = 0;
    for (double v : row) s += v;
    return s / row.size();
}

int main() {
    int n, m;
    std::cin >> n >> m;
    if (!std::cin || n <= 0 || m <= 0) return 1;

    std::vector<std::vector<double>> grades(n, std::vector<double>(m));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            std::cin >> grades[i][j];
            if (!std::cin || !validGrade(grades[i][j])) return 1;
        }
    }

    std::vector<double> studentAvg(n), subjectAvg(m, 0.0);
    for (int i = 0; i < n; ++i) {
        studentAvg[i] = rowAvg(grades[i]);
        for (int j = 0; j < m; ++j) subjectAvg[j] += grades[i][j];
    }
    for (int j = 0; j < m; ++j) subjectAvg[j] /= n;

    int best = 0;
    for (int i = 1; i < n; ++i) if (studentAvg[i] > studentAvg[best]) best = i;

    for (double a : studentAvg) std::cout << a << ' ';
    std::cout << "\n";
    for (double a : subjectAvg) std::cout << a << ' ';
    std::cout << "\nЛучший студент: " << best << "\n";
}
