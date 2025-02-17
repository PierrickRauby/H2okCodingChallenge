#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <fstream>
#include <mutex>
#include <string>

class Logger {
public:
    static void log(const std::string& message);
};

#endif // LOGGER_HPP
