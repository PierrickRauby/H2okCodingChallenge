#ifndef PLC_MANAGER_HPP
#define PLC_MANAGER_HPP

#include "device_manager.hpp"
#include "plc.hpp"

class PLCManager : public DeviceManager {
public:
    PLCManager();
    ~PLCManager();

    void add_plc(int id);

protected:
    std::unique_ptr<IoTDevice> create_device(int id) override;
};

#endif // PLC_MANAGER_HPP
