#ifndef DATA_RECEIVER_HPP
#define DATA_RECEIVER_HPP

#include <boost/asio.hpp>
#include <thread>
#include <vector>
#include <mutex>
#include <memory>

class DataReceiver {
public:
    DataReceiver();
    void run();
    void stop();

private:
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::acceptor acceptor;
    bool data_receiver_running;

    // void accept_connections();
    void start_accept();
    void handle_client(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    // void handle_client(boost::asio::ip::tcp::socket socket);
};

#endif // DATA_RECEIVER_HPP
