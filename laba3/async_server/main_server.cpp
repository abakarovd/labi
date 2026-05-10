#include "server.hpp"

#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <vector>

int main(int argc, char* argv[]) {
    try {
        unsigned short port = 12345;
        int thread_count = 4;

        if (argc >= 2) {
            thread_count = std::max(1, std::atoi(argv[1]));
        }

        boost::asio::io_context io;
        boost::asio::thread_pool workers(thread_count);

        Server server(io, port, workers);

        boost::asio::signal_set signals(io, SIGINT, SIGTERM);
        signals.async_wait([&](const boost::system::error_code&, int) {
            std::cout << "\nОстановка сервера...\n";
            io.stop();
            workers.stop();
        });

        std::vector<std::thread> io_threads;
        for (int i = 0; i < thread_count; ++i) {
            io_threads.emplace_back([&io]() { io.run(); });
        }

        for (auto& thread : io_threads) {
            thread.join();
        }

        workers.join();
        std::cout << "Сервер завершил работу корректно.\n";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка сервера: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
