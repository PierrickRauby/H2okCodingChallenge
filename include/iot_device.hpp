#ifndef IOT_DEVICE_HPP
#define IOT_DEVICE_HPP

#include <queue>
#include <string>
#include <thread>
#include <mutex>

class IoTDevice {
public:
    IoTDevice(int id, std::queue<std::string>& queue);
    virtual ~IoTDevice();

    void start();
    void stop();
    
protected:
    virtual void generate_data() = 0; // data are different for Sensor and PLC
    void push_message(const std::string& message);

    int id; //unique ID, retrieve via Config file
    std::queue<std::string>& message_queue; // data queue for the sensor
    std::thread device_thread; ;//data generation thread 
    std::mutex queue_mutex;  // No need for global mutex
    bool running;
};

#endif // IOT_DEVICE_HPP
