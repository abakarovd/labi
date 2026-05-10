#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <semaphore>
#include <random>
#include <atomic>

using namespace std;
using namespace std::chrono_literals;

constexpr int TRUCKS_COUNT = 30;
constexpr int BASE_CRANES = 5;
constexpr int MAX_CRANES = 6;

counting_semaphore<MAX_CRANES> cranes(BASE_CRANES);

mutex cout_mtx;

atomic<int> waiting_trucks = 0;
atomic<int> loaded_in_port = 2;
atomic<bool> reserve_crane_on = false;
atomic<bool> emergency_mode = true;
atomic<bool> stop_port_monitor = false;

random_device rd;
mt19937 gen(rd());

void safe_print(const string& text) {
    lock_guard<mutex> lock(cout_mtx);
    cout << text << endl;
}

void update_emergency_mode() {
    emergency_mode = (loaded_in_port.load() < 3);
}

void truck(int id) {
    {
        int now_waiting = ++waiting_trucks;
        safe_print("Грузовик " + to_string(id) + " прибыл в порт. Ожидают: " + to_string(now_waiting));

        if (now_waiting > 5 && !reserve_crane_on.exchange(true)) {
            cranes.release(); // добавляем ещё один доступный кран
            safe_print(">>> Включён РЕЗЕРВНЫЙ кран, потому что очередь ожидания стала больше 5.");
        }
    }

    cranes.acquire(); // ждём свободный кран
    --waiting_trucks;

    uniform_int_distribution<int> normal_load(3, 6);
    uniform_int_distribution<int> fast_load(1, 2);

    int load_time = emergency_mode ? fast_load(gen) : normal_load(gen);

    safe_print("Грузовик " + to_string(id) +
               " начал загрузку. Режим: " +
               string(emergency_mode ? "аварийный" : "обычный") +
               ", время = " + to_string(load_time) + " сек.");

    this_thread::sleep_for(chrono::seconds(load_time));

    ++loaded_in_port;
    update_emergency_mode();

    safe_print("Грузовик " + to_string(id) +
               " загружен и ожидает отправки. Загруженных в порту: " +
               to_string(loaded_in_port.load()));

    cranes.release();
}

void departure_controller() {
    // Периодически из порта уезжают уже загруженные грузовики
    while (!stop_port_monitor) {
        this_thread::sleep_for(2s);

        if (loaded_in_port > 0) {
            --loaded_in_port;
            update_emergency_mode();

            safe_print("Из порта уехал загруженный грузовик. Осталось в порту: " +
                       to_string(loaded_in_port.load()));

            if (emergency_mode) {
                safe_print(">>> Включён АВАРИЙНЫЙ режим загрузки: в порту меньше 3 загруженных грузовиков.");
            }
        }
    }
}

int main() {

    safe_print("Старт: 5 обычных кранов, 30 грузовиков.");
    safe_print("Изначально загруженных грузовиков в порту: " + to_string(loaded_in_port.load()));

    thread depart_thread(departure_controller);

    vector<thread> trucks;

    for (int i = 1; i <= TRUCKS_COUNT; ++i) {
        trucks.emplace_back(truck, i);
        this_thread::sleep_for(300ms);
    }

    for (auto& t : trucks) {
        t.join();
    }

    stop_port_monitor = true;
    depart_thread.join();

    safe_print("Все грузовики обработаны");
    return 0;
}