#include "iperf.h"
#include "custom_tester.h"
#include <future>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <thread>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <fstream>

void custom_tester_thread_client(custom_tester_description t_description, struct test_results::custom* results);
void custom_tester_thread_server(custom_tester_description t_description, struct test_results::custom* results);

//            _ _         _
//         __| (_)___ _ _| |_
//        / _| | / -_) ' \  _|
//  iperf_\__|_|_\___|_||_\__|
//

/**
 * @brief iperf_client::iperf_client
 * Prepares iPerf or custom_tester for the current test.
 *
 * @param description Test Description of current scenario.
 */
iperf_client::iperf_client(test_description description) {
    m_description = description;

    m_custom_description.sock_type     = m_description.connection.type;
    m_custom_description.src_ip        = m_description.connection.client_ip;
    m_custom_description.dst_ip        = m_description.connection.server_ip;
    m_custom_description.dst_port      = m_description.connection.port;

    m_custom_description.duration      = m_description.duration;
    m_custom_description.datagram_size = m_description.connection.datagram_size;
    m_custom_description.cycletime     = m_description.connection.cycletime;

    // timestamps (fixed)
    if(description.latency.measurement != test_description::latency::measurement::DISABLED) {
        m_custom_description.timestamps.enabled = true;

        if(description.latency.measurement == test_description::latency::measurement::END_TO_END) {
            m_custom_description.timestamps.mode    = uce::timestamp_mode::TSTMP_SW;
        }
        else if(description.latency.measurement == test_description::latency::measurement::FULL) {
            m_custom_description.timestamps.mode    = uce::timestamp_mode::TSTMP_ALL;
        }
    }

    // query (fixed)
    m_custom_description.query.enabled = false;

    // dynamic scenario (fixed)
    m_custom_description.dynamic_scenario.enabled = false;
}

void iperf_client::start() {
    m_results.ethtool_statistic_start = metrics::get_ethtool_statistic(m_description.interface.client);
    m_results.ip_statistic_start      = metrics::get_ip_statistic(m_description.interface.client);
    m_results.netstat_statistic_start = metrics::get_netstat_statistic();

    m_thread_ptr = std::unique_ptr<std::thread>(new std::thread(custom_tester_thread_client, m_custom_description, &(m_results.custom)));
}

test_results iperf_client::get_results() {
    if(m_thread_ptr == nullptr) {
        throw std::runtime_error("[iperf_client] E01 - Custom_tester control thread does not exist. Cannot get results.");
    }
    m_thread_ptr->join();

    m_results.ethtool_statistic_end = metrics::get_ethtool_statistic(m_description.interface.client);
    m_results.ip_statistic_end      = metrics::get_ip_statistic(m_description.interface.client);
    m_results.netstat_statistic_end = metrics::get_netstat_statistic();

    m_results.status                = test_results::STATUS_SUCCESS;
    return m_results;
}



//
//         ___ ___ _ ___ _____ _ _
//        (_-</ -_) '_\ V / -_) '_|
//  iperf_/__/\___|_|  \_/\___|_|
//

void iperf_server::start() {
}

void iperf_server::load_test(test_description description) {
    m_description = description;

    m_custom_description.sock_type     = m_description.connection.type;
    m_custom_description.src_ip        = m_description.connection.client_ip;
    m_custom_description.dst_ip        = m_description.connection.server_ip;
    m_custom_description.dst_port      = m_description.connection.port;

    m_custom_description.duration      = m_description.duration;
    m_custom_description.datagram_size = m_description.connection.datagram_size;
    m_custom_description.cycletime     = m_description.connection.cycletime;

    // timestamps (fixed)
    m_custom_description.timestamps.enabled = true;
    m_custom_description.timestamps.mode    = uce::timestamp_mode::TSTMP_SW;

    // query (fixed)
    m_custom_description.query.enabled = false;

    // dynamic scenario (fixed)
    m_custom_description.dynamic_scenario.enabled = false;

    m_results.custom.timestamps.clear();
    m_results.ethtool_statistic_start = metrics::get_ethtool_statistic(m_description.interface.server);
    m_results.ip_statistic_start      = metrics::get_ip_statistic(m_description.interface.server);
    m_results.netstat_statistic_start = metrics::get_netstat_statistic();

    m_thread_ptr = std::unique_ptr<std::thread>(new std::thread(custom_tester_thread_server, m_custom_description, &(m_results.custom)));
}

test_results iperf_server::get_results() {
    if(m_thread_ptr == nullptr) {
        throw std::runtime_error("[iperf_server] E02 - iPerf control thread does not exist. Cannot get results.");
    }

    m_results.ethtool_statistic_end = metrics::get_ethtool_statistic(m_description.interface.server);
    m_results.ip_statistic_end      = metrics::get_ip_statistic(m_description.interface.server);
    m_results.netstat_statistic_end = metrics::get_netstat_statistic();

    m_thread_ptr->join();

    m_results.status = test_results::STATUS_SUCCESS;
    return m_results;
}


void custom_tester_thread_client(custom_tester_description t_description, struct test_results::custom* results) {
    custom_tester_client m_test(t_description);
    m_test.run(results);
}

void custom_tester_thread_server(custom_tester_description t_description, struct test_results::custom* results) {
    custom_tester_server m_test(t_description);
    m_test.run(results);
}
