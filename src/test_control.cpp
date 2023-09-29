#include "test_control.h"
#include <ctime>
#include <chrono>
#include <fstream>
#include <iostream>
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
    pugi::xml_node sizes_node = datagram_node.child("sizes");
    for(pugi::xml_node value_node = sizes_node.child("value"); value_node; value_node = value_node.next_sibling("value")) {
        description.target_connection.datagram.sizes.push_back(value_node.text().as_int());
    }
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
    else if(stress_type == "TIMER") {
        description.stress.type = test_description::stress::TIMER;
    }
    else if(stress_type == "NETWORK") {
        description.stress.type = test_description::stress::NETWORK;
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
    pugi::xml_node sizes_node = datagram_node.append_child("sizes");
    for (int sizes : description.target_connection.datagram.sizes) {
        sizes_node.append_child("value").text() = std::to_string(sizes).c_str();
    }
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
    case test_description::stress::TIMER: {
        stress_node.append_child("type").text() = "TIMER";
        break;
    }
    case test_description::stress::NETWORK: {
        stress_node.append_child("type").text() = "NETWORK";
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

std::vector<std::string> test_control_parser::read_client_main_XML(std::string filename) {
    std::vector<std::string> paths;

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename.c_str());

    if (result) {
        pugi::xml_node root = doc.child("description_files");

        if (root) {
            for (pugi::xml_node path_node = root.child("client_description"); path_node; path_node = path_node.next_sibling("client_description")) {
                std::string description_path = path_node.child_value("path");
                std::string execute          = path_node.child_value("execute");

                if("true" == execute) {
                    paths.push_back(description_path);
                }
            }
        }
    } else {
        std::cerr << "XML parsing error: " << result.description() << std::endl;
    }

    return paths;
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
    std::string filename = std::string(description.path) + "/control_log.xml";
    std::time_t timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    pugi::xml_document doc;
    pugi::xml_node root = doc.append_child("client_log");

    root.append_child("startup").text() = std::ctime(&timestamp);

    if(!doc.save_file(filename.c_str())) {
        throw std::runtime_error("[tc_logger] E01 - Error while saving XML file.");
    }
}

void test_control_logger::log_control(server_description description) {
    std::string filename = std::string(description.path) + "/control_log.xml";
    std::time_t timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    pugi::xml_document doc;
    pugi::xml_node root = doc.append_child("server_log");

    root.append_child("startup").text() = std::ctime(&timestamp);

    if(!doc.save_file(filename.c_str())) {
        throw std::runtime_error("[tc_logger] E01 - Error while saving XML file.");
    }
}

void test_control_logger::log_scenario(std::string path, test_description description, test_results* results) {
    std::string filename = std::string(path) + "/control_log.xml";
    std::time_t timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    pugi::xml_document doc;
    if(!doc.load_file(filename.c_str())) {
        throw std::runtime_error("[tc_parser] E01 - Error while loading XML file (scenario).");
    }

    pugi::xml_node root = doc.child("client_log");
    if(!root) {
        root = doc.child("server_log");
    }

    pugi::xml_node scenario_node = root.append_child("scenario");
    scenario_node.append_child("t_uid").text() = description.metadata.t_uid;
    scenario_node.append_child("log_time").text() = std::ctime(&timestamp);

    switch(description.metadata.method) {
    case test_description::metadata::IPERF: {
        scenario_node.append_child("method").text() = "IPERF";
        break;
    }
    case test_description::metadata::CUSTOM: {
        scenario_node.append_child("method").text() = "CUSTOM";
        break;
    }
    }

    scenario_node.append_child("duration").text() = std::to_string(description.duration).c_str();
    scenario_node.append_child("datagram_size").text() = std::to_string(description.connection.custom.datagram.size).c_str();

    pugi::xml_node stress_node = scenario_node.append_child("stress");
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
    case test_description::stress::TIMER: {
        stress_node.append_child("type").text() = "TIMER";
        break;
    }
    case test_description::stress::NETWORK: {
        stress_node.append_child("type").text() = "NETWORK";
        break;
    }
    }
    stress_node.append_child("num").text() = std::to_string(description.stress.num).c_str();
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

    if(results != nullptr) {
        pugi::xml_node results_node = scenario_node.append_child("results");
        switch(results->status) {
        case test_results::STATUS_FAIL: {
            results_node.append_child("status").text() = "STATUS_FAIL";
            break;
        }
        case test_results::STATUS_SUCCESS: {
            results_node.append_child("status").text() = "STATUS_SUCCESS";
            break;
        }
        case test_results::STATUS_UNKNOWN: {
            results_node.append_child("status").text() = "STATUS_UNKNOWN";
            break;
        }
        }
        switch(description.metadata.method) {
        case test_description::metadata::IPERF: {
            results_node.append_child("num_loss").text() = std::to_string(results->iperf.num_loss).c_str();
            results_node.append_child("num_total").text() = std::to_string(results->iperf.num_total).c_str();
            break;
        }
        case test_description::metadata::CUSTOM: {
            results_node.append_child("num_loss").text() = std::to_string(results->custom.num_loss).c_str();
            results_node.append_child("num_total").text() = std::to_string(results->custom.num_total).c_str();
            results_node.append_child("num_misses").text() = std::to_string(results->custom.num_misses).c_str();
            break;
        }
        }
    }

    if(!doc.save_file(filename.c_str())) {
        throw std::runtime_error("[tc_logger] E01 - Error while saving XML file.");
    }
}
