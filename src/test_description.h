#ifndef TEST_DESCRIPTION_H
#define TEST_DESCRIPTION_H

#include <string>
#include "communication.h"
#include "lib/uce/uce.h"

struct client_description;

struct test_description {
    struct metadata {
        char t_uid[21];
        char path[50];
    } metadata;

    int duration;

    struct connection {
        enum uce::sock_type type;
        char client_ip[16];         // IP-Addresse des Clients
        char server_ip[16];         // IP-Addresse des Servers
        int  port;                  // Port
        int  cycletime;             // Zeit zwischen Nachrichten [ns]
        int  datagram_size;         // Datagramgroesse
    } connection;

    struct interface {
        char client[11];
        char server[11];
    } interface;

    struct stress {
        enum type {
            CPU_USR,
            CPU_KERNEL,
            CPU_REALTIME,
            MEMORY,
            IO,
            TIMER,
            NONE,
        } type;
        int num;
        enum location {
            LOC_CLIENT,
            LOC_SERVER,
            LOC_BOTH,
        } location;
    } stress;
};

typedef enum test_description::stress::type stress_type;
typedef enum test_description::stress::location stress_location;

struct test_description_message {
    const enum communication::udp::message_type type = communication::udp::DESCR_MSG;
    struct test_description description;
};



class test_description_builder
{
public:
    static test_description build(client_description description, int datagramsize);
    test_description_builder() = delete;
};

class test_description_parser
{
public:
    static void write_to_XML(std::string filename, test_description &description);
    test_description_parser() = delete;
};

#endif // TEST_DESCRIPTION_H
