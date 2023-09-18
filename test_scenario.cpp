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
}

void test_scenario::write_log(bool error, std::string error_message) {
    std::time_t timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string filepath = std::string(m_description.metadata.path) + "/" + get_type() + "_";

    if(error) {
        filepath += "error_log_" + std::string(m_description.metadata.t_uid) + ".txt";
    }
    else {
        filepath += "result_log_" + std::string(m_description.metadata.t_uid) + ".txt";
    }

    std::ofstream filestream(filepath);
    if(filestream.is_open()) {
        filestream << "[TD] METADATA METHOD        : " << m_description.metadata.method << std::endl;
        filestream << "[TD] METADATA T_UID         : " << m_description.metadata.t_uid << std::endl;
        filestream << "[TD] METADATA PATH          : " << m_description.metadata.path << std::endl;
        filestream << "[TD] DURATION               : " << m_description.duration << std::endl;
        filestream << "[TD] CONNECTION IP          : " << m_description.connection.ip << std::endl;
        filestream << "[TD] CONNECTION BANDWIDTH   : " << m_description.connection.bandwidth << std::endl;
        filestream << "[TD] CONNECTION DATAGRAMSIZE: " << m_description.connection.datagramsize << std::endl;
        filestream << "[TD] INTERFACE CLIENT       : " << m_description.interface.client << std::endl;
        filestream << "[TD] INTERFACE SERVER       : " << m_description.interface.server << std::endl;
        filestream << "[TD] STRESS TYPE            : " << m_description.stress.type << std::endl;
        filestream << "[TD] STRESS NUM             : " << m_description.stress.num << std::endl;
        filestream << std::endl;

        filestream << "[TS] REPORT TIMESTAMP       : " << std::ctime(&timestamp);
        filestream << std::endl;

        if(error) {
            filestream << "[ER] ERROR MESSAGE          : " << error_message << std::endl;
            filestream << std::endl;
        }
        else {
            switch (m_results.status) {
            case test_results::status::STATUS_SUCCESS:
                filestream << "[RE] STATUS                 : " << "SUCCESS" << std::endl;
                break;
            case test_results::status::STATUS_FAIL:
                filestream << "[RE] STATUS                 : " << "FAIL" << std::endl;
                break;
            default:
                filestream << "[RE] STATUS                 : " << "UNKNOWN" << std::endl;
                break;
            }

            if(m_description.metadata.method == test_description::metadata::IPERF) {
                filestream << "[RE] IPERF INTERVAL         : " << m_results.iperf.interval << std::endl;
                filestream << "[RE] IPERF TANSFER          : " << m_results.iperf.transfer << std::endl;
                filestream << "[RE] IPERF BANDWIDTH        : " << m_results.iperf.bandwidth << std::endl;
                filestream << "[RE] IPERF JITTER           : " << m_results.iperf.jitter << std::endl;
                filestream << "[RE] IPERF DTGR_LOSS        : " << std::to_string(m_results.iperf.num_loss) << std::endl;
                filestream << "[RE] IPERF DTGR_TOTAL       : " << std::to_string(m_results.iperf.num_total) << std::endl;
                filestream << std::endl;

                filestream << "[RE] IPERF STARTUP_MESSAGE  : " << std::endl;
                filestream << m_results.iperf.startup_message << std::endl;
                filestream << std::endl;
            }
            else if (m_description.metadata.method == test_description::metadata::CTEST) {
                filestream << "[RE] CTEST DTGR_LOSS        : " << std::to_string(m_results.custom.num_loss) << std::endl;
                filestream << std::endl;
            }

            filestream << "[MT] ETH RX_PACKETS    (S/E): " << m_results.ethtool_statistic_start.rx_packets << "/" << m_results.ethtool_statistic_end.rx_packets << std::endl;
            filestream << "[MT] ETH TX_PACKETS    (S/E): " << m_results.ethtool_statistic_start.tx_packets << "/" << m_results.ethtool_statistic_end.tx_packets << std::endl;
            filestream << "[MT] ETH RX_DROPPED    (S/E): " << m_results.ethtool_statistic_start.rx_dropped << "/" << m_results.ethtool_statistic_end.rx_dropped << std::endl;
            filestream << "[MT] ETH TX_DROPPED    (S/E): " << m_results.ethtool_statistic_start.tx_dropped << "/" << m_results.ethtool_statistic_end.tx_dropped << std::endl;
            filestream << "[MT] ETH COLLISIONS    (S/E): " << m_results.ethtool_statistic_start.collisions << "/" << m_results.ethtool_statistic_end.collisions << std::endl;
            filestream << "[MT] ETH PORT_TX_ERRORS(S/E): " << m_results.ethtool_statistic_start.port_tx_errors << "/" << m_results.ethtool_statistic_end.port_tx_errors << std::endl;
            filestream << "[MT] ETH PORT_RX_DRP   (S/E): " << m_results.ethtool_statistic_start.port_rx_dropped << "/" << m_results.ethtool_statistic_end.port_rx_dropped << std::endl;
            filestream << "[MT] ETH PORT_TX_DRP_LD(S/E): " << m_results.ethtool_statistic_start.port_tx_dropped_link_down << "/" << m_results.ethtool_statistic_end.port_tx_dropped_link_down << std::endl;
            filestream << std::endl;

            filestream << "[MT] IP MTU                 : " << ((m_results.ip_statistic_start.mtu == m_results.ip_statistic_end.mtu) ? m_results.ip_statistic_start.mtu : -1) << std::endl;
            filestream << "[MT] IP RX_BYTES       (S/E): " << m_results.ip_statistic_start.rx_bytes << "/" << m_results.ip_statistic_end.rx_bytes << std::endl;
            filestream << "[MT] IP RX_PACKETS     (S/E): " << m_results.ip_statistic_start.rx_packets << "/" << m_results.ip_statistic_end.rx_packets << std::endl;
            filestream << "[MT] IP RX_ERRORS      (S/E): " << m_results.ip_statistic_start.rx_errors << "/" << m_results.ip_statistic_end.rx_errors << std::endl;
            filestream << "[MT] IP RX_DROPPED     (S/E): " << m_results.ip_statistic_start.rx_dropped << "/" << m_results.ip_statistic_end.rx_dropped << std::endl;
            filestream << "[MT] IP RX_MISSED      (S/E): " << m_results.ip_statistic_start.rx_missed << "/" << m_results.ip_statistic_end.rx_missed << std::endl;
            filestream << "[MT] IP RX_MCAST       (S/E): " << m_results.ip_statistic_start.rx_mcast << "/" << m_results.ip_statistic_end.rx_mcast << std::endl;
            filestream << "[MT] IP TX_BYTES       (S/E): " << m_results.ip_statistic_start.tx_bytes << "/" << m_results.ip_statistic_end.tx_bytes << std::endl;
            filestream << "[MT] IP TX_PACKETS     (S/E): " << m_results.ip_statistic_start.tx_packets << "/" << m_results.ip_statistic_end.tx_packets << std::endl;
            filestream << "[MT] IP TX_ERRORS      (S/E): " << m_results.ip_statistic_start.tx_errors << "/" << m_results.ip_statistic_end.tx_errors << std::endl;
            filestream << "[MT] IP TX_DROPPED     (S/E): " << m_results.ip_statistic_start.tx_dropped << "/" << m_results.ip_statistic_end.tx_dropped << std::endl;
            filestream << "[MT] IP TX_CARRIER     (S/E): " << m_results.ip_statistic_start.tx_carrier << "/" << m_results.ip_statistic_end.tx_carrier << std::endl;
            filestream << "[MT] IP TX_COLLISIONS  (S/E): " << m_results.ip_statistic_start.tx_collisions << "/" << m_results.ip_statistic_end.tx_collisions << std::endl;
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

    m_stress_ptr->stop();
    write_log();

    return true;
}

void test_scenario_client::write_log(bool error, std::string error_message) {
    test_scenario::write_log();

    if(m_results.status != m_results.STATUS_SUCCESS) {
        std::string filepath = std::string(m_description.metadata.path) + "/" + get_type() + "_";
        std::ofstream filestream(filepath);
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

    m_stress_ptr->stop();
    write_log();

    return true;
}
