#include "plc_manager.hpp"
#include "config_loader.hpp"  
#include "logger.hpp"
#include <iostream>
#include <csignal>
#include <chrono>
#include <thread>

// TODO: plc_running might not be useful anymore
PLCManager manager;
bool plc_running = true;


int main() {

    Logger::log("[INFO] Starting PLC simulation...");

    auto config = load_config("plc");

    for (int i = 0; i < config["PLC"]; i++) {
        manager.add_plc(i);
    }
    std::cout<<"plc running: "<<plc_running<<std::endl;
    // Start sending data to receiver in a separate thread
    std::thread send_thread([&] { manager.send_data(); });

    // Keep plc running indefinitely
    while (plc_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    Logger::log("[INFO] Stopping PLC Manager...");
    manager.stop();
    if (send_thread.joinable()) send_thread.join();

    Logger::log("[INFO] PLC simulation stopped.");
    return 0;
}
