#include <gtest/gtest.h>
#include "../include/data_receiver.hpp"
#include "../include/sensor_manager.hpp"
#include "../include/plc_manager.hpp"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

class SystemTest : public ::testing::Test {
protected:
    DataReceiver* receiver;
    SensorManager* sensor_manager;
    PLCManager* plc_manager;
    std::thread receiver_thread, sensor_thread, plc_thread;

    void SetUp() override {
        // Clear previous log file
        std::ofstream clear_log("data.log", std::ios::trunc);
        clear_log.close();

        //setups, but starting components during tests to test different orders
        receiver = nullptr;
        sensor_manager = nullptr;
        plc_manager = nullptr;
    }

    void TearDown() override {
        // Stop and clean-up
        // first stop the managers and receiver
        if (sensor_manager) {
            sensor_manager->stop();
        }
        if (plc_manager) {
            plc_manager->stop();
        }
        if (receiver) {
            receiver->stop();
        }
        // wait for thread termination
        if (sensor_thread.joinable()) sensor_thread.join();
        if (plc_thread.joinable()) plc_thread.join();
        if (receiver_thread.joinable()) receiver_thread.join();
        // clean-up pointers 
        if (sensor_manager) {
            delete sensor_manager;
            sensor_manager = nullptr;
        }
        if (plc_manager) {
            delete plc_manager;
            plc_manager = nullptr;
        }
        if (receiver) {
            delete receiver;
            receiver = nullptr;
        }

    }

    void verifyLogFile() {
        // simple utility to verify if the log file exist and is non empty
        std::ifstream log_file("data.log");
        ASSERT_TRUE(log_file.good()) << "[ERROR] data.log file does not exist!";

        std::string line;
        bool has_data = false;
        while (std::getline(log_file, line)) {
            if (!line.empty()) {
                has_data = true;
                std::cout << "[LOG ENTRY] " << line << std::endl;
            }
        }

        ASSERT_TRUE(has_data) << "[FAIL] Log file is empty!";
        log_file.close();
    }
};

// **Test Case 1: Start DataReceiver first, then PLCManager and SensorManager**
TEST_F(SystemTest, StartReceiverFirst) {
    receiver = new DataReceiver();
    receiver_thread = std::thread([this]() { receiver->run(); });

    std::this_thread::sleep_for(std::chrono::seconds(1));  // Allow receiver to start

    sensor_manager = new SensorManager();
    plc_manager = new PLCManager();

    sensor_manager->add_sensor('A', 1);
    sensor_manager->add_sensor('B', 2);
    plc_manager->add_plc(1);
    plc_manager->add_plc(2);

    sensor_thread = std::thread([this]() { sensor_manager->send_data(); });
    plc_thread = std::thread([this]() { plc_manager->send_data(); });

    std::this_thread::sleep_for(std::chrono::seconds(5));

    verifyLogFile();
}



