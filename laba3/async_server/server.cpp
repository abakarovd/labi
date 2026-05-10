#include "server.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace {
constexpr int SESSION_TIMEOUT_SECONDS = 60;
}

Session::Session(tcp::socket socket,
                 boost::asio::io_context& io,
                 boost::asio::thread_pool& workers,
                 boost::asio::strand<boost::asio::io_context::executor_type>& log_strand,
                 std::vector<std::string>& log_storage)
    : socket_(std::move(socket)),
      io_(io),
      workers_(workers),
      log_strand_(log_strand),
      log_(log_storage),
      timeout_timer_(std::make_shared<boost::asio::steady_timer>(io_)) {}

void Session::start() {
    try {
        log_message("Клиент подключился: " + socket_.remote_endpoint().address().to_string() +
                    ":" + std::to_string(socket_.remote_endpoint().port()));
    } catch (...) {
        log_message("Клиент подключился");
    }

    start_timeout();
    do_read();
}

void Session::start_timeout() {
    refresh_timeout();
}

void Session::refresh_timeout() {
    auto self = shared_from_this();
    timeout_timer_->expires_after(std::chrono::seconds(SESSION_TIMEOUT_SECONDS));
    timeout_timer_->async_wait([this, self](const boost::system::error_code& ec) {
        if (!ec) {
            log_message("Соединение закрыто по таймауту");
            close_socket();
        }
    });
}

void Session::do_read() {
    auto self = shared_from_this();

    boost::asio::async_read_until(
        socket_, buffer_, '\n',
        [this, self](boost::system::error_code ec, std::size_t) {
            if (ec) {
                if (ec != boost::asio::error::operation_aborted &&
                    ec != boost::asio::error::eof &&
                    ec != boost::asio::error::connection_reset) {
                    log_message("Ошибка чтения: " + ec.message());
                }
                return;
            }

            refresh_timeout();

            std::istream input(&buffer_);
            std::string request;
            std::getline(input, request);
            request = trim(request);

            handle_request(request);
        });
}

void Session::handle_request(const std::string& request) {
    if (request.empty()) {
        do_write("Ошибка: пустой запрос\n");
        return;
    }

    int value = 0;
    if (parse_number_command(request, value)) {
        handle_number_command(value);
        return;
    }

    if (parse_timer_command(request, value)) {
        handle_timer_command(value);
        return;
    }

    do_write("Ошибка: используйте команды 'число N' или 'таймер N'\n");
}

void Session::handle_number_command(int n) {
    if (n < 0 || n > 20) {
        do_write("Ошибка: факториал поддерживается для N от 0 до 20\n");
        return;
    }

    auto self = shared_from_this();

    // Долгую работу отправляем в пул вычислительных потоков.
    boost::asio::post(workers_, [this, self, n]() {
        try {
            auto result = factorial(n);
            std::string response = "Факториал " + std::to_string(n) + " = " +
                                   std::to_string(result) + "\n";

            log_message("Вычислено: " + response);

            // Возвращаемся в io_context, потому что писать в сокет безопаснее из сетевого контекста.
            boost::asio::post(io_, [this, self, response]() {
                do_write(response);
            });
        } catch (const std::exception& e) {
            boost::asio::post(io_, [this, self, err = std::string(e.what())]() {
                do_write("Ошибка вычисления: " + err + "\n");
            });
        }
    });
}

void Session::handle_timer_command(int seconds) {
    if (seconds < 0 || seconds > 3600) {
        do_write("Ошибка: таймер поддерживается от 0 до 3600 секунд\n");
        return;
    }

    auto self = shared_from_this();
    response_timer_ = std::make_shared<boost::asio::steady_timer>(io_, std::chrono::seconds(seconds));

    log_message("Запущен таймер на " + std::to_string(seconds) + " секунд");

    response_timer_->async_wait([this, self, seconds](boost::system::error_code ec) {
        if (ec) {
            log_message("Таймер отменён: " + ec.message());
            return;
        }

        do_write("Прошло " + std::to_string(seconds) + " секунд!\n");
    });
}

void Session::do_write(const std::string& response, bool continue_reading) {
    auto self = shared_from_this();
    auto message = std::make_shared<std::string>(response);

    boost::asio::async_write(
        socket_, boost::asio::buffer(*message),
        [this, self, message, continue_reading](boost::system::error_code ec, std::size_t) {
            if (ec) {
                log_message("Ошибка записи: " + ec.message());
                return;
            }

            if (continue_reading) {
                do_read();
            }
        });
}

void Session::log_message(const std::string& message) {
    boost::asio::post(log_strand_, [this, message]() {
        log_.push_back(message);
        std::cout << "[LOG] " << message << '\n';
    });
}

void Session::close_socket() {
    boost::system::error_code ignored_ec;
    socket_.shutdown(tcp::socket::shutdown_both, ignored_ec);
    socket_.close(ignored_ec);
}

std::string Session::trim(std::string value) {
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    value.erase(std::find_if(value.rbegin(), value.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), value.end());
    return value;
}

unsigned long long Session::factorial(int n) {
    unsigned long long result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= static_cast<unsigned long long>(i);
    }
    return result;
}

bool Session::parse_number_command(const std::string& request, int& n) {
    std::istringstream iss(request);
    std::string command;
    iss >> command;

    if (command == "число" || command == "number") {
        return static_cast<bool>(iss >> n);
    }

    // Для удобства принимаем просто "5" как команду вычисления факториала.
    std::istringstream only_number(request);
    int parsed = 0;
    if ((only_number >> parsed) && only_number.eof()) {
        n = parsed;
        return true;
    }

    return false;
}

bool Session::parse_timer_command(const std::string& request, int& seconds) {
    std::istringstream iss(request);
    std::string command;
    iss >> command;

    if (command != "таймер" && command != "timer") {
        return false;
    }

    return static_cast<bool>(iss >> seconds);
}

Server::Server(boost::asio::io_context& io, unsigned short port, boost::asio::thread_pool& workers)
    : io_(io),
      acceptor_(io, tcp::endpoint(tcp::v4(), port)),
      workers_(workers),
      log_strand_(io.get_executor()) {
    log_message("Сервер запущен на порту " + std::to_string(port));
    do_accept();
}

const std::vector<std::string>& Server::logs() const {
    return log_;
}

void Server::do_accept() {
    acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            std::make_shared<Session>(std::move(socket), io_, workers_, log_strand_, log_)->start();
        } else {
            log_message("Ошибка accept: " + ec.message());
        }

        do_accept();
    });
}

void Server::log_message(const std::string& message) {
    boost::asio::post(log_strand_, [this, message]() {
        log_.push_back(message);
        std::cout << "[LOG] " << message << '\n';
    });
}
