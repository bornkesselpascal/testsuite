#include "test_control_server.h"

#include <iostream>

const size_t MAX_MSG_SIZE = sizeof(test_description_message);


test_control_server::test_control_server(server_description description)
    : m_description(description)
    , m_comm_server(m_description.service_connection.server_ip, m_description.service_connection.port)
{
    system(("mkdir -p " + std::string(m_description.path)).c_str());
    test_control_logger::log_control(m_description);
}

void test_control_server::run() {
    void* msg_buff = malloc(MAX_MSG_SIZE);

    while(true) {
        int bytes_received = m_comm_server.receive(msg_buff, MAX_MSG_SIZE);
        if(bytes_received == -1) {
            std::cerr << "[tc_server] E02 - Error when receiving data." << std::endl;
            continue;
        }

        communication::udp::message_type* msg_type = (communication::udp::message_type*) msg_buff;

        switch(*msg_type) {
        case communication::udp::DESCR_MSG:
        {
            test_description_message* msg_tdm = (test_description_message*) msg_buff;
            m_testdescription = msg_tdm->description;

            handle_DESCR_MSG();
            break;
        }
        case communication::udp::TSTOP_MSG:
        {
            handle_TSTOP_MSG();
            break;
        }
        default:
        {
            std::cerr << "[tc_server] E03 - Unknown message type." << std::endl;
            break;
        }
        };
    }
}

void test_control_server::handle_DESCR_MSG() {
    if(m_iperf_server_ptr == nullptr) {
        m_iperf_server_ptr = std::shared_ptr<iperf_server>(new iperf_server(m_testdescription.metadata.method, m_testdescription.connection.iperf.datagram.size));
        m_iperf_server_ptr->start();
    }

    m_scenario_ptr = std::unique_ptr<test_scenario_server>(new test_scenario_server(m_testdescription, m_iperf_server_ptr));
    m_scenario_ptr->start();
}

void test_control_server::handle_TSTOP_MSG() {
    if(m_scenario_ptr != nullptr) {
        m_scenario_ptr->stop();
        m_scenario_ptr.release();
    }

    // test_control_logger::log_scenario(m_description.path, m_testdescription);
}
