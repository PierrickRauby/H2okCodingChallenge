#include "logger.hpp"

std::mutex log_mutex;
// TODO: have multiple log files!
constexpr char LOG_FILE[] = "log_file.log";

void Logger::log(const std::string& message) {
    // log on file with corresponding timestamp
    std::lock_guard<std::mutex> lock(log_mutex);
    std::ofstream log_file(LOG_FILE, std::ios::app);
    if (log_file.is_open()) {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        log_file << "[" << std::ctime(&now) << "] " << message << std::endl;
    }
    std::cout << message << std::endl;
}
