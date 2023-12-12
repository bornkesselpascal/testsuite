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
    description.path = root.child_value("path");
    description.duration = std::stoi(root.child_value("duration"));

    pugi::xml_node dynamic_behavior = root.child("dynamic_behavior");
    if(dynamic_behavior) {
        std::string mode = dynamic_behavior.child_value("mode");
        if(mode == "DATAGRAM_SIZE") {
            description.dynamic_behavoir.mode = client_description::dynamic_behavoir::DATAGRAM_SIZE;
        }
        else if(mode == "CYCLE_TIME") {
            description.dynamic_behavoir.mode = client_description::dynamic_behavoir::CYCLE_TIME;
        }
        else {
            description.dynamic_behavoir.mode = client_description::dynamic_behavoir::DISABLED;
        }

        description.dynamic_behavoir.min   = std::stoi(dynamic_behavior.child_value("min"));
        description.dynamic_behavoir.max   = std::stoi(dynamic_behavior.child_value("max"));
        description.dynamic_behavoir.steps = std::stoi(dynamic_behavior.child_value("steps"));
    }
    else {
        description.dynamic_behavoir.mode = client_description::dynamic_behavoir::DISABLED;
    }

    pugi::xml_node targetconnection_node = root.child("target_connection");
    std::string connection_type = targetconnection_node.child_value("type");
    if(connection_type == "ST_UDP") {
        description.target_connection.type = uce::ST_UDP;
    }
    else if(connection_type == "ST_RAW") {
        description.target_connection.type = uce::ST_RAW;
    }
    else if(connection_type == "ST_PACKET") {
        description.target_connection.type = uce::ST_PACKET;
    }
    description.target_connection.client_ip = targetconnection_node.child_value("client_ip");
    description.target_connection.server_ip = targetconnection_node.child_value("server_ip");
    description.target_connection.port = std::stoi(targetconnection_node.child_value("port"));
    description.target_connection.cycletime = std::stoi(targetconnection_node.child_value("cycletime"));
    pugi::xml_node datagramsizes_node = targetconnection_node.child("datagram_sizes");
    for(pugi::xml_node size_node = datagramsizes_node.child("size"); size_node; size_node = size_node.next_sibling("size")) {
        description.target_connection.datagram_sizes.push_back(size_node.text().as_int());
    }

    pugi::xml_node interface_node = root.child("interface");
    description.interface.client = interface_node.child_value("client");
    description.interface.server = interface_node.child_value("server");

    pugi::xml_node serviceconnection_node = root.child("service_connection");
    description.service_connection.server_ip = serviceconnection_node.child_value("server_ip");
    description.service_connection.port = std::stoi(serviceconnection_node.child_value("port"));

    pugi::xml_node stress_node = root.child("stress");
    std::string stress_type = stress_node.child_value("type");
    if(stress_type == "CPU_USR") {
        description.stress.type = stress_type::CPU_USR;
    }
    else if(stress_type == "CPU_KERNEL") {
        description.stress.type = stress_type::CPU_KERNEL;
    }
    else if(stress_type == "CPU_REALTIME") {
        description.stress.type = stress_type::CPU_REALTIME;
    }
    else if(stress_type == "MEMORY") {
        description.stress.type = stress_type::MEMORY;
    }
    else if(stress_type == "IO") {
        description.stress.type = stress_type::IO;
    }
    else if(stress_type == "TIMER") {
        description.stress.type = stress_type::TIMER;
    }
    else if(stress_type == "NONE") {
        description.stress.type = stress_type::NONE;
    }
    description.stress.num = std::stoi(stress_node.child_value("num"));
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

    if(root.child("latency_measurement")) {
        std::string mode = root.child_value("latency_measurement");
        if(mode == "END_TO_END") {
            description.latency_measurement = test_description::latency::measurement::END_TO_END;
        }
        else if(mode == "FULL") {
            description.latency_measurement = test_description::latency::measurement::FULL;
        }
        else {
            description.latency_measurement = test_description::latency::measurement::DISABLED;
        }
    }
    else {
        description.latency_measurement = test_description::latency::measurement::DISABLED;
    }

    if(root.child("latency_reduced")) {
        std::string reduced = root.child_value("latency_reduced");
        if(reduced == "TRUE") {
            description.latency_reduced = true;
        }
        else {
            description.latency_reduced = false;
        }
    }
    else {
        description.latency_reduced = false;
    }

    return description;
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

std::vector<std::string> test_control_parser::read_main_XML(std::string filename, testsuite_type type) {
    std::vector<std::string> paths;

    std::string description_entry_name;
    switch(type)
    {
    case testsuite_type::CLIENT:
        description_entry_name = "client_description";
        break;
    case testsuite_type::SERVER:
        description_entry_name = "server_description";
        break;
    }

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filename.c_str());

    if (result) {
        pugi::xml_node root = doc.child("description_files");

        if (root) {
            for (pugi::xml_node path_node = root.child(description_entry_name.c_str()); path_node; path_node = path_node.next_sibling(description_entry_name.c_str())) {
                std::string description_path = path_node.child_value("path");
                paths.push_back(description_path);
            }
        }
    } else {
        std::cerr << "XML parsing error: " << result.description() << std::endl;
    }

    return paths;
}
