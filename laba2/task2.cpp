#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <random>
#include <atomic>

using namespace std;
using namespace std::chrono_literals;

constexpr int CAMERAS = 6;
constexpr int ACCELERATORS = 3;
constexpr int FRAMES_PER_CAMERA = 8;
constexpr int HIGH_LOAD_THRESHOLD = 10;

struct FrameTask {
    int camera_id;
    int frame_id;
    int priority;   // 1 - важный, 2 - обычный
    int duration;
};

struct CompareFrameTask {
    bool operator()(const FrameTask& a, const FrameTask& b) const {
        return a.priority > b.priority;
    }
};

priority_queue<FrameTask, vector<FrameTask>, CompareFrameTask> task_queue;

mutex queue_mtx;
mutex cout_mtx;
condition_variable cv;

atomic<int> cameras_finished = 0;
atomic<bool> accelerator_failed = false;
atomic<int> failed_accelerator_id = -1;

void safe_print(const string& text) {
    lock_guard<mutex> lock(cout_mtx);
    cout << text << endl;
}

void camera_stream(int camera_id) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> pause_dis(200, 700);
    uniform_int_distribution<int> proc_dis(1, 3);

    for (int frame = 1; frame <= FRAMES_PER_CAMERA; ++frame) {
        int priority = (frame % 2 == 0 ? 1 : 2); // чётные кадры считаем важными
        int duration = proc_dis(gen);

        {
            lock_guard<mutex> lock(queue_mtx);
            task_queue.push({camera_id, frame, priority, duration});
        }

        safe_print("Камера " + to_string(camera_id) +
                   " отправила кадр " + to_string(frame) +
                   " (приоритет " + to_string(priority) + ").");

        cv.notify_all();
        this_thread::sleep_for(chrono::milliseconds(pause_dis(gen)));
    }

    ++cameras_finished;
    cv.notify_all();
}

void accelerator_worker(int accelerator_id,
                        int selected_accelerator_for_failure,
                        int break_after_tasks) {
    random_device rd;
    mt19937 gen(rd());
    bernoulli_distribution fail_after_limit(0.3); // 30% после достижения порога

    int processed_tasks = 0;

    while (true) {
        FrameTask task;

        {
            unique_lock<mutex> lock(queue_mtx);

            cv.wait(lock, [] {
                return !task_queue.empty() || cameras_finished == CAMERAS;
            });

            if (task_queue.empty()) {
                if (cameras_finished == CAMERAS) {
                    return;
                }
                continue;
            }

            bool should_fail =
                    !accelerator_failed &&
                    accelerator_id == selected_accelerator_for_failure &&
                    processed_tasks >= break_after_tasks &&
                    (
                            fail_after_limit(gen) ||
                            processed_tasks >= break_after_tasks + 3
                    );

            if (should_fail) {
                accelerator_failed = true;
                failed_accelerator_id = accelerator_id;

                safe_print("!!! Ускоритель " + to_string(accelerator_id) +
                           " вышел из строя во время работы. "
                           "Его задачи будут обработаны другими ускорителями.");
                return;
            }

            task = task_queue.top();
            task_queue.pop();
        }

        safe_print("Ускоритель " + to_string(accelerator_id) +
                   " обрабатывает кадр " + to_string(task.frame_id) +
                   " с камеры " + to_string(task.camera_id) +
                   " (приоритет " + to_string(task.priority) + ").");

        this_thread::sleep_for(chrono::seconds(task.duration));
        ++processed_tasks;

        safe_print("Ускоритель " + to_string(accelerator_id) +
                   " завершил кадр " + to_string(task.frame_id) +
                   " с камеры " + to_string(task.camera_id) + ".");
    }
}

void global_monitor() {
    bool failure_reported = false;
    auto last_load_report = chrono::steady_clock::now() - 3s;

    while (true) {
        int current_queue_size;
        bool current_failure_state;
        int current_failed_id;

        {
            lock_guard<mutex> lock(queue_mtx);
            current_queue_size = static_cast<int>(task_queue.size());
            current_failure_state = accelerator_failed.load();
            current_failed_id = failed_accelerator_id.load();
        }

        auto now = chrono::steady_clock::now();

        if (current_failure_state && !failure_reported) {
            safe_print("------ Мониторинг: ускоритель " +
                       to_string(current_failed_id) +
                       " недоступен, очередь перераспределяется автоматически.");
            failure_reported = true;
        }

        if (now - last_load_report >= 3s) {
            if (current_queue_size > HIGH_LOAD_THRESHOLD) {
                safe_print("------ Мониторинг: высокая нагрузка, в очереди " +
                           to_string(current_queue_size) + " задач.");
            } else {
                safe_print("------ Мониторинг: нагрузка нормальная, в очереди " +
                           to_string(current_queue_size) + " задач.");
            }

            last_load_report = now;
        }

        if (cameras_finished == CAMERAS) {
            lock_guard<mutex> lock(queue_mtx);
            if (task_queue.empty()) {
                break;
            }
        }

        this_thread::sleep_for(1s);
    }

    safe_print("------ Мониторинг: обработка видеопотоков завершена.");
}

int main() {

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> accel_dist(1, ACCELERATORS);
    uniform_int_distribution<int> break_after_dist(3, 6);

    int selected_accelerator_for_failure = accel_dist(gen);
    int break_after_tasks = break_after_dist(gen);

    safe_print(">>> В этом запуске ускоритель " +
               to_string(selected_accelerator_for_failure) +
               " выйдет из строя после порога в " +
               to_string(break_after_tasks) +
               " обработанных задач с вероятностью 30% на каждой следующей задаче.");

    vector<thread> cameras;
    vector<thread> accelerators;

    for (int i = 1; i <= CAMERAS; ++i) {
        cameras.emplace_back(camera_stream, i);
    }

    for (int i = 1; i <= ACCELERATORS; ++i) {
        accelerators.emplace_back(
                accelerator_worker,
                i,
                selected_accelerator_for_failure,
                break_after_tasks
        );
    }

    thread monitor(global_monitor);

    for (auto& t : cameras) {
        t.join();
    }

    cv.notify_all();

    for (auto& t : accelerators) {
        t.join();
    }

    monitor.join();

    safe_print("Все видеоданные обработаны");
    return 0;
}