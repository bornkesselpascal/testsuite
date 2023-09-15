#ifndef TEST_CONTROL_COMMON_H
#define TEST_CONTROL_COMMON_H

#include <string>
#include "test_description.h"
#include "test_results.h"
#include "communication.h"

struct service_connection {
    std::string ip;
    int         port;
};

struct client_description {
    enum test_description::metadata::method method;
    std::string path;

    struct duration {
        int min_duration;
        int mid_duration;
        int max_duration;
    } duration;

    struct target_connection {
        std::string ip;
        std::string bandwidth;
        int datagramsize;
    } target_connection;

    struct interface {
        std::string client;
        std::string server;
    } interface;

    service_connection svc_connection;
    bool client_only = false;

    struct stress {
        enum test_description::stress::type type;
        struct num {
            int num_min;
            int num_max;
            int steps;
        } num;
    } stress;
};

struct server_description {
    enum test_description::metadata::method method;
    std::string path;

    service_connection svc_connection;
    std::string interface;

    int datagramsize;
};

struct test_description_message {
    const enum communication::udp::message_type type = communication::udp::DESCR_MSG;
    struct test_description description;
};


class test_control_common
{
public:
    static void log_control (client_description description);
    static void log_control (server_description description);
    static void log_scenario(test_description description, test_results* results = nullptr);

    test_control_common() = delete;
};

#endif // TEST_CONTROL_COMMON_H
