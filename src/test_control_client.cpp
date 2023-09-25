#include "test_control_client.h"
#include "test_scenario.h"
#include <iostream>
#include <unistd.h>

test_control_client::test_control_client(client_description description)
    : m_description(description)
    , m_comm_client(m_description.service_connection.server_ip, m_description.service_connection.port)
{
    system(("mkdir -p " + std::string(m_description.path)).c_str());
    test_control_logger::log_control(m_description);
}

void test_control_client::run() {
    if(m_description.stress.num.steps < 2) {
        std::cerr << "[tc_client] E01 - You need at least 2 stress steps." << std::endl;
        return;
    }
    double c_stress_current  = m_description.stress.num.num_max;                                                                                                    // start with the highest level
    double c_stress_stepsize = ((double) (m_description.stress.num.num_max - m_description.stress.num.num_min)) / ((double) (m_description.stress.num.steps - 1));  // calculate stepsize
    bool   c_stress_loss_occured = false;

    int c_duration_current = m_description.duration.short_duration;
    int c_datagramsize_index = 0;

    while(true) {
        test_description current_description = test_description_builder::simple_build(m_description, c_duration_current, m_description.target_connection.datagram.sizes.at(c_datagramsize_index), c_stress_current);
        test_results     current_results = perform_scenario(current_description);

        if(get_loss_counter(current_results) > 0) {
            c_stress_loss_occured = true;
        }

        if(c_duration_current == m_description.duration.short_duration) {
            if(get_loss_counter(current_results) == 0) {
                // Es trat kein Verlust beim kurzen Test auf.
                //    -> Fortfahren mit langem Test.

                c_duration_current = m_description.duration.long_duration;
                continue;
            }
        }
        else {
            c_duration_current = m_description.duration.short_duration;
        }

        c_datagramsize_index++;
        if(c_datagramsize_index >= m_description.target_connection.datagram.sizes.size()) {
            // Es wurden alle Datagramgroessen getestet.
            //    -> Fortfahren mit niedrigerem Stresslevel.

            c_datagramsize_index = 0;

            if(c_stress_loss_occured) {
                c_stress_loss_occured = false;
            }
            else {
                // Es wurde kein Verlust erkannt, niedrige Stressoren sollen (nach Absprache) dann nicht getestet werden.
                //    -> Beenden von Test Control.

                break;
            }

            c_stress_current -= c_stress_stepsize;
            if(c_stress_current < m_description.stress.num.num_min) {
                // Es wurden alle Stress-Level getestet.
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
