#include "test_description.h"
#include <cstring>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <random>
#include "pugixml.hpp"

size_t strlcpy(char* dst, const char* src, size_t dst_len);
std::string generate_id();



//                                _                __
//                               (_)_ __  ___ _ _ / _|
//                               | | '_ \/ -_) '_|  _|
//  Test Description Builder for |_| .__/\___|_| |_|
//                                 |_|

static test_description build(enum test_description::metadata::method metadata_method, std::string metadata_path,
                              int duration,
                              std::string iperf_serverip, std::string iperf_bandwidthlimit, int iperf_datagramsize,
                              std::string interface_client, std::string interface_server,
                              enum test_description::stress::type stress_type, int stress_num, enum test_description::stress::location stress_location)
{
    test_description tdb_ret;
    system(("mkdir -p " + metadata_path).c_str());


    tdb_ret.metadata.method = metadata_method;
    std::string tdb_tuid = generate_id();
    if(tdb_tuid.length() < sizeof(tdb_ret.metadata.t_uid))
        strlcpy(tdb_ret.metadata.t_uid, tdb_tuid.c_str(), sizeof(tdb_ret.metadata.t_uid));
    else
        std::cerr << "[test_description_builder] Your value for metadata.t_uid is too long. The maximum is " + std::to_string(sizeof(tdb_ret.metadata.t_uid)) + " characters." << std::endl;
    if(metadata_path.length() < sizeof(tdb_ret.metadata.path))
        strlcpy(tdb_ret.metadata.path, metadata_path.c_str(), sizeof(tdb_ret.metadata.path));
    else
        std::cerr << "[test_description_builder] Your value for metadata.path is too long. The maximum is " + std::to_string(sizeof(tdb_ret.metadata.path)) + " characters." << std::endl;

    tdb_ret.duration = duration;

    if(iperf_serverip.length() < sizeof(tdb_ret.connection.iperf.server_ip))
        strlcpy(tdb_ret.connection.iperf.server_ip, iperf_serverip.c_str(), sizeof(tdb_ret.connection.iperf.server_ip));
    else
        std::cerr << "[test_description_builder] Your value for connection.iperf.server_ip is too long. The maximum is " + std::to_string(sizeof(tdb_ret.connection.iperf.server_ip)) + " characters." << std::endl;
    if(iperf_bandwidthlimit.length() < sizeof(tdb_ret.connection.iperf.bandwidth_limit))
        strlcpy(tdb_ret.connection.iperf.bandwidth_limit, iperf_bandwidthlimit.c_str(), sizeof(tdb_ret.connection.iperf.bandwidth_limit));
    else
        std::cerr << "[test_description_builder] Your value for connection.iperf.bandwidth_limit is too long. The maximum is " + std::to_string(sizeof(tdb_ret.connection.iperf.bandwidth_limit)) + " characters." << std::endl;
    tdb_ret.connection.iperf.datagram.size = iperf_datagramsize;

    if(interface_client.length() < sizeof(tdb_ret.interface.client))
        strlcpy(tdb_ret.interface.client, interface_client.c_str(), sizeof(tdb_ret.interface.client));
    else
        std::cerr << "[test_description_builder] Your value for interface.client is too long. The maximum is " + std::to_string(sizeof(tdb_ret.interface.client)) + " characters." << std::endl;
    if(interface_server.length() < sizeof(tdb_ret.interface.server))
        strlcpy(tdb_ret.interface.server, interface_server.c_str(), sizeof(tdb_ret.interface.server));
    else
        std::cerr << "[test_description_builder] Your value for interface.server is too long. The maximum is " + std::to_string(sizeof(tdb_ret.interface.server)) + " characters." << std::endl;

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

static test_description build(enum test_description::metadata::method metadata_method, std::string metadata_path,
                              int duration,
                              std::string custom_clientip,std::string custom_serverip, int custom_port, int custom_gap, int custom_datagramsize, bool custom_datagramrandom,
                              std::string interface_client, std::string interface_server,
                              enum test_description::stress::type stress_type, int stress_num, enum test_description::stress::location stress_location)
{
    test_description tdb_ret;
    system(("mkdir -p " + metadata_path).c_str());


    tdb_ret.metadata.method = metadata_method;
    std::string tdb_tuid = generate_id();
    if(tdb_tuid.length() < sizeof(tdb_ret.metadata.t_uid))
        strlcpy(tdb_ret.metadata.t_uid, tdb_tuid.c_str(), sizeof(tdb_ret.metadata.t_uid));
    else
        std::cerr << "[test_description_builder] Your value for metadata.t_uid is too long. The maximum is " + std::to_string(sizeof(tdb_ret.metadata.t_uid)) + " characters." << std::endl;
    if(metadata_path.length() < sizeof(tdb_ret.metadata.path))
        strlcpy(tdb_ret.metadata.path, metadata_path.c_str(), sizeof(tdb_ret.metadata.path));
    else
        std::cerr << "[test_description_builder] Your value for metadata.path is too long. The maximum is " + std::to_string(sizeof(tdb_ret.metadata.path)) + " characters." << std::endl;

    tdb_ret.duration = duration;

    if(custom_clientip.length() < sizeof(tdb_ret.connection.custom.client_ip))
        strlcpy(tdb_ret.connection.custom.client_ip, custom_clientip.c_str(), sizeof(tdb_ret.connection.custom.client_ip));
    else
        std::cerr << "[test_description_builder] Your value for connection.custom.client_ip is too long. The maximum is " + std::to_string(sizeof(tdb_ret.connection.custom.client_ip)) + " characters." << std::endl;
    if(custom_serverip.length() < sizeof(tdb_ret.connection.custom.server_ip))
        strlcpy(tdb_ret.connection.custom.server_ip, custom_serverip.c_str(), sizeof(tdb_ret.connection.custom.server_ip));
    else
        std::cerr << "[test_description_builder] Your value for connection.custom.server_ip is too long. The maximum is " + std::to_string(sizeof(tdb_ret.connection.custom.server_ip)) + " characters." << std::endl;
    tdb_ret.connection.custom.port = custom_port;
    tdb_ret.connection.custom.gap  = custom_gap;
    tdb_ret.connection.custom.datagram.size = custom_datagramsize;
    tdb_ret.connection.custom.datagram.random = true;

    if(interface_client.length() < sizeof(tdb_ret.interface.client))
        strlcpy(tdb_ret.interface.client, interface_client.c_str(), sizeof(tdb_ret.interface.client));
    else
        std::cerr << "[test_description_builder] Your value for interface.client is too long. The maximum is " + std::to_string(sizeof(tdb_ret.interface.client)) + " characters." << std::endl;
    if(interface_server.length() < sizeof(tdb_ret.interface.server))
        strlcpy(tdb_ret.interface.server, interface_server.c_str(), sizeof(tdb_ret.interface.server));
    else
        std::cerr << "[test_description_builder] Your value for interface.server is too long. The maximum is " + std::to_string(sizeof(tdb_ret.interface.server)) + " characters." << std::endl;

    tdb_ret.stress.type = stress_type;
    tdb_ret.stress.num = stress_num;
    tdb_ret.stress.location = stress_location;

    return tdb_ret;
}

struct test_description {
    struct connection {

        struct custom {
            char client_ip[16];         // IP-Addresse des Clients
            char server_ip[16];         // IP-Addresse des Servers
            int  port;                  // Port
            int  gap;                   // Zeit zwischen Nachrichten [ns]
            struct datagram {
                int  size;              // Datagramgroesse
                bool random;            // Zufaellige Datagramgroesse (im Bereich 0 bis size)
            } datagram;
        } custom;
    } connection;

    struct interface {
        char client[10];
        char server[10];
    } interface;

    struct stress {
        enum type {
            CPU_USR,
            CPU_KERNEL,
            CPU_REALTIME,
            MEMORY,
            IO,
        } type;
        int num;
        enum location {
            LOC_CLIENT,
            LOC_SERVER,
            LOC_BOTH,
        } location;
    } stress;
};

//                           __  ____  __ _
//                           \ \/ /  \/  | |
//                            >  <| |\/| | |__
//  Test Description from/to /_/\_\_|  |_|____|
//

static test_description read_from_XML() {

}

void test_description_parser::write_to_XML(test_description description) {
    pugi::xml_document doc;

    pugi::xml_node root = doc.append_child("test_description");

    pugi::xml_node metadata_node = root.append_child("metadata");
    switch(description.metadata.method) {
    case test_description::metadata::IPERF:
    {
        metadata_node.append_child("method").text() = "IPERF";
        break;
    }
    case test_description::metadata::CUSTOM:
    {
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
        datagram_node.append_child("size").text() = std::to_string(description.connection.iperf.datagram.size).c_str();
        // Finish mee HERE!!!!!!!!!!!!!!!!!!
        throw;
        break;
    }
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
