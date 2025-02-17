#ifndef SENSOR_MANAGER_HPP
#define SENSOR_MANAGER_HPP

#include "device_manager.hpp"
#include "sensor.hpp"

class SensorManager : public DeviceManager {
public:
    SensorManager();
    ~SensorManager();

    void add_sensor(char type, int id);

protected:
    std::unique_ptr<IoTDevice> create_device(int id) override;
};

#endif // SENSOR_MANAGER_HPP
