#include "data_receiver.hpp"
#include "logger.hpp"
#include <iostream>
#include <csignal>
#include <chrono>
#include <thread>

// Create the receiver and start it
int main() {
    DataReceiver receiver;

    receiver.run();

    return 0;
}
