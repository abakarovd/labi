#include "client.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <string>

int main() {
    try {
        boost::asio::io_context io;
        Client client(io, "127.0.0.1", "12345");
        client.run();
    } catch (const std::exception& e) {
        std::cerr << "Ошибка клиента: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
