#include "metrics.h"
#include <array>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <memory>
#include <sstream>
#include <regex>

ethtool_statistic metrics::get_ethtool_statistic(std::string interface_name) {
    std::string output = metrics::execute_command("ethtool -S " + interface_name);

    if(output.find("No such device") != std::string::npos) {
        throw std::runtime_error("[metrics] Interface is unknown. Cannot get Metrics for \"ethtool\".");
    }

    ethtool_statistic result;
    std::istringstream output_stream(output);
    std::string output_line;

    while(std::getline(output_stream, output_line)) {
        std::istringstream output_line_stream(output_line);
        std::string current_key;
        int64_t current_value;

        if(output_line_stream >> current_key >> current_value) {
            if(current_key == "rx_packets:")                { result.rx_packets                = current_value; continue; }
            if(current_key == "tx_packets:")                { result.tx_packets                = current_value; continue; }
            if(current_key == "rx_dropped:")                { result.rx_dropped                = current_value; continue; }
            if(current_key == "tx_dropped:")                { result.tx_dropped                = current_value; continue; }
            if(current_key == "collisions:")                { result.collisions                = current_value; continue; }
            if(current_key == "port.tx_errors:")            { result.port_tx_errors            = current_value; continue; }
            if(current_key == "port.rx_dropped:")           { result.port_rx_dropped           = current_value; continue; }
            if(current_key == "port.tx_dropped_link_down:") { result.port_tx_dropped_link_down = current_value; continue; }
        }
    }

    return result;
}

ip_statistic metrics::get_ip_statistic(std::string interface_name) {
    std::string output = metrics::execute_command("ip -s link ls " + interface_name);

    if(output.find("does not exist") != std::string::npos) {
        throw std::runtime_error("[metrics] Interface is unknown. Cannot get Metrics for \"ip -s link\".");
    }

    ip_statistic result;

    // MTU
    if(output.find("mtu") != std::string::npos) {
        size_t mtu_start = output.find("mtu") + 4;
        size_t mtu_end   = output.find(" ", mtu_start);
        result.mtu = std::stoi(output.substr(mtu_start, mtu_end - mtu_start));
    }

    std::istringstream output_stream(output);
    std::string output_line;

    for (int i = 0; i < 3; ++i)
        std::getline(output_stream, output_line);
    output_stream >> result.rx_bytes >> result.rx_packets >> result.rx_errors >> result.rx_dropped >> result.rx_missed >> result.rx_mcast;

    for (int i = 0; i < 2; ++i)
        std::getline(output_stream, output_line);
    output_stream >> result.tx_bytes >> result.tx_packets >> result.tx_errors >> result.tx_dropped >> result.tx_carrier >> result.tx_collisions;

    return result;
}

std::string metrics::execute_command (std::string command)
{
    std::array<char, 128> buffer;
    std::string result;

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if(!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    while(fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return result;
}
