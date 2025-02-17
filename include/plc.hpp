#ifndef PLC_HPP
#define PLC_HPP

#include "iot_device.hpp"

class PLC : public IoTDevice {
public:
    PLC(int id, std::queue<std::string>& queue);
    ~PLC();

protected:
    void generate_data() override;
};

#endif // PLC_HPP
