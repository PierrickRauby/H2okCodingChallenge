#include "data_receiver.hpp"
#include "logger.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>

using boost::asio::ip::tcp;
constexpr int PORT = 12345;

DataReceiver::DataReceiver() 
    : acceptor(io_context, tcp::endpoint(tcp::v4(), PORT)), 
      data_receiver_running(false) {}

void DataReceiver::run() {
    std::cout << "[INFO] Data Receiver listening on port " << PORT << "...\n";
    data_receiver_running = true;

    // Start accepting clients asynchronously, prevent blokcing
   start_accept();

    // asio loop
    io_context.restart();
    io_context.run();

    Logger::log("[INFO] Data Receiver stopped.");
}

void DataReceiver::start_accept() {
    if (!data_receiver_running) return;
    // New connection for potential new client 
    auto socket = std::make_shared<tcp::socket>(io_context);
    //start async accept for new client to connect
    acceptor.async_accept(*socket, [this, socket](const boost::system::error_code& error) {
        if (!error && data_receiver_running) {
            // on connection check that the receiver is still supposed to run 
            std::cout << "[INFO] Client connected.\n";
            std::thread(&DataReceiver::handle_client, this, socket).detach();
        } else if (error) {
            Logger::log("[ERROR] Accept failed: " + error.message());
        }

        // call itself for next connection
        start_accept();
    });
}

void DataReceiver::handle_client(std::shared_ptr<tcp::socket> socket) {
    try {
        while (data_receiver_running) {
            // get the data from asio
            boost::asio::streambuf buffer;
            boost::asio::read_until(*socket, buffer, "\n");
            // read data line by line
            std::istream is(&buffer);
            std::string received_data;
            std::getline(is, received_data);
            if (received_data.empty()) {
                std::cerr << "[INFO] Client disconnected.\n";
                break;
            }
            //Get time stamp
            long long timestamp = extract_timestamp(received_data);
            {
                // write to the the data queue
                std::lock_guard<std::mutex> lock(data_queue_mutex);
                data_log_queue.push({timestamp, received_data});
            }
            // Write logs now
            write_logs();
            //output data and ACK
            std::cout << "[RECEIVED] " << received_data << std::endl;
            boost::asio::write(*socket, boost::asio::buffer("ACK\n"));
        }
    } catch (std::exception& e) {
        std::cerr << "[ERROR] Client connection error: " << e.what() << "\n";
    }
}

void DataReceiver::write_logs() {

    std::lock_guard<std::mutex> lock(data_queue_mutex);
    // append to log files
    std::fstream log_file("data.log", std::ios::app);
    //TODO: Check that log file would be created if doesn't exist

    //write the entire data queue
    while (!data_log_queue.empty()) {
        DataToLog entry = data_log_queue.top();
        data_log_queue.pop();
        log_file << entry.data << std::endl;
    }
    log_file.close();
}

long long DataReceiver::extract_timestamp(const std::string& message) {
    size_t pos = message.find("\"timestamp\":");
    if (pos != std::string::npos) {
        pos += 11;  // Move past `"timestamp":`
        // Skip any whitespace between `timestamp:` and the number
        while (pos < message.size() && (message[pos] == ' ' || message[pos] == ':')) {
            pos++;
        }
        // Find the end of the timestamp (first non-digit character)
        size_t end = message.find_first_not_of("0123456789", pos);
        if (end == std::string::npos) {
            end = message.size();  
        }
        // get timestamp as a string
        std::string timestamp_str = message.substr(pos, end - pos);

        if (!timestamp_str.empty()) {
            try {
                return std::stoll(timestamp_str);
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Failed to convert timestamp: " << e.what() << std::endl;
            }
        } else {
            std::cerr << "[ERROR] Extracted empty timestamp!" << std::endl;
        }
    } else {
        std::cerr << "[ERROR] No timestamp found in message!" << std::endl;
    }
    // fail safe, if we don't have a timestamp, use now
    return std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
}


void DataReceiver::stop() {
    if (!data_receiver_running) return;
    //Cleanup
    data_receiver_running = false;
    acceptor.close();
    io_context.stop();
    //TODO: is the data_log_queue empty?
    Logger::log("[INFO] Data Receiver stopping...");
}
