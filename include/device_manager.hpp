#ifndef DEVICE_MANAGER_HPP
#define DEVICE_MANAGER_HPP

#include "iot_device.hpp"
#include <boost/asio.hpp>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <memory>

class DeviceManager {
public:
    virtual ~DeviceManager();

    void start();
    void stop();
    // send data to the data receiver object
    virtual void send_data(bool testing = false);
    // expected to be only used for testing
    // 1 thread per device so can return the size of devices
    int get_device_count() const { return devices.size(); }
    std::queue<std::string>& get_message_queue() { return message_queue; }

protected:
    explicit DeviceManager();
    // to queue the data to be sent, easy to handle interruption
    virtual std::unique_ptr<IoTDevice> create_device(int id) = 0; 

    std::queue<std::string> message_queue;
    std::mutex queue_mutex;
    boost::asio::io_context io_context;
    // to place the different devices who each have separated threads
    std::vector<std::unique_ptr<IoTDevice>> devices;
    bool running = false;

private:
    void flush_buffer(boost::asio::ip::tcp::socket& socket);
    void reconnect(boost::asio::ip::tcp::socket& socket, boost::asio::ip::tcp::resolver& resolver);
};

#endif // DEVICE_MANAGER_HPP
