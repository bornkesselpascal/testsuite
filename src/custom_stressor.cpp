#include "custom_stressor.h"
#include "pugixml.hpp"
#include <atomic>
#include <iostream>
#include <unistd.h>

void iperf_thread(std::string t_command);

custom_stressor_network::custom_stressor_network(std::string path, int duration, int num, stress_location location)
    : m_duration(duration)
{
    if(!read_from_XML(path)) {
        std::cout << "[cs_network] Error when parsing custom_stressor_description." << std::endl;
    }

    if(location == stress_location::LOC_BOTH) {
        std::cout << "[cs_network] This location specifier is not supported." << std::endl;
    }

    m_iperf_command  = "iperf -u ";
    m_iperf_command += "-l " + std::to_string(m_description.datagram.size) + " ";
    if(!(m_server != (location == stress_location::LOC_CLIENT))) {
        m_iperf_command += "-s -w 50M ";
    }
    else {
        m_iperf_command += "-c " + m_description.companion_ip + " ";;
        m_iperf_command += "-t " + std::to_string(m_duration) + " ";
        m_iperf_command += "-b " + std::to_string(num) + "G ";
    }
}

void custom_stressor_network::start() {
    m_thread_ptr = std::unique_ptr<std::thread>(new std::thread(iperf_thread, m_iperf_command));
}

void custom_stressor_network::stop() {
    system("killall iperf");
    m_thread_ptr.release();
}

bool custom_stressor_network::read_from_XML(std::string path) {
    pugi::xml_document doc;
    if(!doc.load_file(path.c_str())) {
        return false;
    }

    test_description description;
    pugi::xml_node root = doc.child("network_stress_description");

    pugi::xml_node connection_node = root.child("connection");
    m_description.companion_ip = connection_node.child_value("companion_ip");

    pugi::xml_node datagram_node = connection_node.child("datagram");
    m_description.datagram.size = std::stoi(datagram_node.child_value("size"));

    m_server = (std::string(root.child_value("is_server")) == "true");

    return true;
}



void iperf_thread(std::string t_command) {
    system(t_command.c_str());
}
