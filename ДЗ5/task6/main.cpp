#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <semaphore>
#include <thread>
#include <unordered_map>
#include <vector>

struct FileChunk {
    int chunk_id;
    int file_id;
    std::size_t size;

    void download() const {
        std::this_thread::sleep_for(std::chrono::milliseconds(10 + static_cast<int>(size % 20)));
    }
};

struct FileDownload {
    int file_id;
    std::vector<FileChunk> chunks;
    std::atomic<int> downloaded_chunks{0};

    bool is_complete() const {
        return downloaded_chunks.load() == static_cast<int>(chunks.size());
    }

    void mark_chunk_downloaded() {
        ++downloaded_chunks;
    }
};

class DownloadManager {
public:
    DownloadManager(int max_active_files, int max_chunks)
        : active_downloads_(max_active_files), chunk_downloads_(max_chunks) {}

    void add_file(const FileDownload& file) {
        std::lock_guard<std::mutex> lk(queue_mutex_);
        file_sizes_[file.file_id] = static_cast<int>(file.chunks.size());
        for (const auto& c : file.chunks) queue_.push(c);
    }

    void download_worker() {
        while (true) {
            FileChunk chunk;
            {
                std::lock_guard<std::mutex> lk(queue_mutex_);
                if (queue_.empty()) return;
                chunk = queue_.front();
                queue_.pop();
            }

            active_downloads_.acquire();
            chunk_downloads_.acquire();
            process_chunk(chunk);
            chunk_downloads_.release();
            active_downloads_.release();
            std::this_thread::yield();
        }
    }

    inline void process_chunk(const FileChunk& chunk) {
        std::cout << "thread " << std::this_thread::get_id() << " file=" << chunk.file_id << " chunk=" << chunk.chunk_id << " start\n";
        chunk.download();
        std::cout << "thread " << std::this_thread::get_id() << " file=" << chunk.file_id << " chunk=" << chunk.chunk_id << " done\n";

        int done_count;
        int total;
        {
            std::lock_guard<std::mutex> lk(stats_mutex_);
            done_count = ++downloaded_by_file_[chunk.file_id];
            total = file_sizes_[chunk.file_id];
        }
        if (done_count == total) ++completed_files_;
    }

    int completed_files() const { return completed_files_.load(); }

private:
    std::queue<FileChunk> queue_;
    std::counting_semaphore<1024> active_downloads_;
    std::counting_semaphore<1024> chunk_downloads_;
    std::mutex queue_mutex_;
    std::mutex stats_mutex_;
    std::unordered_map<int, int> downloaded_by_file_;
    std::unordered_map<int, int> file_sizes_;
    std::atomic<int> completed_files_{0};
};

int main() {
    DownloadManager dm(2, 3);

    FileDownload f1{1, {{1, 1, 100}, {2, 1, 200}, {3, 1, 150}}};
    FileDownload f2{2, {{1, 2, 80}, {2, 2, 90}}};

    dm.add_file(f1);
    dm.add_file(f2);

    std::mutex wm;
    std::condition_variable wcv;
    int done = 0;

    auto worker = [&] {
        dm.download_worker();
        { std::lock_guard<std::mutex> g(wm); ++done; }
        wcv.notify_one();
    };

    std::thread(worker).detach();
    std::thread(worker).detach();
    std::thread(worker).detach();

    std::unique_lock<std::mutex> lk(wm);
    wcv.wait(lk, [&] { return done == 3; });
    std::cout << "completed_files=" << dm.completed_files() << "\n";
}
