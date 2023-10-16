#include "test_results.h"
#include "pugixml.hpp"

void test_results_parser::write_to_XML(std::string filename, test_results &results, enum test_description::metadata::method method) {
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

    switch(method) {
    case test_description::metadata::IPERF: {
        pugi::xml_node iperf_node = root.append_child("iperf");
        iperf_node.append_child("interval").text() = results.iperf.interval.c_str();
        iperf_node.append_child("transfer").text() = results.iperf.transfer.c_str();
        iperf_node.append_child("bandwidth").text() = results.iperf.bandwidth.c_str();
        iperf_node.append_child("jitter").text() = results.iperf.jitter.c_str();
        iperf_node.append_child("num_loss").text() = std::to_string(results.iperf.num_loss).c_str();
        iperf_node.append_child("num_total").text() = std::to_string(results.iperf.num_total).c_str();
        iperf_node.append_child("startup_message").text() = results.iperf.startup_message.c_str();
        break;
    }
    case test_description::metadata::CUSTOM: {
        pugi::xml_node custom_node = root.append_child("custom");
        custom_node.append_child("num_loss").text() = std::to_string(results.custom.num_loss).c_str();
        custom_node.append_child("num_total").text() = std::to_string(results.custom.num_total).c_str();
        custom_node.append_child("num_misses").text() = std::to_string(results.custom.num_misses).c_str();
        custom_node.append_child("elapsed_time").text() = std::to_string(results.custom.elapsed_time).c_str();

        pugi::xml_node query_node = custom_node.append_child("query");
        for(query_report &report : results.custom.query_response) {
            pugi::xml_node report_node = query_node.append_child("report");
            report_node.append_child("misses").text() = std::to_string(report.cur_misses).c_str();
            report_node.append_child("total").text() = std::to_string(report.cur_packages).c_str();
        }

        break;
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


    if(!doc.save_file(filename.c_str())) {
        throw std::runtime_error("[tr_parser] E02 - Error while saving XML file.");
    }
}
