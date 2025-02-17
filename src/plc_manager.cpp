#include "plc_manager.hpp"
#include "logger.hpp"
#include <iostream>

PLCManager::PLCManager() {}

PLCManager::~PLCManager() {
    stop();
}

void PLCManager::add_plc(int id) {
    if(!running){
        // Starting the plc manager before adding plcs
        start();
    }
    // Add the plc with correct id, then start it
    devices.push_back(std::make_unique<PLC>(id, message_queue));
    devices.back()->start();
    Logger::log("[INFO] PLC " + std::to_string(id) + " added and running.");
}

std::unique_ptr<IoTDevice> PLCManager::create_device(int id) {
    return std::make_unique<PLC>(id, message_queue);
}
