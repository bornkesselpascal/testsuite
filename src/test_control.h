#ifndef TEST_CONTROL_H
#define TEST_CONTROL_H

#include <string>
#include "test_description.h"
#include "test_results.h"

struct service_connection {
    std::string server_ip;
    int         port;
};

struct client_description {
    enum test_description::metadata::method method;
    std::string path;

    struct duration {
        int short_duration;
        int long_duration;
    } duration;

    struct target_connection {
        std::string client_ip;
        std::string server_ip;
        std::string bandwidth_limit;
        int port;
        int gap;
        struct datagram {
            int size_max;
            int steps;
            bool random;
        } datagram;
    } target_connection;

    struct interface {
        std::string client;
        std::string server;
    } interface;

    struct service_connection service_connection;
    bool client_only = false;

    struct stress {
        enum test_description::stress::type type;
        struct num {
            int num_min;
            int num_max;
            int steps;
        } num;
        enum test_description::stress::location location;
    } stress;
};

struct server_description {
    std::string path;
    struct service_connection service_connection;
};



class test_control_parser
{
public:
    static client_description read_client_from_XML(std::string filename);
    static void               write_client_to_XML(std::string filename, client_description &description);

    static server_description read_server_from_XML(std::string filename);
    static void               write_server_to_XML(std::string filename, server_description &description);

    test_control_parser() = delete;
};



class test_control_logger
{
public:
    static void log_control    (client_description description);
    static void log_control    (server_description description);
    static void log_description(test_description description, test_results* results = nullptr);

    test_control_logger() = delete;
};

#endif // TEST_CONTROL_H
