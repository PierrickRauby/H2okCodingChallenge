#include "iot_device.hpp"
#include <iostream>

IoTDevice::IoTDevice(int id, std::queue<std::string>& queue)
    : id(id), message_queue(queue), running(false) {}

IoTDevice::~IoTDevice() {
    stop();//stop all plc thread at destruction
}

void IoTDevice::start() {
    if (running) return;
    running = true;
    device_thread = std::thread(&IoTDevice::generate_data, this);
}

void IoTDevice::stop() {
    if (!running) return;
    running = false;
    // Wait for data gen threads to terminate
    if (device_thread.joinable()) {
        device_thread.join();
    }
    std::cout << "Device " << id << " stopped." << std::endl;
}

void IoTDevice::push_message(const std::string& message) {
    //utility to prepare data to be pushed
    // take the queue and put the new data in it!
    std::lock_guard<std::mutex> lock(queue_mutex);
    message_queue.push(message);
}
