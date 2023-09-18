#ifndef TEST_DESCRIPTION_H
#define TEST_DESCRIPTION_H

#include <string>

struct test_description {
    struct metadata {
        enum method {
            IPERF,
            CUSTOM,
        } method;
        char t_uid[21];
        char path[50];
    } metadata;

    int duration;

    struct connection {
        struct iperf {
            char server_ip[16];         // IP-Addresse des Servers
            char bandwidth_limit[4];    // maximale Bandbreite
            struct datagram {
                int size;               // Datagramgroesse
            } datagram;
        } iperf;

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


class test_description_builder
{
public:
    static test_description build(enum test_description::metadata::method metadata_method, std::string metadata_path,
                                  int duration,
                                  std::string iperf_serverip, std::string iperf_bandwidthlimit, int iperf_datagramsize,
                                  std::string interface_client, std::string interface_server,
                                  enum test_description::stress::type stress_type, int stress_num, enum test_description::stress::location stress_location);
    static test_description build(enum test_description::metadata::method metadata_method, std::string metadata_path,
                                  int duration,
                                  std::string custom_clientip,std::string custom_serverip, int custom_port, int custom_gap, int custom_datagramsize, bool custom_datagramrandom,
                                  std::string interface_client, std::string interface_server,
                                  enum test_description::stress::type stress_type, int stress_num, enum test_description::stress::location stress_location);


    static test_description read_from_XML();
    static void             write_to_XML();

    test_description_builder() = delete;
};

class test_description_parser
{
public:
    static test_description read_from_XML();
    static void             write_to_XML(test_description description);

    test_description_parser() = delete;
};

#endif // TEST_DESCRIPTION_H
