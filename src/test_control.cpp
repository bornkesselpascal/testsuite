#include "test_control.h"
#include <ctime>
#include <chrono>
#include <fstream>
#include "pugixml.hpp"


//                       __  ____  __ _
//                       \ \/ /  \/  | |
//                        >  <| |\/| | |__
//  Test Control from/to /_/\_\_|  |_|____|
//

client_description test_control_parser::read_client_from_XML(std::string filename) {
    pugi::xml_document doc;
    if(!doc.load_file(filename.c_str())) {
        throw std::runtime_error("[tc_parser] E01 - Error while loading XML file.");
    }

    client_description description;
    pugi::xml_node root = doc.child("client_description");

    std::string metadata_method = root.child_value("method");
    if(metadata_method == "IPERF") {
        description.method = test_description::metadata::IPERF;
    }
    else if(metadata_method == "CUSTOM") {
        description.method = test_description::metadata::CUSTOM;
    }
    description.path = root.child_value("path");

    pugi::xml_node duration_node = root.child("duration");
    description.duration.short_duration = std::stoi(duration_node.child_value("short_duration"));
    description.duration.long_duration = std::stoi(duration_node.child_value("long_duration"));

    pugi::xml_node targetconnection_node = root.child("target_connection");
    description.target_connection.client_ip = targetconnection_node.child_value("client_ip");
    description.target_connection.server_ip = targetconnection_node.child_value("server_ip");
    description.target_connection.bandwidth_limit = targetconnection_node.child_value("bandwidth_limit");
    description.target_connection.port = std::stoi(targetconnection_node.child_value("port"));
    description.target_connection.gap = std::stoi(targetconnection_node.child_value("gap"));
    pugi::xml_node datagram_node = targetconnection_node.child("datagram");
    description.target_connection.datagram.size_max = std::stoi(datagram_node.child_value("size_max"));
    description.target_connection.datagram.steps = std::stoi(datagram_node.child_value("steps"));
    description.target_connection.datagram.random = (std::string(datagram_node.child_value("random")) == "true");

    pugi::xml_node interface_node = root.child("interface");
    description.interface.client = interface_node.child_value("client");
    description.interface.server = interface_node.child_value("server");

    pugi::xml_node serviceconnection_node = root.child("service_connection");
    description.service_connection.server_ip = serviceconnection_node.child_value("server_ip");
    description.service_connection.port = std::stoi(serviceconnection_node.child_value("port"));
    description.client_only = (std::string(root.child_value("client_only")) == "true");

    pugi::xml_node stress_node = root.child("stress");
    std::string stress_type = stress_node.child_value("type");
    if(stress_type == "CPU_USR") {
        description.stress.type = test_description::stress::CPU_USR;
    }
    else if(stress_type == "CPU_KERNEL") {
        description.stress.type = test_description::stress::CPU_KERNEL;
    }
    else if(stress_type == "CPU_REALTIME") {
        description.stress.type = test_description::stress::CPU_REALTIME;
    }
    else if(stress_type == "MEMORY") {
        description.stress.type = test_description::stress::MEMORY;
    }
    else if(stress_type == "IO") {
        description.stress.type = test_description::stress::IO;
    }
    pugi::xml_node num_node = stress_node.child("num");
    description.stress.num.num_min = std::stoi(num_node.child_value("num_min"));
    description.stress.num.num_max = std::stoi(num_node.child_value("num_max"));
    description.stress.num.steps = std::stoi(num_node.child_value("steps"));
    std::string stress_location = stress_node.child_value("location");
    if(stress_location == "LOC_CLIENT") {
        description.stress.location = test_description::stress::LOC_CLIENT;
    }
    else if(stress_location == "LOC_SERVER") {
        description.stress.location = test_description::stress::LOC_SERVER;
    }
    else if(stress_location == "LOC_BOTH") {
        description.stress.location = test_description::stress::LOC_BOTH;
    }

    return description;
}

void test_control_parser::write_client_to_XML(std::string filename, client_description &description) {
    pugi::xml_document doc;

    pugi::xml_node root = doc.append_child("client_description");

    switch(description.method) {
    case test_description::metadata::IPERF: {
        root.append_child("method").text() = "IPERF";
        break;
    }
    case test_description::metadata::CUSTOM: {
        root.append_child("method").text() = "CUSTOM";
        break;
    }
    }
    root.append_child("path").text() = description.path.c_str();

    pugi::xml_node duration_node = root.append_child("duration");
    duration_node.append_child("short_duration").text() = std::to_string(description.duration.short_duration).c_str();
    duration_node.append_child("long_duration").text() = std::to_string(description.duration.long_duration).c_str();

    pugi::xml_node targetconnection_node = root.append_child("target_connection");
    targetconnection_node.append_child("client_ip").text() = description.target_connection.client_ip.c_str();
    targetconnection_node.append_child("server_ip").text() = description.target_connection.server_ip.c_str();
    targetconnection_node.append_child("bandwidth_limit").text() = description.target_connection.bandwidth_limit.c_str();
    targetconnection_node.append_child("port").text() = std::to_string(description.target_connection.port).c_str();
    targetconnection_node.append_child("gap").text() = std::to_string(description.target_connection.gap).c_str();
    pugi::xml_node datagram_node = targetconnection_node.append_child("datagram");
    datagram_node.append_child("size_max").text() = std::to_string(description.target_connection.datagram.size_max).c_str();
    datagram_node.append_child("steps").text() = std::to_string(description.target_connection.datagram.steps).c_str();
    datagram_node.append_child("random").text() = description.target_connection.datagram.random ? "true" : "false";

    pugi::xml_node interface_node = root.append_child("interface");
    interface_node.append_child("client").text() = description.interface.client.c_str();
    interface_node.append_child("server").text() = description.interface.server.c_str();

    pugi::xml_node serviceconnection_node = root.append_child("service_connection");
    serviceconnection_node.append_child("server_ip").text() = description.service_connection.server_ip.c_str();
    serviceconnection_node.append_child("port").text() = std::to_string(description.service_connection.port).c_str();
    root.append_child("client_only").text() = description.client_only ? "true" : "false";

    pugi::xml_node stress_node = root.append_child("stress");
    switch(description.stress.type) {
    case test_description::stress::CPU_USR: {
        stress_node.append_child("type").text() = "CPU_USR";
        break;
    }
    case test_description::stress::CPU_KERNEL: {
        stress_node.append_child("type").text() = "CPU_KERNEL";
        break;
    }
    case test_description::stress::CPU_REALTIME: {
        stress_node.append_child("type").text() = "CPU_REALTIME";
        break;
    }
    case test_description::stress::MEMORY: {
        stress_node.append_child("type").text() = "MEMORY";
        break;
    }
    case test_description::stress::IO: {
        stress_node.append_child("type").text() = "IO";
        break;
    }
    }
    pugi::xml_node num_node = stress_node.append_child("num");
    num_node.append_child("num_min").text() = std::to_string(description.stress.num.num_min).c_str();
    num_node.append_child("num_max").text() = std::to_string(description.stress.num.num_max).c_str();
    num_node.append_child("steps").text() = std::to_string(description.stress.num.steps).c_str();
    switch(description.stress.location) {
    case test_description::stress::LOC_CLIENT: {
        stress_node.append_child("location").text() = "LOC_CLIENT";
        break;
    }
    case test_description::stress::LOC_SERVER: {
        stress_node.append_child("location").text() = "LOC_SERVER";
        break;
    }
    case test_description::stress::LOC_BOTH: {
        stress_node.append_child("location").text() = "LOC_BOTH";
        break;
    }
    }


    if(!doc.save_file(filename.c_str())) {
        throw std::runtime_error("[tc_parser] E02 - Error while saving XML file.");
    }
}

server_description test_control_parser::read_server_from_XML(std::string filename) {
    pugi::xml_document doc;
    if(!doc.load_file(filename.c_str())) {
        throw std::runtime_error("[tc_parser] E01 - Error while loading XML file.");
    }

    server_description description;
    pugi::xml_node root = doc.child("server_description");

    description.path = root.child_value("path");

    pugi::xml_node serviceconnection_node = root.child("service_connection");
    description.service_connection.server_ip = serviceconnection_node.child_value("server_ip");
    description.service_connection.port = std::stoi(serviceconnection_node.child_value("port"));

    return description;
}

void test_control_parser::write_server_to_XML(std::string filename, server_description &description) {
    pugi::xml_document doc;

    pugi::xml_node root = doc.append_child("server_description");

    root.append_child("path").text() = description.path.c_str();

    pugi::xml_node serviceconnection_node = root.append_child("service_connection");
    serviceconnection_node.append_child("server_ip").text() = description.service_connection.server_ip.c_str();
    serviceconnection_node.append_child("port").text() = std::to_string(description.service_connection.port).c_str();


    if(!doc.save_file(filename.c_str())) {
        throw std::runtime_error("[tc_parser] E02 - Error while saving XML file.");
    }
}

void test_control_logger::log_control(client_description description) {
    std::time_t timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string filepath = std::string(description.path) + "/control_log.txt";
    std::ofstream filestream(filepath);

    if(filestream.is_open()) {
        filestream << "[IM] TYPE               : " << "TEST CONTROL CLIENT LOG" << std::endl;
        filestream << "[IM] STARTUP            : " << std::ctime(&timestamp);
        filestream << std::endl << std::endl;
    }
    filestream.close();
}

void test_control_logger::log_control(server_description description) {
    std::time_t timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string filepath = std::string(description.path) + "/control_log.txt";
    std::ofstream filestream(filepath);

    if(filestream.is_open()) {
        filestream << "[IM] TYPE               : " << "TEST CONTROL SERVER LOG" << std::endl;
        filestream << "[IM] STARTUP            : " << std::ctime(&timestamp);
        filestream << std::endl << std::endl << std::endl;
    }
    filestream.close();
}

void test_control_logger::log_description(test_description description, test_results* results) {
    std::time_t timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string filepath = std::string(description.metadata.path) + "/control_log.txt";
    std::ofstream filestream(filepath, std::ios_base::app);

    if(filestream.is_open()) {
        filestream << "[TC] DESCRIPTION T_UID  : " << description.metadata.t_uid << std::endl;
        filestream << "[TC] END TIME           : " << std::ctime(&timestamp);

        filestream << "[TD] STRESS (TYPE/NUM)  : " << description.stress.type << "/" << description.stress.num << std::endl;
        filestream << "[TD] DURATION           : " << description.duration << std::endl;

        if(results != nullptr) {
            switch(description.metadata.method) {
            case test_description::metadata::IPERF:
                filestream << "[RE] STATUS             : " << results->status << std::endl;
                filestream << "[RE] RESULT (LOSS/TOTAL): " << results->iperf.num_loss << "/" << results->iperf.num_total << std::endl;
                break;

            case test_description::metadata::CUSTOM:
                filestream << "[RE] STATUS             : " << results->status << std::endl;
                filestream << "[RE] RESULT (LOSS/TOAL) : " << results->custom.num_loss << "/" << results->custom.num_total << std::endl;
                break;
            }
        }
        filestream << std::endl << std::endl << std::endl;
    }
    filestream.close();
}
