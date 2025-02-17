# Compiler and flags
CXX = g++
# CXXFLAGS = -std=c++11 -Wall -I./include -I/usr/local/include/gtest
#TODO: google test need c++14!
CXXFLAGS = -std=c++14 -Wall -I./include -I/usr/local/include/gtest
LDFLAGS = -L /usr/local/lib -lboost_system -lboost_thread -lpthread -lgtest -lgtest_main

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
TEST_DIR = tests

# Source files
DATA_RECEIVER_SOURCES = $(SRC_DIR)/data_receiver.cpp  $(SRC_DIR)/logger.cpp 
SENSOR_SOURCES = $(SRC_DIR)/iot_device.cpp $(SRC_DIR)/sensor.cpp $(SRC_DIR)/device_manager.cpp $(SRC_DIR)/sensor_manager.cpp $(SRC_DIR)/logger.cpp $(SRC_DIR)/config_loader.cpp
PLC_SOURCES = $(SRC_DIR)/iot_device.cpp $(SRC_DIR)/plc.cpp $(SRC_DIR)/device_manager.cpp $(SRC_DIR)/plc_manager.cpp $(SRC_DIR)/logger.cpp $(SRC_DIR)/config_loader.cpp
TEST_SOURCES = $(TEST_DIR)/sensor_manager_test.cpp $(TEST_DIR)/plc_manager_test.cpp $(TEST_DIR)/data_receiver_test.cpp $(TEST_DIR)/main.cpp

# Object files
DATA_RECEIVER_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(DATA_RECEIVER_SOURCES))
SENSOR_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SENSOR_SOURCES))
PLC_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(PLC_SOURCES))
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(TEST_SOURCES))

# Executables
DATA_RECEIVER_EXECUTABLE = $(BIN_DIR)/data_receiver
SENSOR_EXECUTABLE = $(BIN_DIR)/sensor_manager
PLC_EXECUTABLE = $(BIN_DIR)/plc_manager
TEST_EXECUTABLE = $(BIN_DIR)/run_tests

# Build normal executables
all: $(BIN_DIR) $(SENSOR_EXECUTABLE) $(PLC_EXECUTABLE) $(DATA_RECEIVER_EXECUTABLE)

# Build test executable
test: $(BIN_DIR) $(TEST_EXECUTABLE)

# Build data_receiver
$(DATA_RECEIVER_EXECUTABLE): $(DATA_RECEIVER_OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -I./include $(SRC_DIR)/data_receiver_main.cpp $^ $(LDFLAGS) -o $@

# Build sensor_manager
$(SENSOR_EXECUTABLE): $(SENSOR_OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -I./include $(SRC_DIR)/sensor_manager_main.cpp $^ $(LDFLAGS) -o $@

# Build plc_manager
$(PLC_EXECUTABLE): $(PLC_OBJECTS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -I./include $(SRC_DIR)/plc_manager_main.cpp $^ $(LDFLAGS) -o $@

# Build run_tests
$(TEST_EXECUTABLE): $(TEST_OBJECTS) $(SENSOR_OBJECTS) $(PLC_OBJECTS) $(DATA_RECEIVER_OBJECTS)| $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure directories exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# **Utilities*
# Run tests
run_tests: test
	./bin/run_tests

# Clean compiled files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all test clean run_tests
