#include "test_scenario.h"
#include <fstream>

//
//                 __ ___ _ __  _ __  ___ _ _
//                / _/ _ \ '  \| '  \/ _ \ ' \
//  test_scenario_\__\___/_|_|_|_|_|_\___/_||_|
//

test_scenario::test_scenario(test_description description)
    : m_description(description)
{
    system(("mkdir -p " + std::string(m_description.metadata.path) + "/" + std::string(m_description.metadata.t_uid)).c_str());
}

void test_scenario::write_log(bool error, std::string error_message) {
    std::string basepath = std::string(m_description.metadata.path) + "/" + std::string(m_description.metadata.t_uid);
    std::time_t timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::string filepath_description = basepath + "/test_description.xml";
    test_description_parser::write_to_XML(filepath_description, m_description);
    std::string filepath_results = basepath + "/test_results.xml";
    if(!error) {
        test_results_parser::write_to_XML(filepath_results, m_results, m_description.metadata.method);
    }

    std::string filepath_log = basepath + "/" + get_type() + "_";
    if(error) {
        filepath_log += "error_log.txt";
    }
    else {
        filepath_log += "scenario_log.txt";
    }

    std::ofstream filestream(filepath_log);
    if(filestream.is_open()) {
        filestream << "[TD] PATH            : " << filepath_description << std::endl;
        filestream << std::endl;

        filestream << "[TS] REPORT TIMESTAMP: " << std::ctime(&timestamp);
        filestream << std::endl;

        if(error) {
            filestream << "[ER] ERROR MESSAGE   : " << error_message << std::endl;
            filestream << std::endl;
        }
        else {
            filestream << "[RE] PATH            : " << filepath_results << std::endl;
            filestream << std::endl;
        }
    }
    filestream.close();
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

void test_scenario_client::write_log(bool error, std::string error_message) {
    test_scenario::write_log();

    if((m_results.status != m_results.STATUS_SUCCESS) && (m_description.metadata.method == test_description::metadata::IPERF)) {
        std::string filepath = std::string(m_description.metadata.path) + "/" + get_type() + "_";
        std::ofstream filestream(filepath, std::ios_base::app);
        if(filestream.is_open()) {
            filestream << "[RE] IPERF COMPLETE_OUTPUT  : " << std::endl;
            filestream << m_client_ptr->get_current_output() << std::endl;
            filestream << std::endl;
        }
        filestream.close();
    }
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
