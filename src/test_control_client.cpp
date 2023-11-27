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
    std::time_t current_time = std::time(nullptr);
    std::tm* time_info = std::localtime(&current_time);

    std::ostringstream formatted_time;
    formatted_time << std::setfill('0');
    formatted_time << std::setw(2) << time_info->tm_hour << std::setw(2) << time_info->tm_min << std::setw(2) << time_info->tm_sec << '_'
                   << std::setw(2) << time_info->tm_mday << std::setw(2) << (time_info->tm_mon + 1) << std::setw(2) << (time_info->tm_year % 100);
    m_description.path += "_" + formatted_time.str();

    system(("mkdir -p " + std::string(m_description.path)).c_str());
    test_control_logger::log_control(m_description);
}

void test_control_client::run() {

    double ctl_stress_current, ctl_stress_stepsize = 0;
    bool   ctl_stress_loss = false;
    if(stress_type::NONE != m_description.stress.type) {
        ctl_stress_current  = m_description.stress.num.num_max;
        ctl_stress_stepsize = ((double) (m_description.stress.num.num_max - m_description.stress.num.num_min)) / ((double) (m_description.stress.num.steps - 1));
    }

    int ctl_duration = m_description.duration.short_duration;
    int ctl_datagramsize_index = 0;

    /// MODIFIKATIONEN FUER REALSTRESS-TESTS
//    bool ctl_gap_init = false;
    /// MODIFIKATIONEN ENDE

    while(true) {
        test_description current_description = test_description_builder::simple_build(m_description,
                                                                                      ctl_duration,
                                                                                      m_description.target_connection.datagram.sizes.at(ctl_datagramsize_index),
                                                                                      ctl_stress_loss);
        print_current_test_to_console(current_description, ctl_datagramsize_index, ctl_duration, ctl_stress_current);
        test_results current_results = perform_scenario(current_description);

        std::cout << "Szenario beendet." << std::endl;
        sleep(1);

        if(get_loss_counter(current_results) > 0) {
            ctl_stress_loss = true;
        }

        /// MODIFIKATIONEN FUER REALSTRESS-TESTS
//        if(ctl_gap_init) {
//            m_description.target_connection.gap += gap_stepsize;
//        }
//        else {
//            if(m_description.target_connection.datagram.sizes.at(ctl_datagramsize_index) < 9000) {
//                m_description.target_connection.gap = 10000;
//            }
//            else {
//                m_description.target_connection.gap = 60000;
//            }
//            ctl_gap_init = true;
//        }

//        if(ctl_stress_loss) {
//            ctl_stress_loss = false;
//            continue;
//        }
//        else {
//            m_description.target_connection.gap = 2500;
//            ctl_gap_init = false;

//            // Kein Verlust aufgetreten.
//            //    -> Fortfahren mit weiterer Datagrammgroesse.
//        }
        /// MODIFIKATIONEN ENDE

        ctl_datagramsize_index++;
        if(ctl_datagramsize_index >= m_description.target_connection.datagram.sizes.size()) {
            // Es wurden alle Datagramgroessen getestet.
            //    -> Fortfahren mit niedrigerem Stresslevel oder beenden.

            ctl_datagramsize_index = 0;

            if(ctl_stress_loss) {
                ctl_stress_loss = false;
            }
            else {
                // Es wurde kein Verlust erkannt, niedrige Stressoren sollen (nach Absprache) dann nicht getestet werden.
                //    -> Beenden von Test Control.

                break;
            }

            ctl_stress_current -= ctl_stress_stepsize;
            if(ctl_stress_current < m_description.stress.num.num_min || stress_type::NONE == m_description.stress.type) {
                // Es wurden alle Stress-Level getestet oder wir testen mit NONE.
                //    -> Beenden von Test Control.

                break;
            }
            continue;
        }
    }
}

test_results test_control_client::perform_scenario(test_description testdescription) {
    test_scenario_client current_scenario(testdescription);

    sleep(1);

    if(!m_description.client_only) {
        test_description_message td_m;
        td_m.description = testdescription;
        int bytes_send = m_comm_client.send(&td_m, sizeof(td_m));
        if(bytes_send != sizeof(td_m)) {
            std::cerr << "[tc_client] E02 - Error when sending data." << std::endl;
        }
    }

    sleep(2);

    current_scenario.start();
    current_scenario.stop();

    sleep(4);

    if(!m_description.client_only) {
        communication::udp::message_type m_type = communication::udp::TSTOP_MSG;
        int bytes_send = m_comm_client.send(&m_type, sizeof(m_type));
        if(bytes_send != sizeof(m_type)) {
            std::cerr << "[tc_client] E03 - Error when sending data." << std::endl;
        }
    }

    test_results current_results = current_scenario.get_results();
    test_control_logger::log_scenario(m_description.path, testdescription, &current_results);
    return current_results;
}

long long int test_control_client::get_loss_counter(test_results &results) {
    switch(m_description.method) {
    case test_description::metadata::IPERF:
        return results.iperf.num_loss;
        break;
    case test_description::metadata::CUSTOM:
        return results.custom.num_loss;
        break;
    }

    return -1;
}

void test_control_client::print_current_test_to_console(test_description testdescription, int datagramsize_index, int duration, int stress) {
    std::cout << "* [TC_CLIENT] NEXT TEST" << std::endl;
    std::cout << "* - T_UID       : " << testdescription.metadata.t_uid << std::endl;
    std::cout << "* - GAP         : " << testdescription.connection.custom.gap << std::endl;
    std::cout << "* - DATAGR-SIZE : " << m_description.target_connection.datagram.sizes.at(datagramsize_index) << std::endl;
    std::cout << "* - MAX DURATION: " << duration << std::endl;

    if(stress_type::NONE != m_description.stress.type) {
        std::cout << "* - STRESS INT. : " << stress << std::endl;
    }
}
