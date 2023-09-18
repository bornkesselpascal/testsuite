#ifndef TEST_RESULTS_H
#define TEST_RESULTS_H

#include <string>
#include "metrics.h"

struct test_results {
    enum status {
        STATUS_FAIL,
        STATUS_SUCCESS,
        STATUS_UNKNOWN,
    } status = test_results::STATUS_UNKNOWN;

    struct iperf {
        std::string interval;
        std::string transfer;
        std::string bandwidth;
        std::string jitter;
        unsigned long int num_loss = 0;
        unsigned long int num_total = 0;

        std::string startup_message;
    } iperf;

    struct custom {
        long num_loss = 0;
    } custom;

    ethtool_statistic ethtool_statistic_start;
    ethtool_statistic ethtool_statistic_end;

    ip_statistic ip_statistic_start;
    ip_statistic ip_statistic_end;
};

#endif // TEST_RESULTS_H
