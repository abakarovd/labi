#pragma once

#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;

class Client {
public:
    Client(boost::asio::io_context& io, const std::string& host, const std::string& port);
    void run();

private:
    boost::asio::io_context& io_;
    tcp::socket socket_;
};
