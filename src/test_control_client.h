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
    test_results perform_scenario(test_description testdescription);
    long long int get_loss_counter(test_results &results);

    client_description m_description;
    communication::udp::client m_comm_client;
};

#endif // TEST_CONTROL_CLIENT_H
