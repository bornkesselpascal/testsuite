#ifndef TEST_CONTROL_CLIENT_H
#define TEST_CONTROL_CLIENT_H

#include "test_control.h"
#include "communication.h"
#include "test_results.h"

class test_control_client
{
public:
    test_control_client(client_description description);
    void run();

private:
    void run_dynamic_disabled();
    void run_dynamic_datagramsize();
    void run_dynamic_cycletime();
    test_results perform_scenario(test_description testdescription);
    void print_current_test_to_console(test_description testdescription, int datagramsize_index);

    client_description m_description;
    communication::udp::client m_comm_client;
};

#endif // TEST_CONTROL_CLIENT_H
