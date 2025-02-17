#ifndef SENSOR_HPP
#define SENSOR_HPP

#include "iot_device.hpp"

class Sensor : public IoTDevice {
public:
    Sensor(char type, int id, std::queue<std::string>& queue);
    ~Sensor();
protected:
    void generate_data() override;

private:
    char type; //Sensor A or Sensor B 
};

#endif // SENSOR_HPP
