#ifndef CONFIG_LOADER_HPP
#define CONFIG_LOADER_HPP

#include <map>
#include <string>

// Function to load sensor configuration from a simple text file
// avoids recompiling when number of sensors or plc changes
std::map<std::string, int> load_config(const std::string& type);

#endif // CONFIG_LOADER_HPP
