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
