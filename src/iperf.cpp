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

void iperf_thread(std::string t_command, std::string& t_output);
void custom_tester_thread_client(custom_tester_client_description t_description, struct test_results::custom* results);
void custom_tester_thread_server(custom_tester_server_description t_description, struct test_results::custom* results);

void tokenize(std::string const &str, const char delim, std::vector<std::string> &out);



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
 * @param udp         Enables testing with udp [iperf only]. Default value is "true".
 */
iperf_client::iperf_client(test_description description, bool udp) {
    m_description = description;

    switch(m_description.metadata.method) {
    case test_description::metadata::IPERF: {
        m_iperf_command  = "iperf ";
        if(udp)
            m_iperf_command += "-u ";
        m_iperf_command += "-c " + std::string(m_description.connection.iperf.server_ip) + " ";
        m_iperf_command += "-t " + std::to_string(m_description.duration) + " ";
        m_iperf_command += "-b " + std::string(m_description.connection.iperf.bandwidth_limit) + " ";
        m_iperf_command += "-l " + std::to_string(m_description.connection.iperf.datagram.size) + " ";
        break;
    }
    case test_description::metadata::CUSTOM: {
        m_custom_description.client_ip = m_description.connection.custom.client_ip;
        m_custom_description.server_ip = m_description.connection.custom.server_ip;
        m_custom_description.port = m_description.connection.custom.port;
        m_custom_description.gap = m_description.connection.custom.gap;
        m_custom_description.datagram.size = m_description.connection.custom.datagram.size;
        m_custom_description.datagram.random = m_description.connection.custom.datagram.random;
        m_custom_description.duration = m_description.duration;
        m_custom_description.qos = m_description.connection.custom.qos;
        break;
    }
    }
}

void iperf_client::start() {
    m_results.ethtool_statistic_start = metrics::get_ethtool_statistic(m_description.interface.client);
    m_results.ip_statistic_start      = metrics::get_ip_statistic(m_description.interface.client);
    m_results.netstat_statistic_start = metrics::get_netstat_statistic();

    switch (m_description.metadata.method) {
    case test_description::metadata::IPERF:
        m_thread_ptr = std::unique_ptr<std::thread>(new std::thread(iperf_thread, m_iperf_command, std::ref(m_iperf_output)));
        break;
    case test_description::metadata::CUSTOM:
        m_thread_ptr = std::unique_ptr<std::thread>(new std::thread(custom_tester_thread_client, m_custom_description, &(m_results.custom)));
        break;
    }
}

test_results iperf_client::get_results() {
    if(m_thread_ptr == nullptr) {
        throw std::runtime_error("[iperf_client] E01 - iPerf/custom_tester control thread does not exist. Cannot get results.");
    }
    m_thread_ptr->join();

    m_results.ethtool_statistic_end = metrics::get_ethtool_statistic(m_description.interface.client);
    m_results.ip_statistic_end      = metrics::get_ip_statistic(m_description.interface.client);
    m_results.netstat_statistic_end = metrics::get_netstat_statistic();

    switch (m_description.metadata.method) {
    case test_description::metadata::IPERF:
        if(m_iperf_output.find("Client connecting") != std::string::npos) {
            std::size_t pos_start = m_iperf_output.find('\n') + 1;
            std::size_t pos_end = m_iperf_output.find('-', pos_start) - 1;

            m_results.iperf.startup_message = m_iperf_output.substr(pos_start, pos_end-pos_start);
        }
        else {
            m_results.status = test_results::status::STATUS_FAIL;
            return m_results;
        }

        if(m_iperf_output.find("Connection refused") != std::string::npos) {
            m_results.status = test_results::status::STATUS_FAIL;
            return m_results;
        }

        if(m_iperf_output.find("Server Report") != std::string::npos) {
            m_results.status = test_results::status::STATUS_SUCCESS;

            m_iperf_output.pop_back();
            std::string summary_line = m_iperf_output.substr(m_iperf_output.rfind('\n')+1);

            std::vector<std::string> summary_components;
            tokenize(summary_line, ' ', summary_components);
            summary_components.erase(summary_components.begin(), summary_components.begin() + 2);

            m_results.iperf.interval  = summary_components.at(0) + ' ' + summary_components.at(1);
            m_results.iperf.transfer  = summary_components.at(2) + ' ' + summary_components.at(3);
            m_results.iperf.bandwidth = summary_components.at(4) + ' ' + summary_components.at(5);
            m_results.iperf.jitter    = summary_components.at(6) + ' ' + summary_components.at(7);

            m_results.iperf.num_loss  = std::stoul(summary_components.at(8).substr(0, summary_components.at(8).find("/")));
            m_results.iperf.num_total = std::stoul(summary_components.at(8).substr(summary_components.at(8).find("/")+1));
        }
        break;
    case test_description::metadata::CUSTOM:
        m_results.status = test_results::STATUS_SUCCESS;
        break;
    }

    return m_results;
}



//
//         ___ ___ _ ___ _____ _ _
//        (_-</ -_) '_\ V / -_) '_|
//  iperf_/__/\___|_|  \_/\___|_|
//

iperf_server::iperf_server(enum test_description::metadata::method method, int datagramsize, bool udp)
{
    m_description.metadata.method = method;


    switch(m_description.metadata.method) {
    case test_description::metadata::IPERF: {
        m_iperf_command  = "iperf -s -w 50M";
        if(udp)
            m_iperf_command += "-u ";
        m_iperf_command += "-l " + std::to_string(datagramsize) + " ";
        break;
    }
    case test_description::metadata::CUSTOM: {
        break;
    }
    }
}

iperf_server::~iperf_server() {
    system("killall iperf");
}

void iperf_server::start() {
    switch (m_description.metadata.method) {
    case test_description::metadata::IPERF: {
        m_thread_ptr = std::unique_ptr<std::thread>(new std::thread(iperf_thread, m_iperf_command, std::ref(m_iperf_output)));
        m_started = true;
        break;
    }
    case test_description::metadata::CUSTOM: {
        break;
    }
    }
}

void iperf_server::load_test(test_description description) {
    m_description = description;

    m_results.ethtool_statistic_start = metrics::get_ethtool_statistic(m_description.interface.server);
    m_results.ip_statistic_start      = metrics::get_ip_statistic(m_description.interface.server);
    m_results.netstat_statistic_start = metrics::get_netstat_statistic();

    switch (m_description.metadata.method) {
    case test_description::metadata::IPERF: {
        m_iperf_output = "";

        if (!m_started) {
            throw std::runtime_error("[iperf_server] E01 -  iPerf server was not started.");
        }
        break;
    }
    case test_description::metadata::CUSTOM: {
        m_custom_description.client_ip = m_description.connection.custom.client_ip;
        m_custom_description.server_ip = m_description.connection.custom.server_ip;
        m_custom_description.port      = m_description.connection.custom.port;
        m_custom_description.datagram.size = m_description.connection.custom.datagram.size;
        m_custom_description.qos       = m_description.connection.custom.qos;

        m_thread_ptr = std::unique_ptr<std::thread>(new std::thread(custom_tester_thread_server, m_custom_description, &(m_results.custom)));
        break;
    }
    }
}

test_results iperf_server::get_results() {
    if(m_thread_ptr == nullptr) {
        throw std::runtime_error("[iperf_server] E02 - iPerf control thread does not exist. Cannot get results.");
    }

    m_results.ethtool_statistic_end = metrics::get_ethtool_statistic(m_description.interface.server);
    m_results.ip_statistic_end      = metrics::get_ip_statistic(m_description.interface.server);
    m_results.netstat_statistic_end = metrics::get_netstat_statistic();

    switch (m_description.metadata.method) {
    case test_description::metadata::IPERF: {
        if(m_iperf_output.find("connected with") != std::string::npos) {
            m_results.status = test_results::status::STATUS_SUCCESS;

            m_results.iperf.startup_message = m_iperf_output;

            m_iperf_output.pop_back();
            std::string summary_line = m_iperf_output.substr(m_iperf_output.rfind('\n')+1);

            std::vector<std::string> summary_components;
            tokenize(summary_line, ' ', summary_components);
            summary_components.erase(summary_components.begin(), summary_components.begin() + 2);

            m_results.iperf.interval  = summary_components.at(0) + ' ' + summary_components.at(1);
            m_results.iperf.transfer  = summary_components.at(2) + ' ' + summary_components.at(3);
            m_results.iperf.bandwidth = summary_components.at(4) + ' ' + summary_components.at(5);
            m_results.iperf.jitter    = summary_components.at(6) + ' ' + summary_components.at(7);

            m_results.iperf.num_loss  = std::stoul(summary_components.at(8).substr(0, summary_components.at(8).find("/")));
            m_results.iperf.num_total = std::stoul(summary_components.at(8).substr(summary_components.at(8).find("/")+1));
        }
        else {
            m_results.status = test_results::status::STATUS_FAIL;
        }
        break;
    }
    case test_description::metadata::CUSTOM:
        m_thread_ptr->join();

        m_results.status = test_results::STATUS_SUCCESS;
        break;
    }

    return m_results;
}


void iperf_thread(std::string t_command, std::string& t_output) {
    std::array<char, 128> buffer;

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(t_command.c_str(), "r"), pclose);
    if(!pipe) {
        throw std::runtime_error("[iperf/ctrl_thread] E01 - popen() failed.");
    }

    while(fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        t_output += buffer.data();
    }
}

void custom_tester_thread_client(custom_tester_client_description t_description, struct test_results::custom* results) {
    custom_tester_client m_test(t_description);
    m_test.run(results);
}

void custom_tester_thread_server(custom_tester_server_description t_description, struct test_results::custom* results) {
    custom_tester_server m_test(t_description);
    m_test.run(results);
}

void tokenize(std::string const &str, const char delim, std::vector<std::string> &out)
{
    std::stringstream ss(str);

    std::string s;
    while(std::getline(ss, s, delim)) {
        if(!s.empty()) {
            out.push_back(s);
        }
    }
}
