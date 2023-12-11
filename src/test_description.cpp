#include "test_description.h"
#include "test_control.h"
#include <cstring>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <random>
#include "pugixml.hpp"

std::string generate_id(const int& cycletime, const int& datagramsize);
size_t strlcpy(char* dst, const char* src, size_t dst_len);


//                                           _
//                                __ _  _ __| |_ ___ _ __
//                               / _| || (_-<  _/ _ \ '  \
//  Test Description Builder for \__|\_,_/__/\__\___/_|_|_|
//

test_description test_description_builder::build(client_description description, int datagramsize)
{
    test_description tdb_ret;
    if (system(("mkdir -p " + description.path).c_str()) < 0) {
        std::cerr << "Could not create test description path." << std::endl;
    }

    std::string tdb_tuid = generate_id(description.target_connection.cycletime, datagramsize);
    strlcpy(tdb_ret.metadata.t_uid, tdb_tuid.c_str(), sizeof(tdb_ret.metadata.t_uid));
    strlcpy(tdb_ret.metadata.path, description.path.c_str(), sizeof(tdb_ret.metadata.path));

    tdb_ret.duration = description.duration;

    tdb_ret.connection.type = description.target_connection.type;
    strlcpy(tdb_ret.connection.client_ip, description.target_connection.client_ip.c_str(), sizeof(tdb_ret.connection.client_ip));
    strlcpy(tdb_ret.connection.server_ip, description.target_connection.server_ip.c_str(), sizeof(tdb_ret.connection.server_ip));
    tdb_ret.connection.port = description.target_connection.port;
    tdb_ret.connection.cycletime = description.target_connection.cycletime;
    tdb_ret.connection.datagram_size = datagramsize;

    strlcpy(tdb_ret.interface.client, description.interface.client.c_str(), sizeof(tdb_ret.interface.client));
    strlcpy(tdb_ret.interface.server, description.interface.server.c_str(), sizeof(tdb_ret.interface.server));

    tdb_ret.stress.type = description.stress.type;
    tdb_ret.stress.num = description.stress.num;
    tdb_ret.stress.location = description.stress.location;

    tdb_ret.latency_measurement = description.latency_measurement;

    return tdb_ret;
}


//                      __  ____  __ _
//                      \ \/ /  \/  | |
//                       >  <| |\/| | |__
//  Test Description to /_/\_\_|  |_|____|
//

void test_description_parser::write_to_XML(std::string filename, test_description &description) {
    pugi::xml_document doc;

    pugi::xml_node root = doc.append_child("test_description");

    pugi::xml_node metadata_node = root.append_child("metadata");
    metadata_node.append_child("t_uid").text() = description.metadata.t_uid;
    metadata_node.append_child("path").text() = description.metadata.path;

    root.append_child("duration").text() = std::to_string(description.duration).c_str();

    pugi::xml_node connection_node = root.append_child("connection");
    switch(description.connection.type) {
    case uce::ST_UDP: {
        connection_node.append_child("type").text() = "ST_UDP";
        break;
    }
    case uce::ST_RAW: {
        connection_node.append_child("type").text() = "ST_RAW";
        break;
    }
    case uce::ST_PACKET: {
        connection_node.append_child("type").text() = "ST_PACKET";
        break;
    }
    }
    connection_node.append_child("client_ip").text() = description.connection.client_ip;
    connection_node.append_child("server_ip").text() = description.connection.server_ip;
    connection_node.append_child("port").text() = std::to_string(description.connection.port).c_str();
    connection_node.append_child("cycletime").text() = std::to_string(description.connection.cycletime).c_str();
    connection_node.append_child("datagram_size").text() = std::to_string(description.connection.datagram_size).c_str();

    pugi::xml_node interface_node = root.append_child("interface");
    interface_node.append_child("client").text() = description.interface.client;
    interface_node.append_child("server").text() = description.interface.server;

    pugi::xml_node stress_node = root.append_child("stress");
    switch(description.stress.type) {
    case stress_type::CPU_USR: {
        stress_node.append_child("type").text() = "CPU_USR";
        break;
    }
    case stress_type::CPU_KERNEL: {
        stress_node.append_child("type").text() = "CPU_KERNEL";
        break;
    }
    case stress_type::CPU_REALTIME: {
        stress_node.append_child("type").text() = "CPU_REALTIME";
        break;
    }
    case stress_type::MEMORY: {
        stress_node.append_child("type").text() = "MEMORY";
        break;
    }
    case stress_type::IO: {
        stress_node.append_child("type").text() = "IO";
        break;
    }
    case stress_type::TIMER: {
        stress_node.append_child("type").text() = "TIMER";
        break;
    }
    case stress_type::NONE: {
        stress_node.append_child("type").text() = "NONE";
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

    switch(description.latency_measurement) {
    case test_description::latency_measurement::DISABLED: {
        metadata_node.append_child("latency_measurement").text() = "DISABLED";
        break;
    }
    case test_description::latency_measurement::END_TO_END: {
        metadata_node.append_child("latency_measurement").text() = "END_TO_END";
        break;
    }
    case test_description::latency_measurement::FULL: {
        metadata_node.append_child("latency_measurement").text() = "FULL";
        break;
    }
    }


    if(!doc.save_file(filename.c_str())) {
        throw std::runtime_error("[td_parser] E02 - Error while saving XML file.");
    }
}



std::string generate_id(const int& cycletime, const int& datagramsize) {
    std::string generated_tuid;

//    std::mt19937 mt(time(0));
//    std::uniform_int_distribution<int> dist(100000, 999999);
//    generated_tuid += std::to_string(dist(mt)) + "_";

    std::time_t current_time = std::time(nullptr);
    std::tm* time_info = std::localtime(&current_time);

    std::ostringstream formatted_time;
    formatted_time << std::setfill('0');
    formatted_time << std::setw(2) << time_info->tm_hour << std::setw(2) << time_info->tm_min << std::setw(2) << time_info->tm_sec << '_'
                   << std::setw(2) << time_info->tm_mday << std::setw(2) << (time_info->tm_mon + 1) << std::setw(2) << (time_info->tm_year % 100);
    generated_tuid += formatted_time.str();

    generated_tuid += "_" + std::to_string(cycletime) + "_" + std::to_string(datagramsize);

    return generated_tuid;
}

size_t strlcpy(char* dst, const char* src, size_t dst_len) {
    size_t src_len = strlen(src);
    size_t copy_len = (src_len < dst_len) ? src_len : (dst_len - 1);
    if (dst_len > 0) {
        memcpy(dst, src, copy_len);
        dst[copy_len] = '\0';
    }
    return copy_len;
}
