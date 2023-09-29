#include "custom_tester.h"
#include "helpers.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <unistd.h>



//                    _ _         _
//                 __| (_)___ _ _| |_
//                / _| | / -_) ' \  _|
//  custom_tester_\__|_|_\___|_||_\__|
//

custom_tester_client::custom_tester_client(custom_tester_client_description description)
    : m_description(description)
    , m_comm_client(m_description.server_ip, m_description.port)
    , m_comm_server(m_description.client_ip, (m_description.port + 1))
{
}

void custom_tester_client::run(struct test_results::custom* results) {
    void* data = malloc(m_description.datagram.size);
    communication::udp::message_type* msg_type = (communication::udp::message_type*) data;
    *msg_type = communication::udp::CDATA_MSG;

    long long int msg_counter = 0;
    int tmr_misses = 0;

    struct timespec start_time, end_time, current_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    end_time.tv_sec = start_time.tv_sec + m_description.duration;
    end_time.tv_nsec = start_time.tv_nsec;

    helpers::timer gap_timer;

    if(m_description.datagram.random) {
        // RANDOM DATAGRAM SIZE (0 - size)
        size_t msg_size;
        size_t max_size = m_description.datagram.size;
        size_t min_size = sizeof(communication::udp::message_type);
        gap_timer.initialize(m_description.gap);

        while(true) {
            clock_gettime(CLOCK_MONOTONIC, &current_time);
            if(current_time.tv_sec >= end_time.tv_sec) {
                break;
            }
            msg_size = (std::rand() % (max_size - min_size)) + min_size;

            m_comm_client.send(data, msg_size);
            msg_counter++;
            tmr_misses += gap_timer.wait();
        }
    }
    else {
        // FIXED DATAGRAM SIZE (size)
        size_t msg_size = m_description.datagram.size;
        gap_timer.initialize(m_description.gap);

        while(true) {
            clock_gettime(CLOCK_MONOTONIC, &current_time);
            if(current_time.tv_sec >= end_time.tv_sec) {
                break;
            }

            m_comm_client.send(data, msg_size);
            msg_counter++;
            tmr_misses += gap_timer.wait();
        }
    }

    free(data);
    sleep(4);

    communication::udp::message_type end_message = communication::udp::CSTOP_MSG;
    int bytes_send = m_comm_client.send(&end_message, sizeof(end_message));
    if(bytes_send == -1) {
        std::cerr << "[custom_tester] Error when sending end message." << std::endl;
    }

    custom_tester_result_message server_results;
    int bytes_received = m_comm_server.receive(&server_results, sizeof(custom_tester_result_message));
    if(bytes_received == -1) {
        std::cerr << "[custom_tester] Error when recieving result message." << std::endl;
    }

    std::cout << "Packages send    : " << msg_counter << std::endl;
    std::cout << "Packages received: " << server_results.number_received << std::endl;
    std::cout << "Timer misses     : " << tmr_misses << std::endl;

    results->num_total = msg_counter;
    results->num_loss  = (msg_counter - server_results.number_received);
    results->num_misses = tmr_misses;

    close(m_comm_server.get_socket());
    close(m_comm_client.get_socket());
}



//
//                 ___ ___ _ ___ _____ _ _
//                (_-</ -_) '_\ V / -_) '_|
//  custom_tester_/__/\___|_|  \_/\___|_|
//

custom_tester_server::custom_tester_server(custom_tester_server_description description)
    : m_description(description)
    , m_comm_server(m_description.server_ip, m_description.port)
    , m_comm_client(m_description.client_ip, (m_description.port + 1))
{
}

void custom_tester_server::run(struct test_results::custom* results) {
    void* data = malloc(m_description.datagram.size);
    communication::udp::message_type* msg_type = (communication::udp::message_type*) data;

    long long int msg_counter = 0;

    while(1) {
        m_comm_server.receive(data, m_description.datagram.size);

        if(*msg_type == communication::udp::message_type::CSTOP_MSG) {
            break;
        }
        msg_counter++;
    }

    free(data);

    custom_tester_result_message server_results;
    server_results.number_received = msg_counter;

    sleep(1);

    int bytes_send = m_comm_client.send(&server_results, sizeof(server_results));
    if(bytes_send == -1) {
        std::cerr << "[custom_tester] Error when sending result message." << std::endl;
    }

    close(m_comm_server.get_socket());
    close(m_comm_client.get_socket());
}
