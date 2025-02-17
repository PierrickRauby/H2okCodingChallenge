#include "sensor_manager.hpp"
#include "logger.hpp"
#include <iostream>

SensorManager::SensorManager() {}

SensorManager::~SensorManager() {
    stop();
}

void SensorManager::add_sensor(char type, int id) {
    if(!running){
        // Starting the sensor manager before adding sensors
        start();
    }
    // Add the sensor with correct type and id, then start it
    devices.push_back(std::make_unique<Sensor>(type, id, message_queue));
    devices.back()->start();
    Logger::log("[INFO] Sensor " + std::to_string(id) + " added and running.");
}

std::unique_ptr<IoTDevice> SensorManager::create_device(int id) {
    return std::make_unique<Sensor>('A', id, message_queue);
}
