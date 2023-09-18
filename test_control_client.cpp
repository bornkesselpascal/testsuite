#include "test_control_client.h"
#include "test_scenario.h"
#include <iostream>
#include <unistd.h>

test_control_client::test_control_client(client_description description)
    : m_description(description)
    , m_comm_client(m_description.svc_connection.ip, m_description.svc_connection.port)
{
    system(("mkdir -p " + std::string(m_description.path)).c_str());
    test_control_common::log_control(m_description);
}

void test_control_client::run() {
    // Stress
    if(m_description.stress.num.steps < 2) {
        // print error here and abort
        return;
    }
    double c_stress_current  = m_description.stress.num.num_max;                                                                                                    // start with the highest level
    double c_stress_stepsize = ((double) (m_description.stress.num.num_max - m_description.stress.num.num_min)) / ((double) (m_description.stress.num.steps - 1));  // calculate stepsize


    while(true) {
        if(m_description.duration.min_duration != -1) {
            // Execution of a test with duration.min_duration
            test_description c_mindur_tdf = test_description_builder::build(
                m_description.method, m_description.path,
                m_description.duration.min_duration,
                m_description.target_connection.ip, m_description.target_connection.bandwidth, m_description.target_connection.datagramsize,
                m_description.interface.client, m_description.interface.server,
                m_description.stress.type, int(c_stress_current), m_description.stress.location);
            test_results     c_mindur_res = perform_scenario(c_mindur_tdf);
            if((c_mindur_tdf.metadata.method == test_description::metadata::IPERF) && (c_mindur_res.iperf.num_loss > 0)) {
                if(!(c_stress_current <= m_description.stress.num.num_min)) {
                    c_stress_current -= c_stress_stepsize;
                }
                else {
                    break;
                }
                continue;   // We continue with a test with lower stress level.
            }
            else if((c_mindur_tdf.metadata.method == test_description::metadata::CTEST) && (c_mindur_res.custom.num_loss > 0)){
                if(!(c_stress_current <= m_description.stress.num.num_min)) {
                    c_stress_current -= c_stress_stepsize;
                }
                else {
                    break;
                }
                continue;   // We continue with a test with lower stress level.
            }
            test_control_common::log_scenario(c_mindur_tdf, &c_mindur_res);
        }

        if(m_description.duration.mid_duration != -1) {
            // Execution of a test with duration.mid_duration
            test_description c_middur_tdf = test_description_builder::build(
                m_description.method, m_description.path,
                m_description.duration.mid_duration,
                m_description.target_connection.ip, m_description.target_connection.bandwidth, m_description.target_connection.datagramsize,
                m_description.interface.client, m_description.interface.server,
                m_description.stress.type, int(c_stress_current), m_description.stress.location);
            test_results     c_middur_res = perform_scenario(c_middur_tdf);
            if((c_middur_tdf.metadata.method == test_description::metadata::IPERF) && (c_middur_res.iperf.num_loss > 0)) {
                if(!(c_stress_current <= m_description.stress.num.num_min)) {
                    c_stress_current -= c_stress_stepsize;
                }
                else {
                    break;
                }
                continue;   // We continue with a test with lower stress level.
            }
            else if((c_middur_tdf.metadata.method == test_description::metadata::CTEST) && (c_middur_res.custom.num_loss > 0)){
                if(!(c_stress_current <= m_description.stress.num.num_min)) {
                    c_stress_current -= c_stress_stepsize;
                }
                else {
                    break;
                }
                continue;   // We continue with a test with lower stress level.
            }
            test_control_common::log_scenario(c_middur_tdf, &c_middur_res);
        }

        if(m_description.duration.max_duration != -1) {
            // Execution of a test with duration.max_duration
            test_description c_maxdur_tdf = test_description_builder::build(
                m_description.method, m_description.path,
                m_description.duration.max_duration,
                m_description.target_connection.ip, m_description.target_connection.bandwidth, m_description.target_connection.datagramsize,
                m_description.interface.client, m_description.interface.server,
                m_description.stress.type, int(c_stress_current), m_description.stress.location);
            test_results     c_maxdur_res = perform_scenario(c_maxdur_tdf);
            test_control_common::log_scenario(c_maxdur_tdf, &c_maxdur_res);
        }


        if(!(c_stress_current <= m_description.stress.num.num_min)) {
            c_stress_current -= c_stress_stepsize;
        }
        else {
            break;
        }
    }
}

test_results test_control_client::perform_scenario(test_description testdescription) {
    system(("mkdir -p " + std::string(testdescription.metadata.path)).c_str());
    test_scenario_client current_scenario(testdescription);

    sleep(1);

    if(!m_description.client_only) {
        test_description_message td_m;
        td_m.description = testdescription;
        int bytes_send = m_comm_client.send(&td_m, sizeof(td_m));
        if(bytes_send != sizeof(td_m)) {
            std::cerr << "[tc_client] E01 - Error when sending data." << std::endl;
        }
    }

    sleep(2);

    current_scenario.start();
    current_scenario.stop();

    sleep(1);

    if(!m_description.client_only) {
        communication::udp::message_type m_type = communication::udp::TSTOP_MSG;
        int bytes_send = m_comm_client.send(&m_type, sizeof(m_type));
        if(bytes_send != sizeof(m_type)) {
            std::cerr << "[tc_client] E02 - Error when sending data." << std::endl;
        }
    }

    return current_scenario.get_results();
}
