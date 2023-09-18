#ifndef CUSTOM_TESTER_H
#define CUSTOM_TESTER_H

#include "communication.h"
#include <string>

struct custom_tester_client_description {
    std::string own_ip;
    std::string server_ip;
    int port = 8090;
    int datagram_size;
    int nsec_message_gap;
    int duration_sec;
};

struct custom_tester_server_description {
    std::string own_ip;
    std::string client_ip;
    int port = 8090;
    int datagram_size;
};

class custom_tester_client
{
public:
    custom_tester_client(custom_tester_client_description description);
    long run();

private:
    custom_tester_client_description m_description;
    communication::udp::client m_comm_client;
    communication::udp::server m_comm_server;
};

class custom_tester_server
{
public:
    custom_tester_server(custom_tester_server_description description);
    long run();

private:
    custom_tester_server_description m_description;
    communication::udp::client m_comm_client;
    communication::udp::server m_comm_server;
};


struct custom_tester_result_message {
    const enum communication::udp::message_type type = communication::udp::CRESU_MSG;
    long number_received = -1;
};

#endif // CUSTOM_TESTER_H
