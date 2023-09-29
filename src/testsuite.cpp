#include "custom_tester.h"
#include "test_control.h"
#include <iostream>
#include <unistd.h>
#include <fstream>
#include "test_control_client.h"
#include "test_control_server.h"

void stress_example();

int main() {
    std::string file_name = "/testsuite/master_list.xml";

    std::vector<std::string> filepaths = test_control_parser::read_client_main_XML(file_name);

    for (const std::string& current_path : filepaths) {
        client_description current_description = test_control_parser::read_client_from_XML(current_path);

        test_control_client current_client(current_description);
        current_client.run();
    }

    return 0;

//std::string filename = "/home/aidass/Developer/TestSuite/server_description.xml";
//server_description mdesc = test_control_parser::read_server_from_XML(filename);
//test_control_server mserv(mdesc);
//mserv.run();


//    custom_tester_client client_test({"10.30.0.1", "10.30.0.2", 8100, 100000, {65500, false}, 30});
//    //custom_tester_server server_test({"10.30.0.1", "10.30.0.2", 8100, {10000}});

//    struct test_results results;

//    client_test.run(&(results.custom));
//    //server_test.run(&(results.custom));

//    test_results_parser::write_to_XML("/home/aidass/Downloads/results.xml", results, test_description::metadata::CUSTOM);


//    return 0;

//client_description m_description = {
//    test_description::metadata::CUSTOM, "/testsuite/custom_test_eno1", {40, 60}, {"10.30.0.1", "10.30.0.2", "", 8100, 100000, {{500, 1450, 8900, 65000}, false}}, {"enp1s0f0", "enp1s0f0"}, {"10.30.0.2", 8080}, false, {test_description::stress::CPU_USR, {1, 16, 3}, test_description::stress::LOC_BOTH}
//};

//test_control_parser::write_client_to_XML("/testsuite/tc_client_files/custom_example.xml", m_description);





}
