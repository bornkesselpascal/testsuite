#include "test_control_common.h"
#include <ctime>
#include <chrono>
#include <fstream>


void test_control_common::log_control(client_description description) {
    std::time_t timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string filepath = std::string(description.path) + "/control_log.txt";
    std::ofstream filestream(filepath);

    if(filestream.is_open()) {
        filestream << "[IM] TYPE                   : " << "TEST CONTROL CLIENT LOG" << std::endl;
        filestream << "[IM] STARTUP                : " << std::ctime(&timestamp);
        filestream << std::endl;

        filestream << "[CD] METHOD                 : " << description.method << std::endl;
        filestream << "[CD] PATH                   : " << description.path << std::endl;
        filestream << "[CD] DURATION MIN           : " << description.duration.min_duration << std::endl;
        filestream << "[CD] DURATION MID           : " << description.duration.mid_duration << std::endl;
        filestream << "[CD] DURATION MAX           : " << description.duration.max_duration << std::endl;
        filestream << "[CD] TARGET_CON IP          : " << description.target_connection.ip << std::endl;
        filestream << "[CD] TARGET_CON BANDWIDTH   : " << description.target_connection.bandwidth << std::endl;
        filestream << "[CD] TARGET_CON DATAGRAMSIZE: " << description.target_connection.datagramsize << std::endl;
        filestream << "[CD] INTERFACE CLIENT       : " << description.interface.client << std::endl;
        filestream << "[CD] INTERFACE SERVER       : " << description.interface.server << std::endl;
        filestream << "[CD] SERVIC_CON IP          : " << description.svc_connection.ip << std::endl;
        filestream << "[CD] SERVIC_CON PORT        : " << description.svc_connection.port << std::endl;
        filestream << "[CD] CLIENT_ONLY            : " << (description.client_only ? "TRUE" : "FALSE") << std::endl;
        filestream << "[CD] STRESS TYPE            : " << description.stress.type << std::endl;
        filestream << "[CD] STRESS NUM_MIN         : " << description.stress.num.num_min << std::endl;
        filestream << "[CD] STRESS NUM_MAX         : " << description.stress.num.num_max << std::endl;
        filestream << "[CD] STRESS NUM_STEP        : " << description.stress.num.steps   << std::endl;
        filestream << std::endl << std::endl << std::endl;
    }
    filestream.close();
}

void test_control_common::log_control(server_description description) {
    std::time_t timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string filepath = std::string(description.path) + "/control_log.txt";
    std::ofstream filestream(filepath);

    if(filestream.is_open()) {
        filestream << "[IM] TYPE                   : " << "TEST CONTROL SERVER LOG" << std::endl;
        filestream << "[IM] STARTUP                : " << std::ctime(&timestamp);
        filestream << std::endl;

        filestream << "[SD] METHOD                 : " << description.method << std::endl;
        filestream << "[SD] PATH                   : " << description.path << std::endl;
        filestream << "[SD] TARGET_CON DATAGRAMSIZE: " << description.datagramsize << std::endl;
        filestream << "[SD] INTERFACE SERVER       : " << description.interface << std::endl;
        filestream << "[SD] SERVIC_CON IP          : " << description.svc_connection.ip << std::endl;
        filestream << "[SD] SERVIC_CON PORT        : " << description.svc_connection.port << std::endl;
        filestream << std::endl << std::endl << std::endl;
    }
    filestream.close();
}

void test_control_common::log_scenario(test_description description, test_results* results) {
    std::time_t timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string filepath = std::string(description.metadata.path) + "/control_log.txt";
    std::ofstream filestream(filepath, std::ios_base::app);

    if(filestream.is_open()) {
        filestream << "[TC] SCENARIO T_UID         : " << description.metadata.t_uid << std::endl;
        filestream << "[TC] END TIME               : " << std::ctime(&timestamp);

        filestream << "[TD] STRESS (TYPE/NUM)      : " << description.stress.type << "/" << description.stress.num << std::endl;
        filestream << "[TD] DURATION               : " << description.duration << std::endl;

        if(results != nullptr) {
            switch(description.metadata.method) {
            case test_description::metadata::IPERF:
                filestream << "[RE] STATUS                 : " << results->status << std::endl;
                filestream << "[RE] RESULT (LOSS/TOTAL)    : " << results->iperf.num_loss << "/" << results->iperf.num_total << std::endl;
                break;

            case test_description::metadata::CTEST:
                filestream << "[RE] STATUS                 : " << results->status << std::endl;
                filestream << "[RE] RESULT (LOSS)          : " << results->custom.num_loss << std::endl;
                break;
            }
        }

        filestream << std::endl << std::endl << std::endl;
    }
    filestream.close();
}
