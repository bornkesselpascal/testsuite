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
        break;
    }
    }

    pugi::xml_node ethtool_start = root.append_child("ethtool_statistic_start");
    ethtool_start.append_attribute("rx_packets") = results.ethtool_statistic_start.rx_packets;
    ethtool_start.append_attribute("tx_packets") = results.ethtool_statistic_start.tx_packets;
    ethtool_start.append_attribute("rx_dropped") = results.ethtool_statistic_start.rx_dropped;
    ethtool_start.append_attribute("tx_dropped") = results.ethtool_statistic_start.tx_dropped;
    ethtool_start.append_attribute("collisions") = results.ethtool_statistic_start.collisions;
    ethtool_start.append_attribute("port_tx_errors") = results.ethtool_statistic_start.port_tx_errors;
    ethtool_start.append_attribute("port_rx_dropped") = results.ethtool_statistic_start.port_rx_dropped;
    ethtool_start.append_attribute("port_tx_dropped_link_down") = results.ethtool_statistic_start.port_tx_dropped_link_down;

    pugi::xml_node ethtool_end = root.append_child("ethtool_statistic_end");
    ethtool_end.append_attribute("rx_packets") = results.ethtool_statistic_end.rx_packets;
    ethtool_end.append_attribute("tx_packets") = results.ethtool_statistic_end.tx_packets;
    ethtool_end.append_attribute("rx_dropped") = results.ethtool_statistic_end.rx_dropped;
    ethtool_end.append_attribute("tx_dropped") = results.ethtool_statistic_end.tx_dropped;
    ethtool_end.append_attribute("collisions") = results.ethtool_statistic_end.collisions;
    ethtool_end.append_attribute("port_tx_errors") = results.ethtool_statistic_end.port_tx_errors;
    ethtool_end.append_attribute("port_rx_dropped") = results.ethtool_statistic_end.port_rx_dropped;
    ethtool_end.append_attribute("port_tx_dropped_link_down") = results.ethtool_statistic_end.port_tx_dropped_link_down;

    pugi::xml_node ip_start = root.append_child("ip_statistic_start");
    ip_start.append_attribute("mtu") = results.ip_statistic_start.mtu;
    ip_start.append_attribute("rx_bytes") = results.ip_statistic_start.rx_bytes;
    ip_start.append_attribute("rx_packets") = results.ip_statistic_start.rx_packets;
    ip_start.append_attribute("rx_errors") = results.ip_statistic_start.rx_errors;
    ip_start.append_attribute("rx_dropped") = results.ip_statistic_start.rx_dropped;
    ip_start.append_attribute("rx_missed") = results.ip_statistic_start.rx_missed;
    ip_start.append_attribute("rx_mcast") = results.ip_statistic_start.rx_mcast;
    ip_start.append_attribute("tx_bytes") = results.ip_statistic_start.tx_bytes;
    ip_start.append_attribute("tx_packets") = results.ip_statistic_start.tx_packets;
    ip_start.append_attribute("tx_errors") = results.ip_statistic_start.tx_errors;
    ip_start.append_attribute("tx_dropped") = results.ip_statistic_start.tx_dropped;
    ip_start.append_attribute("tx_carrier") = results.ip_statistic_start.tx_carrier;
    ip_start.append_attribute("tx_collisions") = results.ip_statistic_start.tx_collisions;

    pugi::xml_node ip_end = root.append_child("ip_statistic_end");
    ip_end.append_attribute("mtu") = results.ip_statistic_end.mtu;
    ip_end.append_attribute("rx_bytes") = results.ip_statistic_end.rx_bytes;
    ip_end.append_attribute("rx_packets") = results.ip_statistic_end.rx_packets;
    ip_end.append_attribute("rx_errors") = results.ip_statistic_end.rx_errors;
    ip_end.append_attribute("rx_dropped") = results.ip_statistic_end.rx_dropped;
    ip_end.append_attribute("rx_missed") = results.ip_statistic_end.rx_missed;
    ip_end.append_attribute("rx_mcast") = results.ip_statistic_end.rx_mcast;
    ip_end.append_attribute("tx_bytes") = results.ip_statistic_end.tx_bytes;
    ip_end.append_attribute("tx_packets") = results.ip_statistic_end.tx_packets;
    ip_end.append_attribute("tx_errors") = results.ip_statistic_end.tx_errors;
    ip_end.append_attribute("tx_dropped") = results.ip_statistic_end.tx_dropped;
    ip_end.append_attribute("tx_carrier") = results.ip_statistic_end.tx_carrier;
    ip_end.append_attribute("tx_collisions") = results.ip_statistic_end.tx_collisions;


    if(!doc.save_file(filename.c_str())) {
        throw std::runtime_error("[tr_parser] E02 - Error while saving XML file.");
    }
}
