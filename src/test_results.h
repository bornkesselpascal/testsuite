#ifndef TEST_RESULTS_H
#define TEST_RESULTS_H

#include <string>
#include "metrics.h"
#include "test_description.h"

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
        long long int num_loss = -1;
        long long int num_total = -1;

        std::string startup_message;
    } iperf;

    struct custom {
        long long int num_loss = -1;
        long long int num_total = -1;
        long long int num_misses = -1;
        long double   elapsed_time = -1;
    } custom;

    ethtool_statistic ethtool_statistic_start;
    ethtool_statistic ethtool_statistic_end;

    ip_statistic ip_statistic_start;
    ip_statistic ip_statistic_end;
};



class test_results_parser
{
public:
    static void write_to_XML(std::string filename, test_results &results, enum test_description::metadata::method method);

    test_results_parser() = delete;
};

#endif // TEST_RESULTS_H
