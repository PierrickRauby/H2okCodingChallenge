#ifndef SENSOR_MANAGER_HPP
#define SENSOR_MANAGER_HPP

#include "sensor.hpp"
#include <boost/asio.hpp>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>

class SensorManager {
public:
    SensorManager();
    ~SensorManager();
    void start();
    void stop();  
    void add_sensor(char type, int id);
    // send data to the data receiver object
    virtual void send_data(bool testing=false);
    // expected to be only used for testing
    // 1 thread per sensor so can return the size of sensors
    int get_sensor_count() const {return sensors.size();};
    std::queue<std::string>& get_message_queue(){return message_queue;};

private:
    // to queue the data to be sent, easy to handle interruption
    std::queue<std::string> message_queue;
    std::mutex queue_mutex;
    boost::asio::io_context io_context;
    // to place the different sensors as separated threads 
    std::vector<std::unique_ptr<Sensor>> sensors; // sensors managed by the manager
    bool running =false;

    void flush_buffer(boost::asio::ip::tcp::socket& socket);
    void reconnect(boost::asio::ip::tcp::socket& socket, boost::asio::ip::tcp::resolver& resolver);
};

#endif // SENSOR_MANAGER_HPP
