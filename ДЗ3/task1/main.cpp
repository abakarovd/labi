#include <chrono>
#include <cstdint>
#include <iostream>

std::uint64_t sumToN(std::uint64_t n) {
    std::uint64_t sum = 0;
    for (std::uint64_t i = 1; i <= n; ++i) sum += i;
    return sum;
}

int main() {
    std::uint64_t n;
    std::cout << "Введите N: ";
    std::cin >> n;
    if (!std::cin) return 1;

    const auto start = std::chrono::high_resolution_clock::now();
    const std::uint64_t result = sumToN(n);
    const auto end = std::chrono::high_resolution_clock::now();

    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Сумма: " << result << "\n";
    std::cout << "Время выполнения: " << ms.count() << " миллисекунд\n";
}
