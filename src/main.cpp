#include "iperf.h"
#include "stress.h"
#include "test_control_common.h"
#include "test_control_server.h"
#include "test_scenario.h"
#include <iostream>
#include <unistd.h>
#include <fstream>
#include "metrics.h"
#include "test_description.h"
#include "test_results.h"
#include "test_control_client.h"
#include "custom_tester.h"

void stress_example();

int main() {
    size_t test1 = sizeof(test_description);
    size_t test3 = sizeof(test_results);

//    test_description tdf = test_description_builder::build(test_description::metadata::method::IPERF, "/testsuite/examplenew",
//                                                           20,
//                                                           "10.30.0.2",
//                                                           "10G",
//                                                           8972,
//                                                           "enp1s0f0",
//                                                           "enp1s0f0",
//                                                           test_description::stress::type::CPU_USR,
//                                                           8
//                                                           );

//    test_scenario_client example_scenario(tdf);
//    // NOTIFY SERVER HERE
//    example_scenario.start();
//    example_scenario.stop();
//    // NOTIFY SERVER HERE AGAIN

//    test_control_client m_client("10.30.0.2");
//    m_client.start();

//    test_control_server m_server({{"10.30.0.2", 8080}, "enp1s0f0", 8972});
//    m_server.run();


    client_description m_descr = {
        test_description::metadata::CTEST, "/testsuite/weekendtest", {10, 60, 21600}, {"10.0.0.2", "10G", 1000}, {"enp1s0f0", "enp1s0f0"}, {"10.30.0.2", 8085}, false, {test_description::stress::CPU_USR, {1, 16, 10}}
    };

    test_control_client  m_client(m_descr);
    m_client.run();


//    custom_tester_client m_test({"10.0.0.1", "10.0.0.2", 8095, 50, 20, 10});
//    int num = m_test.run();


    return 0;
}

void stress_example() {

}
