#include "device_manager.hpp"
#include "logger.hpp"
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
constexpr char HOST[] = "127.0.0.1";
constexpr int PORT = 12345;
constexpr int RECONNECT_INTERVAL = 3;

DeviceManager::DeviceManager() {}

DeviceManager::~DeviceManager() {
    stop();
}

void DeviceManager::start() {
    if (running) {
        Logger::log("[INFO] DeviceManager already running.");
        return;
    }
    running = true;
    Logger::log("[INFO] DeviceManager started.");
}

void DeviceManager::stop() {
    // Stop the device manager and all attached devices 
    if (!running) {
        Logger::log("[INFO] DeviceManager already stopped.");
        return;
    }

    Logger::log("[INFO] Stopping DeviceManager...");
    running = false;
    // stop all devices before removing them
    for (auto& device : devices) {
        device->stop();
    }

    devices.clear(); // Now safe to clear the vector
    Logger::log("[INFO] DeviceManager stopped.");
}

void DeviceManager::send_data(bool testing) {
    // TODO: should be good to remove that and testing argument bc of 
    // TestPLCManager class
    // for testing we are not sending data (there is not data_receiver)
    if (testing) {
        Logger::log("[MOCK] Skipping networking for testing...");
        return;
    }

    tcp::socket socket(io_context);
    tcp::resolver resolver(io_context);
    bool connected = false;

    while (running) {
        if (!connected) {
             //** if device is running and not (never)connected**
            try {
                //try to connect
                boost::asio::connect(socket, resolver.resolve(HOST, std::to_string(PORT)));
                Logger::log("[INFO] Connected! Sending data...");
                connected = true;
                // and then flush the buffered data
                flush_buffer(socket);
            } catch (...) {
                //try again later
                Logger::log("[ERROR] Connection failed. Retrying...");
                std::this_thread::sleep_for(std::chrono::seconds(RECONNECT_INTERVAL));
                continue;
            }
        }

        if (connected) {
            //**If sensor running and connected */
            try {
                std::lock_guard<std::mutex> lock(queue_mutex);
                if (!message_queue.empty()) {
                    std::string msg = message_queue.front();
                    boost::asio::write(socket, boost::asio::buffer(msg));
                    Logger::log("[SENT] " + msg);
                    message_queue.pop();
                }
            } catch (...) {
                //Server dropped, trying to reconnect
                connected = false;
                reconnect(socket, resolver);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    Logger::log("[INFO] Stopping device manager...");
}

void DeviceManager::flush_buffer(tcp::socket& socket) {
    //Used to send all the message in the device's queue to empty it
    std::lock_guard<std::mutex> lock(queue_mutex);
    Logger::log("[INFO] Flushing queue...");

    while (!message_queue.empty()) {
        std::string msg = message_queue.front();
        boost::asio::write(socket, boost::asio::buffer(msg));
        Logger::log("[SENT] " + msg);
        message_queue.pop();
    }
}

void DeviceManager::reconnect(tcp::socket& socket, tcp::resolver& resolver) {
    //Function used specifically to reconnect to the server and if successful
    // flush the buffer of data stored in the queue
    socket.close();
    while (running) {
        try {
            boost::asio::connect(socket, resolver.resolve(HOST, std::to_string(PORT)));
            Logger::log("[INFO] Reconnected to server.");
            flush_buffer(socket);
            return;
        } catch (...) {
            std::this_thread::sleep_for(std::chrono::seconds(RECONNECT_INTERVAL));
        }
    }
}
