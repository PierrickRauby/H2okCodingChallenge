#include "plc.hpp"
#include "logger.hpp"
#include <sstream>
#include <chrono>
#include <thread>
#include <random>
#include <iostream>

PLC::PLC(int id, std::queue<std::string>& queue)
    : IoTDevice(id, queue) {}

PLC::~PLC(){
    stop();//stop all plc thread at destruction
}

void PLC::generate_data() {
    int i = 0;
    std::cout<<"Generating data, running="<<running<<std::endl;
    while (running) {  
        i++;
        std::ostringstream oss;
        // Get a timestamp
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        if (rand()%100<3) { // plc have a 3% chance of failure
            // Message if PLC fails
            oss << "{ \"plc_id\": " << id << ", \"status\": \"offline\", \"timestamp\": " << now << " }\n";
            Logger::log("[ERROR] PLC [" + std::to_string(id) + "] is OFFLINE");
        } else {
            // Message with some fake data depending on id only
            oss << "{ \"plc_id\": \"" << id
                << ", \"timestamp\": " << now << ", \"i\": " << i
                << ", \"values\": [" << (10.0 + id) 
                << ", " << (20.0 * id) 
                << ", " << (30.0 * id) << "] }\n";
        }

        push_message(oss.str()); 
        Logger::log("[INFO] PLC [" + std::to_string(id) + "] generated data.");

        // TODO: probably should not hard code that 1 sec
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
