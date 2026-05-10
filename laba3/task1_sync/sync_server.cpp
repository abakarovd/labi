#include <boost/asio.hpp>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io;
        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 12345));

        std::cout << "Синхронный сервер запущен на порту 12345\n";

        while (true) {
            tcp::socket socket(io);
            acceptor.accept(socket);

            boost::asio::streambuf buffer;
            boost::asio::read_until(socket, buffer, '\n');

            std::istream input(&buffer);
            std::string message;
            std::getline(input, message);

            std::cout << "Получено от клиента: " << message << '\n';

            std::string response = "Сообщение получено: " + message + "\n";
            boost::asio::write(socket, boost::asio::buffer(response));
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка сервера: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
