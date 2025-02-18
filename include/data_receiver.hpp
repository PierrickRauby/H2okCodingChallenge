#ifndef DATA_RECEIVER_HPP
#define DATA_RECEIVER_HPP

#include <boost/asio.hpp>
#include <thread>
#include <vector>
#include <mutex>
#include <memory>
#include <queue>
#include <fstream>

// to store the data recieved by the receiver 
struct DataToLog {
    long long timestamp;
    std::string data;
    // Sort on time stamp
    bool operator<(const DataToLog& other) const {
        return timestamp > other.timestamp; 
    }
};

class DataReceiver {
public:
    DataReceiver();
    void run();
    void stop();

private:
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::acceptor acceptor;
    bool data_receiver_running;
    // for ordering incoming data, queue and mutex
    std::priority_queue<DataToLog> data_log_queue;
    std::mutex data_queue_mutex;

       // void accept_connections();
    void start_accept();
    void handle_client(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    // TODO: old version below, cleanup!!
    // void handle_client(boost::asio::ip::tcp::socket socket);
    //write data to log file
    void write_logs();
    long long extract_timestamp(const std::string& message);
};

#endif // DATA_RECEIVER_HPP
