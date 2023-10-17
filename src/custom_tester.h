#ifndef CUSTOM_TESTER_H
#define CUSTOM_TESTER_H

#include "communication.h"
#include "test_results.h"
#include <string>
#include <unistd.h>

struct custom_tester_client_description {
    std::string client_ip;
    std::string server_ip;
    int port = 8090;
    int gap;                        // Time for one message cycle [ns]
    struct datagram {
        int  size;                  // Datagramgroesse
        bool random;                // Zufaellige Datagramgroesse (im Bereich 0 bis size)
    } datagram;
    int duration;                   // Duration of the test [s]
    bool qos;
};

struct custom_tester_server_description {
    std::string client_ip;
    std::string server_ip;
    int port = 8090;
    struct datagram {
        int  size;                  // Datagramgroesse
    } datagram;
    bool qos;
};

class custom_tester_client
{
public:
    custom_tester_client(custom_tester_client_description description);
    void run(struct test_results::custom* results);

private:
    custom_tester_client_description m_description;
    communication::udp::client m_comm_client;
    communication::udp::server m_comm_server;
};

class custom_tester_server
{
public:
    custom_tester_server(custom_tester_server_description description);
    void run(struct test_results::custom* results);

private:
    custom_tester_server_description m_description;
    communication::udp::client m_comm_client;
    communication::udp::server m_comm_server;
};

struct custom_tester_result_message {
    const enum communication::udp::message_type type = communication::udp::CRESU_MSG;
    long long int number_received = -1;
};

#endif // CUSTOM_TESTER_H
