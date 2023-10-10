#include "test_description.h"
#include "test_control.h"
#include <cstring>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <random>
#include "pugixml.hpp"

std::string generate_id();
size_t strlcpy(char* dst, const char* src, size_t dst_len);



test_description test_description_builder::simple_build(client_description description, int duration, int datagramsize, int stress_num) {
    test_description tdb_ret;

    switch(description.method) {
    case test_description::metadata::IPERF: {
        tdb_ret = test_description_builder::build(description.path,
                                                  duration,
                                                  description.target_connection.server_ip, description.target_connection.bandwidth_limit, datagramsize,
                                                  description.interface.client, description.interface.server,
                                                  description.stress.type, stress_num, description.stress.location);
        break;
    }
    case test_description::metadata::CUSTOM: {
        tdb_ret = test_description_builder::build(description.path,
                                                  duration,
                                                  description.target_connection.client_ip, description.target_connection.server_ip, description.target_connection.port, description.target_connection.gap, datagramsize, description.target_connection.datagram.random,
                                                  description.interface.client, description.interface.server,
                                                  description.stress.type, stress_num, description.stress.location);
        break;
    }
    }

    return tdb_ret;
}



//                                _                __
//                               (_)_ __  ___ _ _ / _|
//                               | | '_ \/ -_) '_|  _|
//  Test Description Builder for |_| .__/\___|_| |_|
//                                 |_|

test_description test_description_builder::build(std::string metadata_path,
                                                 int duration,
                                                 std::string iperf_serverip, std::string iperf_bandwidthlimit, int iperf_datagramsize,
                                                 std::string interface_client, std::string interface_server,
                                                 enum test_description::stress::type stress_type, int stress_num, enum test_description::stress::location stress_location)
{
    test_description tdb_ret;
    system(("mkdir -p " + metadata_path).c_str());


    tdb_ret.metadata.method = test_description::metadata::IPERF;
    std::string tdb_tuid = generate_id();
    strlcpy(tdb_ret.metadata.t_uid, tdb_tuid.c_str(), sizeof(tdb_ret.metadata.t_uid));
    strlcpy(tdb_ret.metadata.path, metadata_path.c_str(), sizeof(tdb_ret.metadata.path));

    tdb_ret.duration = duration;

    strlcpy(tdb_ret.connection.iperf.server_ip, iperf_serverip.c_str(), sizeof(tdb_ret.connection.iperf.server_ip));
    strlcpy(tdb_ret.connection.iperf.bandwidth_limit, iperf_bandwidthlimit.c_str(), sizeof(tdb_ret.connection.iperf.bandwidth_limit));
    tdb_ret.connection.iperf.datagram.size = iperf_datagramsize;

    strlcpy(tdb_ret.interface.client, interface_client.c_str(), sizeof(tdb_ret.interface.client));
    strlcpy(tdb_ret.interface.server, interface_server.c_str(), sizeof(tdb_ret.interface.server));

    tdb_ret.stress.type = stress_type;
    tdb_ret.stress.num = stress_num;
    tdb_ret.stress.location = stress_location;

    return tdb_ret;
}



//                                           _
//                                __ _  _ __| |_ ___ _ __
//                               / _| || (_-<  _/ _ \ '  \
//  Test Description Builder for \__|\_,_/__/\__\___/_|_|_|
//

test_description test_description_builder::build(std::string metadata_path,
                                                 int duration,
                                                 std::string custom_clientip,std::string custom_serverip, int custom_port, int custom_gap, int custom_datagramsize, bool custom_datagramrandom,
                                                 std::string interface_client, std::string interface_server,
                                                 enum test_description::stress::type stress_type, int stress_num, enum test_description::stress::location stress_location)
{
    test_description tdb_ret;
    system(("mkdir -p " + metadata_path).c_str());


    tdb_ret.metadata.method = test_description::metadata::CUSTOM;
    std::string tdb_tuid = generate_id();
    strlcpy(tdb_ret.metadata.t_uid, tdb_tuid.c_str(), sizeof(tdb_ret.metadata.t_uid));
    strlcpy(tdb_ret.metadata.path, metadata_path.c_str(), sizeof(tdb_ret.metadata.path));

    tdb_ret.duration = duration;

    strlcpy(tdb_ret.connection.custom.client_ip, custom_clientip.c_str(), sizeof(tdb_ret.connection.custom.client_ip));
    strlcpy(tdb_ret.connection.custom.server_ip, custom_serverip.c_str(), sizeof(tdb_ret.connection.custom.server_ip));
    tdb_ret.connection.custom.port = custom_port;
    tdb_ret.connection.custom.gap  = custom_gap;
    tdb_ret.connection.custom.datagram.size = custom_datagramsize;
    tdb_ret.connection.custom.datagram.random = custom_datagramrandom;

    strlcpy(tdb_ret.interface.client, interface_client.c_str(), sizeof(tdb_ret.interface.client));
    strlcpy(tdb_ret.interface.server, interface_server.c_str(), sizeof(tdb_ret.interface.server));

    tdb_ret.stress.type = stress_type;
    tdb_ret.stress.num = stress_num;
    tdb_ret.stress.location = stress_location;

    return tdb_ret;
}



//                           __  ____  __ _
//                           \ \/ /  \/  | |
//                            >  <| |\/| | |__
//  Test Description from/to /_/\_\_|  |_|____|
//

static test_description read_from_XML(std::string filename) {
    pugi::xml_document doc;
    if(!doc.load_file(filename.c_str())) {
        throw std::runtime_error("[td_parser] E01 - Error while loading XML file.");
    }

    test_description description;
    pugi::xml_node root = doc.child("test_description");

    pugi::xml_node metadata_node = root.child("metadata");
    std::string metadata_method = metadata_node.child_value("method");
    if(metadata_method == "IPERF") {
        description.metadata.method = test_description::metadata::IPERF;
    }
    else if(metadata_method == "CUSTOM") {
        description.metadata.method = test_description::metadata::CUSTOM;
    }
    pugi::xml_node tuid_node = metadata_node.child("t_uid");
    if(tuid_node) {
        strlcpy(description.metadata.t_uid, tuid_node.child_value(), sizeof(description.metadata.t_uid));
    }
    else {
        std::string tuid_generated = generate_id();
        strlcpy(description.metadata.t_uid, tuid_generated.c_str(), sizeof(description.metadata.t_uid));
    }
    strlcpy(description.metadata.path, metadata_node.child_value("path"), sizeof(description.metadata.path));

    description.duration = std::stoi(root.child_value("duration"));

    pugi::xml_node connection_node = root.child("connection");
    switch(description.metadata.method) {
    case test_description::metadata::IPERF: {
        pugi::xml_node iperf_node = connection_node.child("iperf");
        strlcpy(description.connection.iperf.server_ip, iperf_node.child_value("server_ip"), sizeof(description.connection.iperf.server_ip));
        strlcpy(description.connection.iperf.bandwidth_limit, iperf_node.child_value("bandwidth_limit"), sizeof(description.connection.iperf.bandwidth_limit));

        pugi::xml_node datagram_node = iperf_node.child("datagram");
        description.connection.iperf.datagram.size = std::stoi(datagram_node.child_value("size"));
        break;
    }
    case test_description::metadata::CUSTOM: {
        pugi::xml_node custom_node = connection_node.child("custom");
        strlcpy(description.connection.custom.client_ip, custom_node.child_value("client_ip"), sizeof(description.connection.custom.client_ip));
        strlcpy(description.connection.custom.server_ip, custom_node.child_value("server_ip"), sizeof(description.connection.custom.client_ip));
        description.connection.custom.port = std::stoi(custom_node.child_value("port"));
        description.connection.custom.gap = std::stoi(custom_node.child_value("gap"));

        pugi::xml_node datagram_node = custom_node.child("datagram");
        description.connection.custom.datagram.size = std::stoi(datagram_node.child_value("size"));
        description.connection.custom.datagram.random = (std::string(datagram_node.child_value("random")) == "true");
        break;
    }
    }

    pugi::xml_node interface_node = root.child("interface");
    strlcpy(description.interface.client, interface_node.child_value("client"), sizeof(description.interface.client));
    strlcpy(description.interface.server, interface_node.child_value("server"), sizeof(description.interface.server));

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

    return description;
}

void test_description_parser::write_to_XML(std::string filename, test_description &description) {
    pugi::xml_document doc;

    pugi::xml_node root = doc.append_child("test_description");

    pugi::xml_node metadata_node = root.append_child("metadata");
    switch(description.metadata.method) {
    case test_description::metadata::IPERF: {
        metadata_node.append_child("method").text() = "IPERF";
        break;
    }
    case test_description::metadata::CUSTOM: {
        metadata_node.append_child("method").text() = "CUSTOM";
        break;
    }
    }
    metadata_node.append_child("t_uid").text() = description.metadata.t_uid;
    metadata_node.append_child("path").text() = description.metadata.path;

    root.append_child("duration").text() = std::to_string(description.duration).c_str();

    pugi::xml_node connection_node = root.append_child("connection");
    switch(description.metadata.method) {
    case test_description::metadata::IPERF: {
        pugi::xml_node iperf_node = connection_node.append_child("iperf");
        iperf_node.append_child("server_ip").text() = description.connection.iperf.server_ip;
        iperf_node.append_child("bandwidth_limit").text() = description.connection.iperf.bandwidth_limit;

        pugi::xml_node datagram_node = iperf_node.append_child("datagram");
        datagram_node.append_child("size").text() = std::to_string(description.connection.iperf.datagram.size).c_str();
        break;
    }
    case test_description::metadata::CUSTOM: {
        pugi::xml_node custom_node = connection_node.append_child("custom");
        custom_node.append_child("client_ip").text() = description.connection.custom.client_ip;
        custom_node.append_child("server_ip").text() = description.connection.custom.server_ip;
        custom_node.append_child("port").text() = std::to_string(description.connection.custom.port).c_str();
        custom_node.append_child("gap").text() = std::to_string(description.connection.custom.gap).c_str();

        pugi::xml_node datagram_node = custom_node.append_child("datagram");
        datagram_node.append_child("size").text() = std::to_string(description.connection.custom.datagram.size).c_str();
        datagram_node.append_child("random").text() = description.connection.custom.datagram.random ? "true" : "false";
        break;
    }
    }

    pugi::xml_node interface_node = root.append_child("interface");
    interface_node.append_child("client").text() = description.interface.client;
    interface_node.append_child("server").text() = description.interface.server;

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


    if(!doc.save_file(filename.c_str())) {
        throw std::runtime_error("[td_parser] E02 - Error while saving XML file.");
    }
}



std::string generate_id() {
    std::string generated_tuid;

    std::mt19937 mt(time(0));
    std::uniform_int_distribution<int> dist(100000, 999999);
    generated_tuid += std::to_string(dist(mt)) + "_";

    std::time_t current_time = std::time(nullptr);
    std::tm* time_info = std::localtime(&current_time);

    std::ostringstream formatted_time;
    formatted_time << std::setfill('0');
    formatted_time << std::setw(2) << time_info->tm_hour << std::setw(2) << time_info->tm_min << std::setw(2) << time_info->tm_sec << '_'
                   << std::setw(2) << time_info->tm_mday << std::setw(2) << (time_info->tm_mon + 1) << std::setw(2) << (time_info->tm_year % 100);
    generated_tuid += formatted_time.str();

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
