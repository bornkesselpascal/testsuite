#ifndef CUSTOM_STRESSOR_H
#define CUSTOM_STRESSOR_H

#include "test_description.h"
#include <memory>
#include <thread>

struct custom_stressor_description {
    std::string companion_ip;
    struct datagram {
        int size;
    } datagram;

};

class custom_stressor_network
{
public:
    custom_stressor_network(std::string path, int duration, int num, stress_location location);
    void start();
    void stop();

private:
    bool read_from_XML(std::string path);

    std::string m_iperf_command;
    bool        m_server = false;
    std::unique_ptr<std::thread> m_thread_ptr;

    custom_stressor_description m_description;
    int m_duration;
};

#endif // CUSTOM_STRESSOR_H
