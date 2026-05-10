#pragma once

#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include <memory>
#include <string>
#include <vector>

using boost::asio::ip::tcp;

class Server;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket,
            boost::asio::io_context& io,
            boost::asio::thread_pool& workers,
            boost::asio::strand<boost::asio::io_context::executor_type>& log_strand,
            std::vector<std::string>& log_storage);

    void start();

private:
    void do_read();
    void handle_request(const std::string& request);
    void handle_number_command(int n);
    void handle_timer_command(int seconds);
    void do_write(const std::string& response, bool continue_reading = true);
    void log_message(const std::string& message);
    void start_timeout();
    void refresh_timeout();
    void close_socket();

    static std::string trim(std::string value);
    static unsigned long long factorial(int n);
    static bool parse_number_command(const std::string& request, int& n);
    static bool parse_timer_command(const std::string& request, int& seconds);

    tcp::socket socket_;
    boost::asio::io_context& io_;
    boost::asio::thread_pool& workers_;
    boost::asio::strand<boost::asio::io_context::executor_type>& log_strand_;
    std::vector<std::string>& log_;

    boost::asio::streambuf buffer_;
    std::shared_ptr<boost::asio::steady_timer> timeout_timer_;
    std::shared_ptr<boost::asio::steady_timer> response_timer_;
};

class Server {
public:
    Server(boost::asio::io_context& io, unsigned short port, boost::asio::thread_pool& workers);
    const std::vector<std::string>& logs() const;

private:
    void do_accept();
    void log_message(const std::string& message);

    boost::asio::io_context& io_;
    tcp::acceptor acceptor_;
    boost::asio::thread_pool& workers_;
    boost::asio::strand<boost::asio::io_context::executor_type> log_strand_;
    std::vector<std::string> log_;
};
