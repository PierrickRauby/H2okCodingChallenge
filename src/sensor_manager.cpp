#include "sensor_manager.hpp"
#include "logger.hpp"
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
constexpr char HOST[] = "127.0.0.1";
constexpr int PORT = 12345;
constexpr int RECONNECT_INTERVAL = 3;


SensorManager::SensorManager() {}

SensorManager::~SensorManager() {
    std::cout << "[DEBUG] Destructor: Stopping SensorManager" << std::endl;
    stop();
}

void SensorManager::add_sensor(char type, int id) {
    if !running){
        // Starting the sensor manager before adding sensors
        start(); 
    }
    // Add the sensor with correct type and id, then start it
    sensors.push_back(std::make_unique<Sensor>(type, id, message_queue));
    sensors.back()->start();
    Logger::log("[INFO] Sensor " + std::to_string(id) + " added and running.");
}


void SensorManager::send_data(bool testing) {
    // TODO: should be good to remove that and testing argument bc of 
    // TestSensorManager class
    // for testing we are not sending data (there is not data_receiver)
    if(testing){
        Logger::log("[MOCK] Skipping networking for test purposes...");
        return;
    }
    tcp::socket socket(io_context);
    tcp::resolver resolver(io_context);
    bool connected = false;
    while (running) { 
        if (!connected) {
            //** if sensor is running and not (never)connected**
            try {
                // try to connect
                boost::asio::connect(socket, resolver.resolve(HOST, std::to_string(PORT)));
                Logger::log("[INFO] Connected! Sending data...");
                connected = true;
                // and then flush the buffered the data
                flush_buffer(socket);
            } catch (...) {
                // try again later
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
                    // if data in the queue send it!
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

    Logger::log("[INFO] Stopping sensor manager...");
}

void SensorManager::flush_buffer(tcp::socket& socket) {
    //Used to send all the message in the sensor_queue to empty it
    std::lock_guard<std::mutex> lock(queue_mutex);
        Logger::log("[INFO] flushing queue ...");
    while (!message_queue.empty()) {
        std::string msg = message_queue.front();
        boost::asio::write(socket, boost::asio::buffer(msg));
        Logger::log("[SENT] " + msg);
        message_queue.pop();
    }
}

void SensorManager::reconnect(tcp::socket& socket, tcp::resolver& resolver) {
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

void SensorManager::start() {
    if (running) {
        Logger::log("[INFO] SensorManager already running.");
        return;
    }

    running = true;
    Logger::log("[INFO] SensorManager started.");
}

void SensorManager::stop() {
    // Stop the sensor manager and all attached sensors
    if (!running) {
        Logger::log("[INFO] SensorManager already stopped.");
        return;
    }
    Logger::log("[INFO] Stopping SensorManager...");
    running = false;

    //Stop all sensors before removing them
    for (auto& sensor : sensors) {
        sensor->stop();
    }

    sensors.clear();  // Now safe to clear the vector 
    std::cout<<"sensors size: "<<sensors.size()<<std::endl;

    Logger::log("[INFO] SensorManager stopped.");
}