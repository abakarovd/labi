#include "client.hpp"

#include <iostream>

Client::Client(boost::asio::io_context& io, const std::string& host, const std::string& port)
    : io_(io), socket_(io) {
    tcp::resolver resolver(io_);
    auto endpoints = resolver.resolve(host, port);
    boost::asio::connect(socket_, endpoints);
}

void Client::run() {
    std::cout << "Команды:\n";
    std::cout << "  число 5    -> факториал 5\n";
    std::cout << "  таймер 3   -> ответ через 3 секунды\n";
    std::cout << "  exit       -> выход\n\n";

    while (true) {
        std::cout << "> ";
        std::string request;
        std::getline(std::cin, request);

        if (request == "exit") {
            break;
        }

        request += '\n';
        boost::asio::write(socket_, boost::asio::buffer(request));

        boost::asio::streambuf buffer;
        boost::asio::read_until(socket_, buffer, '\n');

        std::istream input(&buffer);
        std::string response;
        std::getline(input, response);

        std::cout << "Ответ сервера: " << response << '\n';
    }
}
