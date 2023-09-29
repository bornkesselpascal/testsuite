#include "stress.h"
#include <iostream>
#include <unistd.h>
#include <signal.h>

stress::stress(test_description description, bool start_recording)
{
    if (start_recording) {
        pid_t pid_nmon = fork();

        if(-1 == pid_nmon) {
            throw std::runtime_error("[stress] Could not fork.");
        }
        else if(0 == pid_nmon) {
            pid_t current_pid = getpid();
            system(("chrt -o -p 0 " + std::to_string(current_pid)).c_str());

            if(-1 == execlp("/usr/bin/nmon", "/usr/bin/nmon", "-F", (std::string(description.metadata.path) + "/" + std::string(description.metadata.t_uid) + "/system_log.nmon").c_str(), "-s", "1", nullptr)) {
                throw std::runtime_error("[stress] Could not launch nmon. execlp failed.");
            }

            return;
        }
    }

    if(description.stress.num > 0 && description.stress.type != stress_type::NETWORK) {
        pid_t pid_stress = fork();

        if(-1 == pid_stress) {
            throw std::runtime_error("[stress] Could not fork.");
        }
        else if(0 == pid_stress) {
            pid_t current_pid = getpid();
            system(("chrt -o -p 0 " + std::to_string(current_pid)).c_str());

            int ret = 0;
            int duration = (description.duration == -1) ? 68400 : (description.duration+5);

            switch(description.stress.type) {
            case test_description::stress::type::CPU_USR:
                ret = execlp("/usr/bin/stress-ng", "/usr/bin/stress-ng", "--cpu", std::to_string(description.stress.num).c_str(), "-t", std::to_string(duration).c_str(), nullptr);
                break;
            case test_description::stress::type::CPU_KERNEL:
                ret = execlp("/usr/bin/stress-ng", "/usr/bin/stress-ng", "--get", std::to_string(description.stress.num).c_str(), "-t", std::to_string(duration).c_str(), nullptr);
                break;
            case test_description::stress::type::CPU_REALTIME:
                ret = execlp("/usr/bin/stress-ng", "/usr/bin/stress-ng", "--cpu", std::to_string(description.stress.num).c_str(), "-t", std::to_string(duration).c_str(), "--sched", "fifo", "--sched-prio", "50", nullptr);
                break;
            case test_description::stress::type::MEMORY:
                ret = execlp("/usr/bin/stress-ng", "/usr/bin/stress-ng", "--bigheap", std::to_string(description.stress.num).c_str(), "-t", std::to_string(duration).c_str(), nullptr);
                break;
            case test_description::stress::type::IO:
                ret = execlp("/usr/bin/stress-ng", "/usr/bin/stress-ng", "--hdd", std::to_string(description.stress.num).c_str(), "-t", std::to_string(duration).c_str(), nullptr);
                break;
            case test_description::stress::type::TIMER:
                ret = execlp("/usr/bin/stress-ng", "/usr/bin/stress-ng", "--timer", std::to_string(description.stress.num).c_str(), "-t", std::to_string(duration).c_str(), nullptr);
                break;
            }

            if(-1 == ret) {
                throw std::runtime_error("[stress] Could not launch stress-ng. execlp failed.");
            }

            return;
        }
    }
    else if(description.stress.type == stress_type::NETWORK) {
//        pid_t pid_stress = fork();

//        if(-1 == pid_stress) {
//            throw std::runtime_error("[stress] Could not fork.");
//        }
//        else if(0 == pid_stress) {
//            pid_t current_pid = getpid();
//            system(("chrt -o -p 0 " + std::to_string(current_pid)).c_str());

//            stressor_network = std::unique_ptr<custom_stressor_network>(new custom_stressor_network("/testsuite/custom_stressor/network.xml", (description.duration+2), description.stress.num, description.stress.location));
//            stressor_network->start();
//        }
    }
}

stress::~stress() {
    stop();
}

void stress::stop()
{
    system("killall stress-ng");
    system("killall nmon");

    if(stressor_network != nullptr) {
        stressor_network->stop();
        stressor_network.release();
    }
}
