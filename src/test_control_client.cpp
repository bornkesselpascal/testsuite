#include "test_control_client.h"
#include "test_scenario.h"
#include <iostream>
#include <unistd.h>

test_control_client::test_control_client(client_description description)
    : m_description(description)
    , m_comm_client(m_description.service_connection.server_ip, m_description.service_connection.port)
{
    m_description.path = "/testsuite/interest_test_server_rt_repeat";
    m_description.stress.location = test_description::stress::LOC_SERVER;
    m_description.duration.short_duration = 100;

    system(("mkdir -p " + std::string(m_description.path)).c_str());
    test_control_logger::log_control(m_description);

}

void test_control_client::run() {
    if(m_description.stress.num.steps < 2) {
        std::cerr << "[tc_client] E01 - You need at least 2 stress steps." << std::endl;
        return;
    }
    double c_stress_current  = 15;                                                                                                 // start with the highest level
    double c_stress_stepsize = 16; // calculate stepsize
    bool   c_stress_loss_occured = false;

    // int c_duration_current = m_description.duration.short_duration;
    int c_datagramsize_index = 0;

    for(const auto e: {test_description::stress::CPU_REALTIME}) {
        while(true) {
            int gap_ns = 10000; // 10 us
            while(gap_ns > 0) {
                std::cout << m_description.duration.short_duration << "   gap " << gap_ns << "  datagram_size " << m_description.target_connection.datagram.sizes.at(c_datagramsize_index) << std::endl;
                test_description current_description = test_description_builder::build(m_description.path,
                                                                                       m_description.duration.short_duration,
                                                                                       m_description.target_connection.client_ip, m_description.target_connection.server_ip, m_description.target_connection.port, gap_ns, m_description.target_connection.datagram.sizes.at(c_datagramsize_index), m_description.target_connection.datagram.random,
                                                                                       m_description.interface.client, m_description.interface.server,
                                                                                       e, c_stress_current, m_description.stress.location);
                test_results     current_results = perform_scenario(current_description);

                if(get_loss_counter(current_results)) {
                    c_stress_loss_occured = true;
                }

                gap_ns -= 2500;
                if(gap_ns == 0) {
                    gap_ns = 1;
                }

                sleep(5);
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
                if(c_stress_current < 0) {
                    // Es wurden alle Stress-Level getestet.
                    //    -> Beenden von Test Control.

                    break;
                }
                continue;
            }

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

    sleep(1);

    current_scenario.start();
    current_scenario.stop();

    sleep(3);

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
