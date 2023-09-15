#ifndef TEST_DESCRIPTION_H
#define TEST_DESCRIPTION_H

#include <string>

struct test_description {
    struct metadata {
        enum method {
            IPERF,
            CTEST,
        } method;
        char t_uid[21];
        char path[50];
    } metadata;

    int duration;

    struct connection {
        char ip[16];
        char bandwidth[4];
        int  datagramsize;
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
    } stress;
};


class test_description_builder
{
public:
    static test_description build(enum test_description::metadata::method metadata_method, std::string metadata_tuid, std::string metadata_path,
                                  int duration,
                                  std::string connection_ip, std::string connection_bandwidth, int connection_datagramsize,
                                  std::string interface_client, std::string interface_server,
                                  enum test_description::stress::type stress_type, int stress_num);
    static test_description build(enum test_description::metadata::method metadata_method, std::string metadata_path,
                                  int duration,
                                  std::string connection_ip, std::string connection_bandwidth, int connection_datagramsize,
                                  std::string interface_client, std::string interface_server,
                                  enum test_description::stress::type stress_type, int stress_num);

    test_description_builder() = delete;
};

#endif // TEST_DESCRIPTION_H
