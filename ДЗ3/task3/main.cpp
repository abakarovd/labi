#include <chrono>
#include <iostream>

int main() {
    long long totalSeconds;
    std::cout << "Введите количество секунд: ";
    std::cin >> totalSeconds;
    if (!std::cin || totalSeconds < 0) return 1;

    std::chrono::seconds sec(totalSeconds);
    auto hours = std::chrono::duration_cast<std::chrono::hours>(sec);
    sec -= hours;
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(sec);
    sec -= minutes;

    std::cout << hours.count() << " час(ов) "
              << minutes.count() << " минут(ы) "
              << sec.count() << " секунд(ы)\n";
}
