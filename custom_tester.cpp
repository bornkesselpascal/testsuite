#include "custom_tester.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <unistd.h>

custom_tester_client::custom_tester_client(custom_tester_client_description description)
    : m_description(description)
    , m_comm_client(m_description.server_ip, m_description.port)
    , m_comm_server(m_description.own_ip, (m_description.port + 1))
{
}

long custom_tester_client::run() {
    void* data = malloc(m_description.datagram_size);
    communication::udp::message_type* msg_type = (communication::udp::message_type*) data;
    *msg_type = communication::udp::CDATA_MSG;
    size_t msg_size_current = m_description.datagram_size;

    long msg_counter = 0;

    struct timespec start_time, end_time, current_time, sleep_duration;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    end_time.tv_sec = start_time.tv_sec + m_description.duration_sec;
    end_time.tv_nsec = start_time.tv_nsec;
    sleep_duration.tv_sec = 0;
    sleep_duration.tv_nsec = m_description.nsec_message_gap;

    while(true) {
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        if(current_time.tv_sec >= end_time.tv_sec) {
            break;
        }

        m_comm_client.send(data, msg_size_current);
        msg_counter++;
        nanosleep(&sleep_duration, NULL);
    }

    sleep(1);

    communication::udp::message_type end_message = communication::udp::CSTOP_MSG;
    int bytes_send = m_comm_client.send(&end_message, sizeof(end_message));
    if(bytes_send == -1) {
        // handle error here...
    }

    free(data);

    custom_tester_result_message result;
    int bytes_received = m_comm_server.receive(&result, sizeof(custom_tester_result_message));
    if(bytes_received == -1) {
        // handle error here...
    }

    std::cout << "Packages sendt: " << msg_counter << std::endl;
    std::cout << "Packages received: " << result.number_received << std::endl;

    if(result.number_received != -1) {
        return (msg_counter - result.number_received);
    }
    return -1;
}



custom_tester_server::custom_tester_server(custom_tester_server_description description)
    : m_description(description)
    , m_comm_server(m_description.own_ip, m_description.port)
    , m_comm_client(m_description.client_ip, (m_description.port + 1))
{
}

long custom_tester_server::run() {
    void* data = malloc(m_description.datagram_size);
    communication::udp::message_type* msg_type = (communication::udp::message_type*) data;

    long msg_counter = 0;

    while(1) {
        m_comm_server.receive(data, m_description.datagram_size);

        if(*msg_type == communication::udp::message_type::CSTOP_MSG) {
            break;
        }
        msg_counter++;
    }

    free(data);

    custom_tester_result_message result;
    result.number_received = msg_counter;

    int bytes_send = m_comm_client.send(&result, sizeof(result));
    if(bytes_send == -1) {
        // handle error here...
    }

    std::cout << "Packages received: " << msg_counter << std::endl;

    return 0;
}
