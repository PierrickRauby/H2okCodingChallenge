#include "data_receiver.hpp"
#include "logger.hpp"
#include <iostream>
#include <thread>
#include <vector>

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
            //output data and ACK
            std::cout << "[RECEIVED] " << received_data << std::endl;
            boost::asio::write(*socket, boost::asio::buffer("ACK\n"));
        }
    } catch (std::exception& e) {
        std::cerr << "[ERROR] Client connection error: " << e.what() << "\n";
    }
}

void DataReceiver::stop() {
    if (!data_receiver_running) return;
    //Cleanup
    data_receiver_running = false;
    acceptor.close();
    io_context.stop();
    Logger::log("[INFO] Data Receiver stopping...");
}
