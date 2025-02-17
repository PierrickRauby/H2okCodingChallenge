#include "plc.hpp"
#include "logger.hpp"
#include <sstream>
#include <chrono>
#include <thread>
#include <random>
#include <iostream>
//TODO: I should be able to remove that!
extern bool plc_running;

std::mutex plc_queue_mutex;

PLC::PLC(int id, std::queue<std::string>& queue)
    : id(id), message_queue(queue) {}



PLC::~PLC(){
    stop();//stop all plc thread at destruction
}

void PLC::start() {
    if (plc_running) return;
    plc_running = true;
    plc_thread = std::thread(&PLC::generate_data, this); 
}

void PLC::stop() {
    std::cout<<"plc "<<id<<" stopped"<<std::endl;
    if (!plc_running) return;
    plc_running = false;
    // Wait for data gen threads to terminate
    if (plc_thread.joinable()) {
        plc_thread.join(); 
    }
}

void PLC::generate_data() {
    int i = 0;
    std::cout<<"Generating data, running="<<plc_running<<std::endl;
    while (plc_running) {
        i++;
        std::ostringstream oss;
        // Get a timestamp
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        if (rand()%100<3) { // plc have a 3% chance of failure
            // Message if PLC fails
            oss << "{ \"plc_id\": " << id << ", \"status\": \"offline\", \"timestamp\": " << now << " }\n";
            Logger::log("[ERROR] PLC " + std::string(1, type) + "[" + std::to_string(id) + "] is OFFLINE");
        } else {
            // Message with some fake data depending on id only
            oss << "{ \"plc_id\": \"" << id
                << ", \"timestamp\": " << now << ", \"i\": " << i
                << ", \"values\": [" << (10.0 + id) 
                << ", " << (20.0 * id) 
                << ", " << (30.0 * id) << "] }\n";
        }

        {
            std::lock_guard<std::mutex> lock(plc_queue_mutex);
            message_queue.push(oss.str());
        }
        Logger::log("[INFO] PLC " + std::string(1, type) + "[" + std::to_string(id) + "] generated data.");

        // TODO: probably should not hard code that 1 sec
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
