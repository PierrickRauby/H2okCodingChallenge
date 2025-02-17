#ifndef SENSOR_HPP
#define SENSOR_HPP

#include <queue>
#include <string>
#include <thread>
#include <mutex>

extern std::mutex queue_mutex;
class Sensor {
public:
    Sensor(char type, int id, std::queue<std::string>& queue);
    ~Sensor();
    void generate_data();
    void start();
    void stop();

private:
    char type; //Sensor A or Sensor B
    int id; //unique ID, retrieve via Config file
    std::queue<std::string>& message_queue; // data queue for the sensor
    bool sensor_running=false;
    std::thread sensor_thread;//data generation thread for that sensor

};

#endif // SENSOR_HPP
