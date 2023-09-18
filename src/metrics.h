#ifndef METRICS_H
#define METRICS_H

#include <string>
#include <sys/types.h>

struct ethtool_statistic
{
    int64_t rx_packets                = -1;
    int64_t tx_packets                = -1;
    int32_t rx_dropped                = -1;
    int32_t tx_dropped                = -1;
    int32_t collisions                = -1;
    int32_t port_tx_errors            = -1;
    int32_t port_rx_dropped           = -1;
    int32_t port_tx_dropped_link_down = -1;
};

struct ip_statistic
{
    int16_t mtu           = -1;
    int64_t rx_bytes      = -1;
    int64_t rx_packets    = -1;
    int32_t rx_errors     = -1;
    int32_t rx_dropped    = -1;
    int32_t rx_missed     = -1;
    int32_t rx_mcast      = -1;
    int64_t tx_bytes      = -1;
    int64_t tx_packets    = -1;
    int32_t tx_errors     = -1;
    int32_t tx_dropped    = -1;
    int32_t tx_carrier    = -1;
    int32_t tx_collisions = -1;
};

class metrics
{
public:
    static ethtool_statistic get_ethtool_statistic(std::string interface_name);
    static      ip_statistic get_ip_statistic     (std::string interface_name);
    static       std::string execute_command      (std::string command);

    metrics() = delete;
};

#endif // METRICS_H
