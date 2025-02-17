#include "sensor_manager.hpp"
#include "config_loader.hpp"  
#include "logger.hpp"
#include <iostream>
#include <csignal>
#include <chrono>
#include <thread>

// TODO: sensor_running might not be useful anymore
SensorManager manager;
bool sensor_running = true;


int main() {

    Logger::log("[INFO] Starting sensor simulation...");

    auto config = load_config("sensor");

    for (int i = 0; i < config["SENSOR_A"]; i++) {
        manager.add_sensor('A', i);
    }
    for (int i = 0; i < config["SENSOR_B"]; i++) {
        manager.add_sensor('B', i);
    }

    // Start sending data to receiver in a separate thread
    std::thread send_thread([&] { manager.send_data(); });

    // Keep sensor running indefinitely
    while (sensor_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    Logger::log("[INFO] Stopping Sensor Manager...");
    manager.stop();
    if (send_thread.joinable()) send_thread.join();

    Logger::log("[INFO] Sensor simulation stopped.");
    return 0;
}
