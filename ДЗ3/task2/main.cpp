#include <chrono>
#include <iostream>
#include <thread>

int main() {
    int n;
    std::cout << "Введите секунды: ";
    std::cin >> n;
    if (!std::cin || n < 0) return 1;

    using namespace std::chrono_literals;
    for (int i = n; i > 0; --i) {
        std::cout << "Осталось: " << i << " секунд\n";
        std::this_thread::sleep_for(1s);
    }
    std::cout << "Время вышло!\n";
}
