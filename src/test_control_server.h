#ifndef TEST_CONTROL_SERVER_H
#define TEST_CONTROL_SERVER_H

#include <memory>
#include <mutex>
#include "test_control.h"
#include "communication.h"
#include "iperf.h"
#include "test_scenario.h"

class test_control_server
{
public:
    test_control_server(server_description description);
    void run();

private:
    void handle_DESCR_MSG();
    void handle_TSTOP_MSG();

    std::shared_ptr<iperf_server> m_iperf_server_ptr;
    std::unique_ptr<test_scenario_server> m_scenario_ptr;
    test_description m_testdescription;

    server_description m_description;
    communication::udp::server m_comm_server;

    mutable std::mutex m_mutex;
};

#endif // TEST_CONTROL_SERVER_H
