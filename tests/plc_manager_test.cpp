#include <gtest/gtest.h>
#include "../include/plc_manager.hpp" 
#include "../include/plc.hpp"
#include "../include/logger.hpp"
#include <queue>
#include <thread>
#include <iostream>


// **TestPLCManager to prevent real network calls**
class TestPLCManager : public PLCManager {
public:
    void send_data(bool testing = true) override {
        Logger::log("[Test] Skipping network calls for testing...");
        return;
    }
};

// **Test adding plcs**
TEST(PLCManagerTest, AddPLCs) {
    TestPLCManager manager;
    manager.add_plc(1);
    manager.add_plc(2);

    EXPECT_EQ(manager.get_device_count(), 2);  
}


// ** Check that plc are sending data to the queue**
TEST(PLCManagerTest, MessageQueueReceivesData) {
    TestPLCManager manager;  // Use PLCManager to control plcs
    manager.add_plc(1);
    manager.add_plc(2);

    // Allow time for plcs to generate data
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Check if the queue has data 
    EXPECT_FALSE(manager.get_message_queue().empty());
    // Stop the plc manager (which stops all plcs)
    manager.stop();
}

// ** Test for stopping the PLC Manager** 
TEST(PLCManagerTest, StopManager) {
    TestPLCManager manager;
    manager.add_plc(1);
    manager.add_plc(2);
    // Stop the plc manager (which stops all plcs) 
    manager.stop();
    // Expecting to have cleared all plcs
    EXPECT_EQ(manager.get_device_count(), 0);  
}
