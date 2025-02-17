#include <gtest/gtest.h>
#include "../include/sensor_manager.hpp"
#include "../include/sensor.hpp"
#include "../include/logger.hpp"
#include <queue>
#include <thread>
#include <iostream>

// **TestSensorManager, avoids using asio stuff **
class TestSensorManager : public SensorManager {
public:
    void send_data(bool testing = true) override {
        Logger::log("[Test] Skipping network calls for testing...");
        return;
    }
};

//** */ Test adding sensors**
TEST(SensorManagerTest, AddSensors) {
    TestSensorManager manager;
    manager.add_sensor('A', 1);
    manager.add_sensor('B', 2);
    // expecting 2 sensors
    EXPECT_EQ(manager.get_sensor_count(), 2);
}

// ** Check that sensors are sending data to the queue**
TEST(SensorManagerTest, MessageQueueReceivesData) {
    TestSensorManager manager;  // Use SensorManager to control sensors
    manager.add_sensor('A', 1);

    // Allow time for sensors to generate data
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Check if the queue has received data
    // {
        EXPECT_FALSE(manager.get_message_queue().empty());
    // }

    // Stop the sensor manager (which stops all sensors)
    manager.stop();
}

// ** Test for stopping the sensor Manager** 
TEST(SensorManagerTest, StopManager) {
    TestSensorManager manager;
    manager.add_sensor('A', 1);
    manager.add_sensor('B', 2);
    // Stop the sensor manager (which stops all sensors)
    manager.stop();
    // Expecting to have cleared all sensors
    EXPECT_EQ(manager.get_sensor_count(), 0);
}
