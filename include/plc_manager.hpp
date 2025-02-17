#ifndef PLC_MANAGER_HPP
#define PLC_MANAGER_HPP

#include "plc.hpp"
#include <boost/asio.hpp>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>

class PLCManager {
public:
    PLCManager();
    ~PLCManager();
    void start();
    void stop();
    void add_plc(int id);
    // send data to the data receiver object
    virtual void send_data(bool testing=false);
    // expected to be only used for testing
    // 1 thread per plc so can return the size of plcs
    int get_plc_count() const {return plcs.size();};
    std::queue<std::string>& get_message_queue(){return message_queue;};

private:
    // to queue the data to be sent, easy to handle interruption
    std::queue<std::string> message_queue; 
    std::mutex queue_mutex;
    boost::asio::io_context io_context;
    // to place the different plcs as separated threads
    std::vector<std::unique_ptr<PLC>> plcs; // PLCs managed by the manager
    bool running =false;


    void flush_buffer(boost::asio::ip::tcp::socket& socket);
    void reconnect(boost::asio::ip::tcp::socket& socket, boost::asio::ip::tcp::resolver& resolver);
};

#endif // PLC_MANAGER_HPP
