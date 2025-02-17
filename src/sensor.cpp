#include "sensor.hpp"
#include "logger.hpp"
#include <sstream>
#include <chrono>
#include <thread>
#include <random>
#include <iostream>
//TODO: I should be able to remove that!
extern bool sensor_running;

std::mutex sensor_queue_mutex;

Sensor::Sensor(char type, int id, std::queue<std::string>& queue)
    : type(type), id(id), message_queue(queue) {}



Sensor::~Sensor(){
    stop();//stop all sensor thread at destruction
}

void Sensor::start() {
    if (sensor_running) return;
    sensor_running = true;
    sensor_thread = std::thread(&Sensor::generate_data, this); 
}

void Sensor::stop() {
    std::cout<<"sensor "<<type<<":"<<id<<" stopped"<<std::endl;
    if (!sensor_running) return;
    sensor_running = false;
    // Wait for data gen thread to terminate
    if (sensor_thread.joinable()) {
        sensor_thread.join(); 
    }
}

void Sensor::generate_data() {
    int i = 0;
    std::cout<<"Generating data, running="<<sensor_running<<std::endl;
    while (sensor_running) {
        i++;
        std::ostringstream oss;
        // Get a timestamp
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        if (rand()%100<5) { // sensor have a 5% chance of failure
            // Message if sensor fails!
            oss << "{ \"sensor\": \"" << type << "\", \"sensor_id\": " << id
                << ", \"status\": \"offline\", \"timestamp\": " << now << " }\n";
            Logger::log("[ERROR] Sensor " + std::string(1, type) + "[" + std::to_string(id) + "] is OFFLINE");
        } else {
            // Message with some fake data, changes depending on sensor type and id
            oss << "{ \"sensor\": \"" << type << "\", \"sensor_id\": " << id
                << ", \"timestamp\": " << now << ", \"i\": " << i
                << ", \"values\": [" << (10.0 * (type == 'A' ? 1 : 4) + id) 
                << ", " << (20.0 * (type == 'A' ? 1 : 4) + id) 
                << ", " << (30.0 * (type == 'A' ? 1 : 4) + id) << "] }\n";
        }

        {
            // take the queue and put the new data in it!
            std::lock_guard<std::mutex> lock(sensor_queue_mutex);
            message_queue.push(oss.str());
        }
        Logger::log("[INFO] Sensor " + std::string(1, type) + "[" + std::to_string(id) + "] generated data.");
        //sensor data read frequency
        // TODO: probably should not hard code that 1 sec
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
