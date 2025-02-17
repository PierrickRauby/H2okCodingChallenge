#include "plc_manager.hpp"
#include "logger.hpp"
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
// TODO: check port
constexpr char HOST[] = "127.0.0.1";
constexpr int PORT = 12345;
constexpr int RECONNECT_INTERVAL = 3;


PLCManager::PLCManager() {}

PLCManager::~PLCManager() {
    std::cout << "[DEBUG] Destructor: Stopping PLCManager" << std::endl;
    stop();
}

void PLCManager::add_plc(int id) {
    if(!plc_running){
        // Starting the plc manager before adding plcs
        start(); 
    }
    // Add the plc with correct id, then start it
    plcs.push_back(std::make_unique<PLC>(id, message_queue));
    plcs.back()->start();
    Logger::log("[INFO] PLC " + std::to_string(id) + " added and running.");
}


void PLCManager::send_data(bool testing) {
    // TODO: should be good to remove that and testing argument bc of 
    // TestPLCManager class
    // for testing we are not sending data (there is not data_receiver)
    if(testing){
        Logger::log("[MOCK] Skipping networking for test purposes...");
        return;
    }
    tcp::socket socket(io_context);
    tcp::resolver resolver(io_context);
    bool connected = false;

    while (plc_running) { 
        if (!connected) {
            //** if plc is running and not (never)connected**
            try {
                //try to connect
                boost::asio::connect(socket, resolver.resolve(HOST, std::to_string(PORT)));
                Logger::log("[INFO] Connected! Sending data...");
                connected = true;
                // and then flush the buffered data
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
                std::lock_guard<std::mutex> lock(plc_queue_mutex);
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

    Logger::log("[INFO] Stopping plc manager...");
}

void PLCManager::flush_buffer(tcp::socket& socket) {
    //Used to send all the message in the plc_queue to empty it
    std::lock_guard<std::mutex> lock(plc_queue_mutex);
        Logger::log("[INFO] flushing queue ...");
    while (!message_queue.empty()) {
        Logger::log("[SENT] " + message_queue.front());
        boost::asio::write(socket, boost::asio::buffer(message_queue.front()));
        message_queue.pop();
    }
}

void PLCManager::reconnect(tcp::socket& socket, tcp::resolver& resolver) {
    //Function used specifically to reconnect to the server and if successful
    // flush the buffer of data stored in the queue
    socket.close();
    while (plc_running) { 
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

void PLCManager::start() {
    if (plc_running) {
        Logger::log("[INFO] PLCManager already running.");
        return;
    }

    plc_running = true;
    Logger::log("[INFO] PLCManager started.");
}

void PLCManager::stop() {
    // Stop the sensor manager and all attached sensors
    if (!plc_running){
    Logger::log("[INFO] PLCManager already stopped...");

    }else{
    Logger::log("[INFO] Stopping PLCManager...");
    plc_running = false;

    }
    //Stop all plcs before removing them
    for (auto& plc : plcs) {
        plc->stop();
    }

    plcs.clear();  // Now safe to clear the vector
    std::cout<<"plcs size: "<<plcs.size()<<std::endl;

    Logger::log("[INFO] PLCManager stopped.");
}