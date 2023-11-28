#include "test_scenario.h"
#include <fstream>
#include <iostream>

//
//                 __ ___ _ __  _ __  ___ _ _
//                / _/ _ \ '  \| '  \/ _ \ ' \
//  test_scenario_\__\___/_|_|_|_|_|_\___/_||_|
//

test_scenario::test_scenario(test_description description)
    : m_description(description)
{
    if (system(("mkdir -p " + std::string(m_description.metadata.path) + "/" + std::string(m_description.metadata.t_uid)).c_str()) < 0) {
        std::cerr << "Could not create scenario path." << std::endl;
    }
}

void test_scenario::write_log(bool error, std::string error_message) {
    std::string basepath = std::string(m_description.metadata.path) + "/" + std::string(m_description.metadata.t_uid);

    std::string filepath_description = basepath + "/test_description.xml";
    test_description_parser::write_to_XML(filepath_description, m_description);
    std::string filepath_results = basepath + "/test_results.xml";
    if(!error) {
        test_results_parser::write_to_XML(filepath_results, m_results, get_type());
    }
}


//                    _ _         _
//                 __| (_)___ _ _| |_
//                / _| | / -_) ' \  _|
//  test_scenario_\__|_|_\___|_||_\__|
//

test_scenario_client::test_scenario_client(test_description description)
    : test_scenario(description)
{
}

bool test_scenario_client::start() {
    try {
        m_client_ptr = std::unique_ptr<iperf_client>(new iperf_client(m_description));
        m_client_ptr->start();

        if((m_description.stress.location == test_description::stress::LOC_CLIENT) || (m_description.stress.location == test_description::stress::LOC_BOTH)) {
            m_stress_ptr = std::unique_ptr<stress>(new stress(m_description));
        }
    } catch (const std::exception& ex) {
        write_log(true, ex.what());
        return false;
    }

    return true;
}

bool test_scenario_client::stop() {
    try {
        m_results = m_client_ptr->get_results();
    } catch (const std::exception& ex) {
        write_log(true, ex.what());
        return false;
    }

    if(m_stress_ptr != nullptr) {
        m_stress_ptr->stop();
    }

    write_log();

    return true;
}



//
//                 ___ ___ _ ___ _____ _ _
//                (_-</ -_) '_\ V / -_) '_|
//  test_scenario_/__/\___|_|  \_/\___|_|
//

test_scenario_server::test_scenario_server(test_description description, std::shared_ptr<iperf_server> server_ptr)
    : test_scenario(description)
    , m_server_ptr(server_ptr)
{
    m_server_ptr->load_test(m_description);
}

bool test_scenario_server::start() {
    try {
        if((m_description.stress.location == test_description::stress::LOC_SERVER) || (m_description.stress.location == test_description::stress::LOC_BOTH)) {
            m_stress_ptr = std::unique_ptr<stress>(new stress(m_description));
        }
    } catch (const std::exception& ex) {
        write_log(true, ex.what());
        return false;
    }

    return true;
}

bool test_scenario_server::stop() {
    try {
        m_results = m_server_ptr->get_results();
    } catch (const std::exception& ex) {
        write_log(true, ex.what());
        return false;
    }

    if(m_stress_ptr != nullptr) {
        m_stress_ptr->stop();
    }

    write_log();

    return true;
}
