#include "config_loader.hpp"
#include "logger.hpp"
#include <fstream>
#include <sstream>

constexpr char CONFIG_FILE[] = "config.txt";

std::map<std::string, int> load_config(const std::string& type) {
    std::map<std::string, int> config;
    // Default config for sensors and PLC    
    if (type == "sensor") {
        config = { {"SENSOR_A", 1}, {"SENSOR_B", 1} };  // Default sensor values
    } else if (type == "plc") {
        config = { {"PLC_COUNT", 1} };  // Default PLC value
    }

    std::ifstream file(CONFIG_FILE);
    if (!file.is_open()) {
        Logger::log("[ERROR] Could not open config file. Using default values for " + type + ".");
        return config;
    }

    //read config file line by line
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        int value;

        if (std::getline(iss, key, '=') && (iss >> value)) {
            config[key] = value;
        }
    }

    if (type == "sensor") {
        Logger::log("[INFO] Loaded sensor config: SENSOR_A=" + std::to_string(config["SENSOR_A"]) +
                    ", SENSOR_B=" + std::to_string(config["SENSOR_B"]));
    } else if (type == "plc") {
        Logger::log("[INFO] Loaded PLC config: PLC_COUNT=" + std::to_string(config["PLC_COUNT"]));
    }
    
    return config;
}
