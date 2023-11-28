#ifndef CUSTOM_TESTER_H
#define CUSTOM_TESTER_H

#include "communication.h"
#include "lib/uce/uce.h"
#include "test_results.h"
#include <string>
#include <unistd.h>

struct custom_tester_description {
    uce::sock_type sock_type;
    std::string src_ip;
    std::string dst_ip;
    int dst_port;

    int duration;           // Test duration [s]
    int datagram_size;      // Datagram size [byte]
    int cycletime;          // Message cycle [ns]

    struct timestamps {
        bool enabled = false;
        uce::timestamp_mode mode;
    } timestamps;

    struct query {
        bool enabled = false;
        bool log = false;
        int frequency;
    } query;

    struct dynamic_scenario {
        bool enabled = false;
        int loss_limit;
    } dynamic_scenario;
};



class custom_tester_client
{
public:
    custom_tester_client(custom_tester_description description);
    bool run(struct test_results::custom* results);

private:
    custom_tester_description m_description;

    uce::client m_sut_client;
    communication::udp::server m_helper_server;
};

class custom_tester_server
{
public:
    custom_tester_server(custom_tester_description description);
    bool run(struct test_results::custom* results);

private:
    custom_tester_description m_description;

    uce::server m_sut_server;
    communication::udp::client m_helper_client;
};



enum custom_tester_message_type {
    CTMT_TEST,
    CTMT_END,
    CTMT_RESULT,
};

struct custom_tester_test_message {
    enum custom_tester_message_type type = CTMT_TEST;
    long unsigned int sequence_number = 0;
};

struct custom_tester_result_message {
    enum custom_tester_message_type type = CTMT_RESULT;
    long long int number_received = -1;
};

#endif // CUSTOM_TESTER_H
