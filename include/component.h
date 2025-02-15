#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <boost/asio.hpp> //for tcp communication between components
#include <fstream> // logging 
#include <chrono> // timestamps
#include <thread> 

class Component{
    protected:
    std::string name; // Name of the component (1,2,3)
    bool is_running; 
    bool is_connected; // Is there an in or outbound connection active
    std::ofstream log_file; //to log data and errors

    public:
    Component(const std::string& component_name);
    ~Component();
    // TODO: start/stop for now, but not sure if I need this in the long run
    void start();
    void stop();
    virtual void run()=0;

    protected:
    // TODO: timestamp should probably be hanlded by the sensors??
    std::string get_timestamp(); //helper for timestamp generation 
    bool log(const std::string& message); //0 for success, 1 for failure
    
}

#endif //COMPONENT_H