# H2okCodingChallenge

![Status](https://github.com/PierrickRauby/H2OkEmbeddedChallenge/workflows/ci.yml/badge.svg)

<details><summary><h2>Content</h2></summary>

- [H2okCodingChallenge](#h2okcodingchallenge)
  - [How to Use](#how-to-use)
    - [Clone the Repository](#clone-the-repository)
    - [Build and Test Locally](#build-and-test-locally)
    - [Dependencies](#dependencies)
  - [Class Documentation](#class-documentation)
      - [DataReceiver](#datareceiver)
      - [DeviceManager](#devicemanager)
      - [SensorManager](#sensormanager)
      - [PLCManager](#plcmanager)
      - [PLC](#plc)
      - [Sensor](#sensor)
      - [IoTDevice](#iotdevice)
  - [Branch descriptions](#branch-descriptions)
  - [Challenge Description](#challenge-description)
</details>


## How to Use

### Clone the Repository

```bash
git clone https://github.com/PierrickRauby/H2okCodingChallenge 
cd CodingPractice
```

### Build and Test Locally

To build the components use the command below.
This should result in 3 Components located in the `/bin` directory:`data_receiver`, `plc_manager` and, `sensor_manager`.

   ```bash
   make
   ```

Alternatively, you can build and execute tests:
  `run_tests` that will test various test cases (unit tests and integration tests).

   ```bash
   make test
   ```

### Dependencies

This project uses google tests as well as the boost libraries.

## Class Documentation

<details><summary><h3>Component Level Classes</h3></summary>

#### DataReceiver

The `DataReceiver` class (i.e. Component 3)  responsible for receiving data from sensors and PLCs. It listens on a specified port and handles incoming connections to receive data. The received data is logged in the `data.log` file, ordered by timestamps.

- `void run()`: start the data receiving operations, and start listening for incoming TCP connections
- `void stop()`: stops data receiving operations and cleans up asio objects

#### DeviceManager

The `DeviceManager` is the base class for the `SensorManager` and the `PLCManager` presented below.

- `send_data(bool testing = false)`: Sends data to the `DataReceiver` and manages disconnection and reconnection.
- `get_device_count()`: gets the number of sensor from both type associated to the current manager.
- `get_message_queue()`: gets the data queue to be sent to the receiver.

#### SensorManager

The `SensorManager` class (i.e. Component 1), gets data from the sensors at 1Hz. It retrieves in formation about the number of sensors of Type A and Type B from a configuration file at startup. It also handles deconnection from the `DataReceiver` by buffering the data locally and transmitting it back to the `DataReceiver` once connection is reestablished.

- `add_sensor()`: add a new sensor to the array of sensor pointers and starts it.

Note: the `SensorManager` is derived from the `DeviceManager` class from which it gets most of its functionnalities.

#### PLCManager

The `PLCManager` class (i.e. Component 1), gets data from the plc at 1Hz. It get the information about the number of plcs from a configuration files when started. It also handles deconnection from the `DataReceiver` by buffering the data locally and transmitting it back to the `DataReceiver` once connection is rehestablished.

- `add_plc()`: add a new plc to the array of plc pointers and starts it.

Note: the `PLCManager` is derived from the `DeviceManager` class from which it gets most of its functionnalities.
</details>

<details><summary><h3>Sensor and PLC level classes</h3></summary>

#### PLC

The `PLC` inherits from the `IOTDevice` class, and only exposes a protected method that generates data specific to PLC devices.

- `generate_data()`: Generates data and places it in the queue of the plc manager.

Note: there is an hardcoded "failure" rate of 3% on PLC data to simulate offline sensors conditions for plcs.

#### Sensor

The `Sensor` inherits from the `IOTDevice` class, and only exposes a protected method that generates data specific to sensor devices of each types.

- `generate_data()`: Generates data and places it in the queue of the sensor manager.

Note: there is an hardcoded "failure" rate of 5% on sensor data to simulate offline sensors conditions for plcs.

#### IoTDevice

The `IoTDevice` is the base class for the `Sensor` and the `PLC`. Manages the process of pushing data to the corresponding device manager.

- `start()`: start the data generation thread for the device.
- `stop()`: safely stops the data generation for the device.
</details>


## Branch descriptions

- main: The stable branch containing the latest validated changes to the test and infrastructure.
- dev: A working branch for testing and development before merging into main.

## Challenge Description

H2Ok Innovations is developing a Smart Factory System (SFS). The factory in mind has two
types of sensors, Type A and B, and one type of programmable logic controller (PLC), which
contains operational data. Write 3 software components for the system. Each component should
be a separate executable, as described below:

- Component 1 reads from both types of sensors once per second. Each reading
produces a record consisting of a timestamp and an array of data.
- Component 2 reads from a PLC once per second. Each reading also produces a record
consisting of a timestamp and an array of data.
- Component 3 combines the data produced by Component 1 and 2, and generates a final
data record, each consisting of a sensor reading record and a PLC data record with their
original timestamps preserved. It should try to align the two records over the timestamps
as best as it can.

If Component 3 is offline temporarily, for example, due to software upgrade, the data produced
by Component 1 and 2 must be preserved. When the Component 3 is back online again, it will
process the data.

The number of sensors as well as the type of each are unknown at the development time. It’s
not allowed to recompile the software or modify the Python script for different sets of sensors.
Using a configuration file is highly desirable.

The software should handle sensor or PLC offline conditions gracefully and report them.

The underlying communication protocols between the SFS and the sensors or PLC are omitted
in the exercise. Using stub functions to return simulated values for the sensors and PLC data is
sufficient.

If any details you feel are missing for you to complete the software, please make necessary
assumptions and write the software accordingly. If you believe what’s missing is critical, please
feel free to reach out to us for clarification.

You can use C/C++, Python, Java, Go, or any programming language that you are most
comfortable with.

Bonus point: use UML diagrams to describe your software.
Bonus point: structure the software so it’s easy for unit tests, and write a unit test.

Note: Using AI code generation software for this exercise is prohibited. Doing so will
disqualify you from consideration.

Copyright © 2025 H2Ok Innovations, Inc