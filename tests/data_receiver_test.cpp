#include <gtest/gtest.h>
#include "../include/data_receiver.hpp"
#include "../include/logger.hpp"
#include <boost/asio.hpp>
#include <thread>
#include <iostream>

constexpr int TEST_PORT = 12345;

// ** Test class for DataReceiver**
// for clean setupand cleanup before each test
class DataReceiverTest : public ::testing::Test {
protected:
    DataReceiver* receiver;
    std::thread receiver_thread;
    //setup
    void SetUp() override {
        // creatae a new data receiver, starts it and wait 1sec
        receiver = new DataReceiver();  
        receiver_thread = std::thread([this]() { receiver->run(); });
        std::this_thread::sleep_for(std::chrono::seconds(1)); 
    }
    // cleanup
    void TearDown() override {
        //Stop the receiver, and ensure that all threads are exiting
        std::cout<<"Tearing down"<<std::endl;
        receiver->stop();  // Stop the receiver
        if (receiver_thread.joinable()) {
            receiver_thread.join(); 
        }
        delete receiver;
    }
};

// **Test if DataReceiver starts and stops properly**
TEST_F(DataReceiverTest, StartAndStop) {
    // check the data receiver has been created
    ASSERT_TRUE(receiver != nullptr);
    receiver->stop();  // Stop the receiver
}

// **Test if DataReceiver accepts connections**
TEST_F(DataReceiverTest, AcceptClientConnection) {
    try {
        // Create a mock client to connect to DataReceiver on porr 12345
        //asio stuff
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::socket socket(io_context);
        boost::asio::ip::tcp::resolver resolver(io_context);
        
        // try to connect to the data reciever 
        boost::asio::connect(socket, resolver.resolve("127.0.0.1", std::to_string(TEST_PORT)));
        //are we connected?
        ASSERT_TRUE(socket.is_open());  
    } catch (std::exception& e) {
        FAIL() << "Failed to connect to DataReceiver: " << e.what();
    }
}

// **Test if DataReceiver processes received data and responds with ACK**
TEST_F(DataReceiverTest, ReceiveDataAndSendAck) {
    try {
        // Create a mock client to connect to DataReceiver on porr 12345
        //asio stuff
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::socket socket(io_context);
        boost::asio::ip::tcp::resolver resolver(io_context);

        // Connect to the DataReceiver
        boost::asio::connect(socket, resolver.resolve("127.0.0.1", std::to_string(TEST_PORT)));

        // Send data to the reciever
        std::string test_message = "Hello\n";
        boost::asio::write(socket, boost::asio::buffer(test_message));

        // Read resp
        boost::asio::streambuf buffer;
        boost::asio::read_until(socket, buffer, "\n");

        std::istream is(&buffer);
        std::string response;
        std::getline(is, response);

        // expect ACK!
        EXPECT_EQ(response, "ACK");
    } catch (std::exception& e) {
        FAIL() << "Error during data exchange: " << e.what();
    }
}
