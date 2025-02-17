#ifndef PLC_HPP
#define PLC_HPP

#include <queue>
#include <string>
#include <thread>
#include <mutex>

extern std::mutex plc_queue_mutex;
class PLC {
public:

    PLC(int id, std::queue<std::string>& queue);
    ~PLC();
    void generate_data();
    void start();
    void stop();

private:
    char type; //PLCs have a single type
    int id; //unique ID, retrieve via Config file
    std::queue<std::string>& message_queue; // data queue for the plc
    bool plc_running=false;
    std::thread plc_thread;//data generation thread for that plc

};


#endif // PLC_HPP
