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

custom_tester_client::custom_tester_client(custom_tester_description description)
    : m_description(description)
    , m_sut_client(m_description.sock_type, m_description.src_ip, m_description.dst_ip, m_description.dst_port)
    , m_helper_server(m_description.src_ip, (m_description.dst_port + 8))
{
    if(m_description.timestamps.enabled)
    {
        m_sut_client.enable_timestamps(m_description.timestamps.mode);
    }
}

bool custom_tester_client::run(struct test_results::custom* results) {
    void* buffer = malloc(m_description.datagram_size);
    if(NULL == buffer) {
        return false;
    }

    custom_tester_test_message* test_message = reinterpret_cast<custom_tester_test_message*>(buffer);
    test_message->type = CTMT_TEST;
    test_message->sequence_number = 0;

    long unsigned int timer_misses = 0;

    struct timespec start_time, end_time, current_time, query_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    end_time.tv_sec = start_time.tv_sec + m_description.duration;
    end_time.tv_nsec = start_time.tv_nsec;

    results->timestamps.reserve(2000 * m_description.duration);
    timestamp_record record_buffer;

    helpers::timer cycle_timer;
    cycle_timer.initialize(m_description.cycletime);

    while(true) {
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        if(current_time.tv_sec >= end_time.tv_sec) {
            break;
        }

        test_message->sequence_number++;
        m_sut_client.send(buffer, m_description.datagram_size);

        if (m_description.timestamps.enabled) {
            // if (test_message->sequence_number % 10000) {
                record_buffer.sequence_number = test_message->sequence_number;
                record_buffer.m_snt_program = m_sut_client.client_timestamps.m_snt_program;
                results->timestamps.push_back(record_buffer);
            // }
        }

        if(m_description.query.enabled) {
            // TODO: implement query and dynamic scenario
        }

        timer_misses += cycle_timer.wait();
    }

    // Get timestamp for test results.
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    if ((current_time.tv_nsec - start_time.tv_nsec) < 0) {
        query_time.tv_sec = current_time.tv_sec - start_time.tv_sec - 1;
        query_time.tv_nsec = current_time.tv_nsec - start_time.tv_nsec + 1000000000L;
    } else {
        query_time.tv_sec = current_time.tv_sec - start_time.tv_sec;
        query_time.tv_nsec = current_time.tv_nsec - start_time.tv_nsec;
    }


    sleep(4);

    // Send stop message.
    custom_tester_message_type end_message = CTMT_END;
    int bytes_snt = m_sut_client.send(&end_message, sizeof(end_message));
    if (bytes_snt == -1) {
        free(buffer);
        return false;
    }

    custom_tester_result_message server_results;
    int bytes_rcv = m_helper_server.receive(&server_results, sizeof(custom_tester_result_message));
    if (bytes_rcv == -1) {
        free(buffer);
        return false;
    }

    // Print info message.
    std::cout << std::endl;
    std::cout << "pkg_snt: " << test_message->sequence_number << std::endl;
    std::cout << "tmr_mis: " << timer_misses << std::endl;

    results->num_total = test_message->sequence_number;
    results->num_misses = timer_misses;
    results->elapsed_time = query_time.tv_sec + (query_time.tv_nsec * 1e-9);

    free(buffer);
    close(m_sut_client.get_socket());
    close(m_helper_server.get_socket());

    return true;
}



//
//                 ___ ___ _ ___ _____ _ _
//                (_-</ -_) '_\ V / -_) '_|
//  custom_tester_/__/\___|_|  \_/\___|_|
//

custom_tester_server::custom_tester_server(custom_tester_description description)
    : m_description(description)
    , m_sut_server(m_description.sock_type, m_description.dst_ip, m_description.dst_port)
    , m_helper_client(m_description.src_ip, (m_description.dst_port + 8))
{
    if(m_description.timestamps.enabled)
    {
        m_sut_server.enable_timestamps(m_description.timestamps.mode);
    }
}

bool custom_tester_server::run(struct test_results::custom* results) {
    void* buffer = malloc(UCE_MTU);
    if(NULL == buffer) {
        return false;
    }

    custom_tester_message_type* message_type;
    custom_tester_test_message* test_message;
    switch (m_description.sock_type)
    {
    case uce::sock_type::ST_UDP:
        message_type = reinterpret_cast<custom_tester_message_type*>(buffer);
        test_message = reinterpret_cast<custom_tester_test_message*>(buffer);
        break;
    case uce::sock_type::ST_RAW:
        message_type = (custom_tester_message_type*)((char*) buffer + UCE_HEADER_IP_SIZE + UCE_HEADER_UDP_SIZE);
        test_message = (custom_tester_test_message*)((char*) buffer + UCE_HEADER_IP_SIZE + UCE_HEADER_UDP_SIZE);
        break;
    case uce::sock_type::ST_PACKET:
        message_type = (custom_tester_message_type*)((char*) buffer + UCE_HEADER_ETH_SIZE + UCE_HEADER_IP_SIZE + UCE_HEADER_UDP_SIZE);
        test_message = (custom_tester_test_message*)((char*) buffer + UCE_HEADER_ETH_SIZE + UCE_HEADER_IP_SIZE + UCE_HEADER_UDP_SIZE);
        break;
    }

    long unsigned int sequence_number = 0;

    results->timestamps.reserve(2000000 * m_description.duration);
    timestamp_record record_buffer;

    while(1) {
        m_sut_server.receive(buffer, UCE_MTU);

        if(*message_type == CTMT_END) {
            break;
        }
        else if(*message_type == CTMT_TEST) {
            if (m_description.timestamps.enabled) {
                record_buffer.sequence_number = test_message->sequence_number;
                record_buffer.m_rec_sw = m_sut_server.server_timestamps.m_rec_sw;
                record_buffer.m_rec_program = m_sut_server.server_timestamps.m_rec_program;
                results->timestamps.push_back(record_buffer);
            }
        }

        sequence_number++;
    }

    custom_tester_result_message server_results;
    server_results.number_received = sequence_number;

    sleep(1);

    int bytes_send = m_helper_client.send(&server_results, sizeof(server_results));
    if(bytes_send == -1) {
        free(buffer);
        return false;
    }

    free(buffer);
    close(m_sut_server.get_socket());
    close(m_helper_client.get_socket());

    return true;
}
