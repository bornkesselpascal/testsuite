#include "test_description.h"
#include <cstring>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <random>

size_t strlcpy(char* dst, const char* src, size_t dst_len);


test_description test_description_builder::build(enum test_description::metadata::method metadata_method, std::string metadata_tuid, std::string metadata_path,
                                                 int duration,
                                                 std::string connection_ip, std::string connection_bandwidth, int connection_datagramsize,
                                                 std::string interface_client, std::string interface_server,
                                                 enum test_description::stress::type stress_type, int stress_num, enum test_description::stress::location stress_location)
{
    test_description tdb_ret;
    system(("mkdir -p " + metadata_path).c_str());

    tdb_ret.metadata.method = metadata_method;
    if(metadata_tuid.length() < sizeof(tdb_ret.metadata.t_uid))
        strlcpy(tdb_ret.metadata.t_uid, metadata_tuid.c_str(), sizeof(tdb_ret.metadata.t_uid));
    else
        std::cerr << "[test_description_builder] Your value for metadata.path ist to long. The maximum is " + std::to_string(sizeof(tdb_ret.metadata.path)) + " characters." << std::endl;

    if(metadata_path.length() < sizeof(tdb_ret.metadata.path))
        strlcpy(tdb_ret.metadata.path, metadata_path.c_str(), sizeof(tdb_ret.metadata.path));
    else
        std::cerr << "[test_description_builder] Your value for metadata.path ist to long. The maximum is " + std::to_string(sizeof(tdb_ret.metadata.path)) + " characters." << std::endl;

    tdb_ret.duration = duration;

    if(connection_ip.length() < sizeof(tdb_ret.connection.ip))
        strlcpy(tdb_ret.connection.ip, connection_ip.c_str(), sizeof(tdb_ret.connection.ip));
    else
        std::cerr << "[test_description_builder] Your value for connection.ip ist to long. The maximum is " + std::to_string(sizeof(tdb_ret.connection.ip)) + " characters." << std::endl;
    if(connection_bandwidth.length() < sizeof(tdb_ret.connection.bandwidth))
        strlcpy(tdb_ret.connection.bandwidth, connection_bandwidth.c_str(), sizeof(tdb_ret.connection.bandwidth));
    else
        std::cerr << "[test_description_builder] Your value for connection.bandwidth ist to long. The maximum is " + std::to_string(sizeof(tdb_ret.connection.bandwidth)) + " characters." << std::endl;
    tdb_ret.connection.datagramsize = connection_datagramsize;

    if(interface_client.length() < sizeof(tdb_ret.interface.client))
        strlcpy(tdb_ret.interface.client, interface_client.c_str(), sizeof(tdb_ret.interface.client));
    else
        std::cerr << "[test_description_builder] Your value for connection.ip ist to long. The maximum is " + std::to_string(sizeof(tdb_ret.interface.client)) + " characters." << std::endl;
    if(interface_server.length() < sizeof(tdb_ret.interface.server))
        strlcpy(tdb_ret.interface.server, interface_server.c_str(), sizeof(tdb_ret.interface.server));
    else
        std::cerr << "[test_description_builder] Your value for connection.bandwidth ist to long. The maximum is " + std::to_string(sizeof(tdb_ret.interface.server)) + " characters." << std::endl;

    tdb_ret.stress.type = stress_type;
    tdb_ret.stress.num = stress_num;
    tdb_ret.stress.location = stress_location;

    return tdb_ret;
}

test_description test_description_builder::build(enum test_description::metadata::method metadata_method, std::string metadata_path,
                                                 int duration,
                                                 std::string connection_ip, std::string connection_bandwidth, int connection_datagramsize,
                                                 std::string interface_client, std::string interface_server,
                                                 enum test_description::stress::type stress_type, int stress_num, enum test_description::stress::location stress_location)
{
    // Generierung der Test-UID im Format UUUUUU_HHMMSS_DDMMYY
    //      UUUUUU - Random ID
    //      HHMMSS - Stunden, Minuten und Sekunden (Erstellung der Test Description)
    //      DDMMYY - Tag, Monat, Jahr (Erstellung der Test Description)

    std::string generated_u_tid;

    std::mt19937 mt(time(0));
    std::uniform_int_distribution<int> dist(100000, 999999);
    generated_u_tid += std::to_string(dist(mt)) + "_";

    std::time_t current_time = std::time(nullptr);
    std::tm* time_info = std::localtime(&current_time);

    std::ostringstream formatted_time;
    formatted_time << std::setfill('0');
    formatted_time << std::setw(2) << time_info->tm_hour << std::setw(2) << time_info->tm_min << std::setw(2) << time_info->tm_sec << '_'
                   << std::setw(2) << time_info->tm_mday << std::setw(2) << (time_info->tm_mon + 1) << std::setw(2) << (time_info->tm_year % 100);
    generated_u_tid += formatted_time.str();

    return build(metadata_method, generated_u_tid, metadata_path,
                 duration,
                 connection_ip, connection_bandwidth, connection_datagramsize,
                 interface_client, interface_server,
                 stress_type, stress_num, stress_location);
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
