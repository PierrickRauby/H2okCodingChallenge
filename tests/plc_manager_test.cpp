#include <gtest/gtest.h>
#include "../include/plc_manager.hpp"
#include "../include/plc.hpp"
#include "../include/logger.hpp"
#include <queue>
#include <thread>
#include <iostream>

bool running = true;

// **TestPLCManager to prevent real network calls**
class TestPLCManager : public PLCManager {
public:
    void send_data(bool testing = true) override {
        if (testing) {
            Logger::log("[Test] Skipping network calls for testing...");
            return;
        }
        PLCManager::send_data();
    }
};

// **Test adding plcs**
TEST(PLCManagerTest, AddPLCs) {
    TestPLCManager manager;
    manager.add_plc(1);
    manager.add_plc(2);

    EXPECT_GE(manager.get_plc_count(), 2);
}


// ** Check that plc are sending data to the queue**
TEST(PLCManagerTest, MessageQueueReceivesData) {
    TestPLCManager manager;  // Use PLCManager to control plcs
    manager.add_plc(1);
    manager.add_plc(2);

    // Run `send_data()` in a separate thread
    std::thread plc_thread([&]() {
        manager.send_data(true); 
    });

    // Allow time for plcs to generate data
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Stop the plc manager (which stops all plcs)
    manager.stop();

    // Ensure all threads stop
    if (plc_thread.joinable()) {
        plc_thread.join();
    }
    // Check if the queue has data
    {
        std::lock_guard<std::mutex> lock(plc_queue_mutex);
        EXPECT_FALSE(manager.get_message_queue().empty());  
    }
}

// ** Test for stopping the PLC Manager** 
TEST(PLCManagerTest, StopManager) {
    TestPLCManager manager;
    manager.add_plc(1);
    manager.add_plc(2);
    // Stop the plc manager (which stops all plcs)
    manager.stop();
    // Expecting to have cleared all plcs
    EXPECT_EQ(manager.get_plc_count(), 0);
}
