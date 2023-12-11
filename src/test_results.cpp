#include "test_results.h"
#include "pugixml.hpp"

void test_results_parser::write_to_XML(std::string filename, test_description& description, test_results& results, std::string type) {
    pugi::xml_document doc;

    pugi::xml_node root = doc.append_child("test_results");

    switch(results.status) {
    case test_results::STATUS_FAIL: {
        root.append_child("status").text() = "STATUS_FAIL";
        break;
    }
    case test_results::STATUS_SUCCESS: {
        root.append_child("status").text() = "STATUS_SUCCESS";
        break;
    }
    case test_results::STATUS_UNKNOWN: {
        root.append_child("status").text() = "STATUS_UNKNOWN";
        break;
    }
    }

    pugi::xml_node custom_node = root.append_child("custom");
    custom_node.append_child("num_total").text() = std::to_string(results.custom.num_total).c_str();
    custom_node.append_child("num_misses").text() = std::to_string(results.custom.num_misses).c_str();
    custom_node.append_child("elapsed_time").text() = std::to_string(results.custom.elapsed_time).c_str();

    if(description.latency_measurement != test_description::latency_measurement::DISABLED) {
        pugi::xml_node timestamp_node = custom_node.append_child("timestamp");
        for(timestamp_record &record : results.custom.timestamps) {
            pugi::xml_node record_node = timestamp_node.append_child("record");

            record_node.append_child("sequence").text() = std::to_string(record.sequence_number).c_str();

            if (type == "client") {
                pugi::xml_node snt_prog =  record_node.append_child("snt_prog");
                snt_prog.append_child("tv_sec").text() = std::to_string(record.m_snt_program.tv_sec).c_str();
                snt_prog.append_child("tv_nsec").text() = std::to_string(record.m_snt_program.tv_nsec).c_str();
            }
            else if (type == "server") {
                if (description.latency_measurement == test_description::latency_measurement::FULL) {
                    pugi::xml_node rec_sw =  record_node.append_child("rec_sw");
                    rec_sw.append_child("tv_sec").text() = std::to_string(record.m_rec_sw.tv_sec).c_str();
                    rec_sw.append_child("tv_nsec").text() = std::to_string(record.m_rec_sw.tv_nsec).c_str();
                }

                pugi::xml_node rec_prog =  record_node.append_child("rec_prog");
                rec_prog.append_child("tv_sec").text() = std::to_string(record.m_rec_program.tv_sec).c_str();
                rec_prog.append_child("tv_nsec").text() = std::to_string(record.m_rec_program.tv_nsec).c_str();
            }
        }
    }

    pugi::xml_node ethtool_node = root.append_child("ethtool_statistic");
    pugi::xml_node ethtool_rx_packets_node = ethtool_node.append_child("rx_packets");
    ethtool_rx_packets_node.append_child("start").text() = std::to_string(results.ethtool_statistic_start.rx_packets).c_str();
    ethtool_rx_packets_node.append_child("end").text() = std::to_string(results.ethtool_statistic_end.rx_packets).c_str();
    pugi::xml_node ethtool_tx_packets_node = ethtool_node.append_child("tx_packets");
    ethtool_tx_packets_node.append_child("start").text() = std::to_string(results.ethtool_statistic_start.tx_packets).c_str();
    ethtool_tx_packets_node.append_child("end").text() = std::to_string(results.ethtool_statistic_end.tx_packets).c_str();
    pugi::xml_node ethtool_rx_dropped_node = ethtool_node.append_child("rx_dropped");
    ethtool_rx_dropped_node.append_child("start").text() = std::to_string(results.ethtool_statistic_start.rx_dropped).c_str();
    ethtool_rx_dropped_node.append_child("end").text() = std::to_string(results.ethtool_statistic_end.rx_dropped).c_str();
    pugi::xml_node ethtool_tx_dropped_node = ethtool_node.append_child("tx_dropped");
    ethtool_tx_dropped_node.append_child("start").text() = std::to_string(results.ethtool_statistic_start.tx_dropped).c_str();
    ethtool_tx_dropped_node.append_child("end").text() = std::to_string(results.ethtool_statistic_end.tx_dropped).c_str();
    pugi::xml_node ethtool_collisions_node = ethtool_node.append_child("collisions");
    ethtool_collisions_node.append_child("start").text() = std::to_string(results.ethtool_statistic_start.collisions).c_str();
    ethtool_collisions_node.append_child("end").text() = std::to_string(results.ethtool_statistic_end.collisions).c_str();
    pugi::xml_node ethtool_port_tx_errors_node = ethtool_node.append_child("port_tx_errors");
    ethtool_port_tx_errors_node.append_child("start").text() = std::to_string(results.ethtool_statistic_start.port_tx_errors).c_str();
    ethtool_port_tx_errors_node.append_child("end").text() = std::to_string(results.ethtool_statistic_end.port_tx_errors).c_str();
    pugi::xml_node ethtool_port_rx_dropped_node = ethtool_node.append_child("port_rx_dropped");
    ethtool_port_rx_dropped_node.append_child("start").text() = std::to_string(results.ethtool_statistic_start.port_rx_dropped).c_str();
    ethtool_port_rx_dropped_node.append_child("end").text() = std::to_string(results.ethtool_statistic_end.port_rx_dropped).c_str();
    pugi::xml_node ethtool_port_port_tx_dropped_link_down_node = ethtool_node.append_child("port_tx_dropped_link_down");
    ethtool_port_port_tx_dropped_link_down_node.append_child("start").text() = std::to_string(results.ethtool_statistic_start.port_tx_dropped_link_down).c_str();
    ethtool_port_port_tx_dropped_link_down_node.append_child("end").text() = std::to_string(results.ethtool_statistic_end.port_tx_dropped_link_down).c_str();

    pugi::xml_node ip_node = root.append_child("ip_statistic");
    pugi::xml_node ip_mtu_node = ip_node.append_child("mtu");
    ip_mtu_node.append_child("start").text() = std::to_string(results.ip_statistic_start.mtu).c_str();
    ip_mtu_node.append_child("end").text() = std::to_string(results.ip_statistic_end.mtu).c_str();
    pugi::xml_node ip_rx_bytes_node = ip_node.append_child("rx_bytes");
    ip_rx_bytes_node.append_child("start").text() = std::to_string(results.ip_statistic_start.rx_bytes).c_str();
    ip_rx_bytes_node.append_child("end").text() = std::to_string(results.ip_statistic_end.rx_bytes).c_str();
    pugi::xml_node ip_rx_packets_node = ip_node.append_child("rx_packets");
    ip_rx_packets_node.append_child("start").text() = std::to_string(results.ip_statistic_start.rx_packets).c_str();
    ip_rx_packets_node.append_child("end").text() = std::to_string(results.ip_statistic_end.rx_packets).c_str();
    pugi::xml_node ip_rx_errors_node = ip_node.append_child("rx_errors");
    ip_rx_errors_node.append_child("start").text() = std::to_string(results.ip_statistic_start.rx_errors).c_str();
    ip_rx_errors_node.append_child("end").text() = std::to_string(results.ip_statistic_end.rx_errors).c_str();
    pugi::xml_node ip_rx_dropped_node = ip_node.append_child("rx_dropped");
    ip_rx_dropped_node.append_child("start").text() = std::to_string(results.ip_statistic_start.rx_dropped).c_str();
    ip_rx_dropped_node.append_child("end").text() = std::to_string(results.ip_statistic_end.rx_dropped).c_str();
    pugi::xml_node ip_rx_missed_node = ip_node.append_child("rx_missed");
    ip_rx_missed_node.append_child("start").text() = std::to_string(results.ip_statistic_start.rx_missed).c_str();
    ip_rx_missed_node.append_child("end").text() = std::to_string(results.ip_statistic_end.rx_missed).c_str();
    pugi::xml_node ip_rx_mcast_node = ip_node.append_child("rx_mcast");
    ip_rx_mcast_node.append_child("start").text() = std::to_string(results.ip_statistic_start.rx_mcast).c_str();
    ip_rx_mcast_node.append_child("end").text() = std::to_string(results.ip_statistic_end.rx_mcast).c_str();
    pugi::xml_node ip_tx_bytes_node = ip_node.append_child("tx_bytes");
    ip_tx_bytes_node.append_child("start").text() = std::to_string(results.ip_statistic_start.tx_bytes).c_str();
    ip_tx_bytes_node.append_child("end").text() = std::to_string(results.ip_statistic_end.tx_bytes).c_str();
    pugi::xml_node ip_tx_packets_node = ip_node.append_child("tx_packets");
    ip_tx_packets_node.append_child("start").text() = std::to_string(results.ip_statistic_start.tx_packets).c_str();
    ip_tx_packets_node.append_child("end").text() = std::to_string(results.ip_statistic_end.tx_packets).c_str();
    pugi::xml_node ip_tx_errors_node = ip_node.append_child("tx_errors");
    ip_tx_errors_node.append_child("start").text() = std::to_string(results.ip_statistic_start.tx_errors).c_str();
    ip_tx_errors_node.append_child("end").text() = std::to_string(results.ip_statistic_end.tx_errors).c_str();
    pugi::xml_node ip_tx_dropped_node = ip_node.append_child("tx_dropped");
    ip_tx_dropped_node.append_child("start").text() = std::to_string(results.ip_statistic_start.tx_dropped).c_str();
    ip_tx_dropped_node.append_child("end").text() = std::to_string(results.ip_statistic_end.tx_dropped).c_str();
    pugi::xml_node ip_tx_carrier_node = ip_node.append_child("tx_carrier");
    ip_tx_carrier_node.append_child("start").text() = std::to_string(results.ip_statistic_start.tx_carrier).c_str();
    ip_tx_carrier_node.append_child("end").text() = std::to_string(results.ip_statistic_end.tx_carrier).c_str();
    pugi::xml_node ip_tx_collisions_node = ip_node.append_child("tx_collisions");
    ip_tx_collisions_node.append_child("start").text() = std::to_string(results.ip_statistic_start.tx_collisions).c_str();
    ip_tx_collisions_node.append_child("end").text() = std::to_string(results.ip_statistic_end.tx_collisions).c_str();

    pugi::xml_node netstat_node = root.append_child("netstat_statistic");
    pugi::xml_node netstat_udp_pkg_rec_node = netstat_node.append_child("udp_pkg_rec");
    netstat_udp_pkg_rec_node.append_child("start").text() = std::to_string(results.netstat_statistic_start.udp_pkg_rec).c_str();
    netstat_udp_pkg_rec_node.append_child("end").text() = std::to_string(results.netstat_statistic_end.udp_pkg_rec).c_str();
    pugi::xml_node netstat_udp_pkg_snt_node = netstat_node.append_child("udp_pkg_snt");
    netstat_udp_pkg_snt_node.append_child("start").text() = std::to_string(results.netstat_statistic_start.udp_pkg_snt).c_str();
    netstat_udp_pkg_snt_node.append_child("end").text() = std::to_string(results.netstat_statistic_end.udp_pkg_snt).c_str();
    pugi::xml_node netstat_udp_rec_err_node = netstat_node.append_child("udp_rec_err");
    netstat_udp_rec_err_node.append_child("start").text() = std::to_string(results.netstat_statistic_start.udp_rec_err).c_str();
    netstat_udp_rec_err_node.append_child("end").text() = std::to_string(results.netstat_statistic_end.udp_rec_err).c_str();
    pugi::xml_node netstat_udp_rec_buf_err_node = netstat_node.append_child("udp_rec_buf_err");
    netstat_udp_rec_buf_err_node.append_child("start").text() = std::to_string(results.netstat_statistic_start.udp_rec_buf_err).c_str();
    netstat_udp_rec_buf_err_node.append_child("end").text() = std::to_string(results.netstat_statistic_end.udp_rec_buf_err).c_str();
    pugi::xml_node netstat_udp_snt_buf_err_node = netstat_node.append_child("udp_snt_buf_err");
    netstat_udp_snt_buf_err_node.append_child("start").text() = std::to_string(results.netstat_statistic_start.udp_snt_buf_err).c_str();
    netstat_udp_snt_buf_err_node.append_child("end").text() = std::to_string(results.netstat_statistic_end.udp_snt_buf_err).c_str();

    if(!doc.save_file(filename.c_str())) {
        throw std::runtime_error("[tr_parser] E02 - Error while saving XML file.");
    }
}
