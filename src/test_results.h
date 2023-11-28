#ifndef TEST_RESULTS_H
#define TEST_RESULTS_H

#include <string>
#include <vector>
#include "metrics.h"
#include "test_description.h"

struct timestamp_record {
    long unsigned int sequence_number;
    struct timespec m_snt_program;
    struct timespec m_rec_sw;
    struct timespec m_rec_program;
};

struct test_results {
    enum status {
        STATUS_FAIL,
        STATUS_SUCCESS,
        STATUS_UNKNOWN,
    } status = test_results::STATUS_UNKNOWN;

    struct custom {
        long long int num_total = -1;
        long long int num_misses = -1;
        long double   elapsed_time = -1;
        std::vector<timestamp_record> timestamps;
    } custom;

    ethtool_statistic ethtool_statistic_start;
    ethtool_statistic ethtool_statistic_end;

    ip_statistic ip_statistic_start;
    ip_statistic ip_statistic_end;

    netstat_statistic netstat_statistic_start;
    netstat_statistic netstat_statistic_end;
};



class test_results_parser
{
public:
    static void write_to_XML(std::string filename, test_results &results, std::string type);

    test_results_parser() = delete;
};

#endif // TEST_RESULTS_H
