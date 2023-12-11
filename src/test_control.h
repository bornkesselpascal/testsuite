#ifndef TEST_CONTROL_H
#define TEST_CONTROL_H

#include <string>
#include <vector>
#include "test_description.h"
#include "test_results.h"

struct service_connection {
    std::string server_ip;
    int         port;
};

struct client_description {
    std::string path;
    int duration;

    // If dynamic behavoir is set, the respective values for Datagram Size or Cycle Time are ignored.
    struct dynamic_behavoir {
        enum mode {
            DISABLED,
            DATAGRAM_SIZE,
            CYCLE_TIME,
        } mode = client_description::dynamic_behavoir::DISABLED;

        int min;
        int max;
        int steps;
    } dynamic_behavoir;

    struct target_connection {
        enum uce::sock_type type;
        std::string client_ip;
        std::string server_ip;
        int  port;
        int  cycletime;
        std::vector<int>  datagram_sizes;
    } target_connection;

    struct interface {
        std::string client;
        std::string server;
    } interface;

    struct service_connection service_connection;

    struct stress {
        stress_type type;
        int num;
        stress_location location;
    } stress;

    enum test_description::latency_measurement latency_measurement = test_description::latency_measurement::DISABLED;
};

struct server_description {
    std::string path;
    struct service_connection service_connection;
};



enum class testsuite_type {
    CLIENT,
    SERVER,
};

class test_control_parser
{
public:
    static client_description read_client_from_XML(std::string filename);
    static server_description read_server_from_XML(std::string filename);
    static std::vector<std::string> read_main_XML(std::string filename, testsuite_type type);

    test_control_parser() = delete;
};

#endif // TEST_CONTROL_H
