#include <iostream>
#include <limits>

bool readInt(const char* prompt, int& value) {
    std::cout << prompt;
    std::cin >> value;
    if (!std::cin) return false;
    return true;
}

bool readGrade(const char* prompt, double& value) {
    std::cout << prompt;
    std::cin >> value;
    return std::cin && value >= 0.0 && value <= 5.0;
}

double avg(const double* a, int n) {
    if (n <= 0) return 0.0;
    double s = 0;
    for (int i = 0; i < n; ++i) s += a[i];
    return s / n;
}

double minv(const double* a, int n) {
    double m = a[0];
    for (int i = 1; i < n; ++i) if (a[i] < m) m = a[i];
    return m;
}

double maxv(const double* a, int n) {
    double m = a[0];
    for (int i = 1; i < n; ++i) if (a[i] > m) m = a[i];
    return m;
}

int countAbove(const double* a, int n, double threshold) {
    int c = 0;
    for (int i = 0; i < n; ++i) if (a[i] > threshold) ++c;
    return c;
}

int main() {
    int n;
    if (!readInt("N = ", n) || n <= 0) {
        std::cout << "Ошибка: N должно быть > 0\n";
        return 1;
    }

    double* grades = new double[n];
    for (int i = 0; i < n; ++i) {
        while (true) {
            if (readGrade("Оценка: ", grades[i])) break;
            std::cout << "Некорректный ввод. Повторите.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    double t;
    std::cout << "Порог: ";
    std::cin >> t;

    std::cout << "Среднее: " << avg(grades, n) << "\n";
    std::cout << "Мин: " << minv(grades, n) << "\n";
    std::cout << "Макс: " << maxv(grades, n) << "\n";
    std::cout << "Выше порога: " << countAbove(grades, n, t) << "\n";

    delete[] grades;
    return 0;
}
