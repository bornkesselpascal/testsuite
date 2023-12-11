#include "test_control_client.h"
#include "test_scenario.h"
#include <iomanip>
#include <iostream>
#include <ctime>
#include <unistd.h>

const int gap_stepsize = 10000;


test_control_client::test_control_client(client_description description)
    : m_description(description)
    , m_comm_client(m_description.service_connection.server_ip, m_description.service_connection.port)
{
    // add startup time to description path
    std::time_t current_time = std::time(nullptr);
    std::tm* time_info = std::localtime(&current_time);
    std::ostringstream formatted_time;
    formatted_time << std::setfill('0');
    formatted_time << std::setw(2) << time_info->tm_hour << std::setw(2) << time_info->tm_min << std::setw(2) << time_info->tm_sec << '_'
                   << std::setw(2) << time_info->tm_mday << std::setw(2) << (time_info->tm_mon + 1) << std::setw(2) << (time_info->tm_year % 100);
    m_description.path += "_" + formatted_time.str();

    if (system(("mkdir -p " + std::string(m_description.path)).c_str()) < 0) {
        std::cerr << "Could not create test control path." << std::endl;
    }
}

void test_control_client::run() {
    switch (m_description.dynamic_behavoir.mode) {
    case client_description::dynamic_behavoir::DISABLED:
        run_dynamic_disabled();
        break;
    case client_description::dynamic_behavoir::DATAGRAM_SIZE:
        run_dynamic_datagramsize();
        break;
    case client_description::dynamic_behavoir::CYCLE_TIME:
        run_dynamic_cycletime();
        break;
    }
}

void test_control_client::run_dynamic_disabled()
{
    int ctl_datagramsize_index = 0;

    while(true) {
        test_description current_description = test_description_builder::build(m_description, m_description.target_connection.datagram_sizes.at(ctl_datagramsize_index));
        print_current_test_to_console(current_description, m_description.target_connection.datagram_sizes.at(ctl_datagramsize_index));
        test_results current_results = perform_scenario(current_description);

        std::cout << "Szenario beendet." << std::endl;
        sleep(1);

        ctl_datagramsize_index++;
        if(ctl_datagramsize_index >= m_description.target_connection.datagram_sizes.size()) {
            // Es wurden alle Datagramgroessen getestet.
            //    -> Test beenden

            break;
        }
    }
}

void test_control_client::run_dynamic_datagramsize()
{
    // Set the duration for each step.
    int duration = m_description.duration / m_description.dynamic_behavoir.steps;
    m_description.duration = duration;

    int datagram_size = m_description.dynamic_behavoir.min;
    int datagram_steps = (m_description.dynamic_behavoir.max - m_description.dynamic_behavoir.min) / m_description.dynamic_behavoir.steps;


    while(true) {
        test_description current_description = test_description_builder::build(m_description, datagram_size);
        print_current_test_to_console(current_description, datagram_size);
        test_results current_results = perform_scenario(current_description);

        std::cout << "Szenario beendet." << std::endl;
        sleep(1);

        datagram_size += datagram_steps;
        if(datagram_size > m_description.dynamic_behavoir.max) {
            // Es wurden alle Steps getestet.
            //    -> Test beenden

            break;
        }
    }
}

void test_control_client::run_dynamic_cycletime()
{
    // Set the duration for each step.
    int duration = m_description.duration / m_description.dynamic_behavoir.steps;
    m_description.duration = duration;

    int cycle_time = m_description.dynamic_behavoir.min;
    int cycle_steps = (m_description.dynamic_behavoir.max - m_description.dynamic_behavoir.min) / m_description.dynamic_behavoir.steps;

    int ctl_datagramsize_index = 0;

    while(true) {
        m_description.target_connection.cycletime = cycle_time;

        test_description current_description = test_description_builder::build(m_description, m_description.target_connection.datagram_sizes.at(ctl_datagramsize_index));
        print_current_test_to_console(current_description, m_description.target_connection.datagram_sizes.at(ctl_datagramsize_index));
        test_results current_results = perform_scenario(current_description);

        std::cout << "Szenario beendet." << std::endl;
        sleep(1);

        cycle_time += cycle_steps;
        if (!(cycle_time > m_description.dynamic_behavoir.max)) {
            // We dont reached the maximum. Continue with a smaller size.
            continue;
        }

        ctl_datagramsize_index++;
        cycle_time = m_description.dynamic_behavoir.min;
        if(ctl_datagramsize_index >= m_description.target_connection.datagram_sizes.size()) {
            // Es wurden alle Datagramgroessen getestet.
            //    -> Test beenden

            break;
        }
    }
}

test_results test_control_client::perform_scenario(test_description testdescription) {
    test_scenario_client current_scenario(testdescription);

    sleep(1);

    test_description_message td_m;
    td_m.description = testdescription;
    int bytes_send = m_comm_client.send(&td_m, sizeof(td_m));
    if(bytes_send != sizeof(td_m)) {
        std::cerr << "[tc_client] E02 - Error when sending data." << std::endl;
    }

    sleep(2);

    current_scenario.start();
    current_scenario.stop();

    sleep(10);

    communication::udp::message_type m_type = communication::udp::TSTOP_MSG;
    bytes_send = m_comm_client.send(&m_type, sizeof(m_type));
    if(bytes_send != sizeof(m_type)) {
        std::cerr << "[tc_client] E03 - Error when sending data." << std::endl;
    }

    test_results current_results = current_scenario.get_results();
    return current_results;
}

void test_control_client::print_current_test_to_console(test_description testdescription, int datagramsize) {
    std::cout << "* [TC_CLIENT] NEXT TEST" << std::endl;
    std::cout << "* - T_UID      : " << testdescription.metadata.t_uid << std::endl;
    std::cout << "* - CYCLETIME  : " << testdescription.connection.cycletime<< std::endl;
    std::cout << "* - DATAGR-SIZE: " << datagramsize << std::endl;
    std::cout << "* - DURATION   : " << testdescription.duration << std::endl;
}
