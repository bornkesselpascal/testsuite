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
            if(-1 == execlp("/usr/bin/nmon", "/usr/bin/nmon", "-F", (std::string(description.metadata.path) + "/" + std::string(description.metadata.t_uid) + "/system_log.nmon").c_str(), "-s", "1", nullptr)) {
                throw std::runtime_error("[stress] Could not launch nmon. execlp failed.");
            }

            return;
        }
    }

    if(description.stress.num > 0) {
        pid_t pid_stress = fork();

        if(-1 == pid_stress) {
            throw std::runtime_error("[stress] Could not fork.");
        }
        else if(0 == pid_stress) {
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
                ret = execlp("/usr/bin/stress-ng", "/usr/bin/stress-ng", "--cpu", std::to_string(description.stress.num).c_str(), "-t", std::to_string(duration).c_str(), "--sched", "fifo", "--sched-prio", "99", nullptr);
                break;
            case test_description::stress::type::MEMORY:
                ret = execlp("/usr/bin/stress-ng", "/usr/bin/stress-ng", "--bigheap", std::to_string(description.stress.num).c_str(), "-t", std::to_string(duration).c_str(), nullptr);
                break;
            case test_description::stress::type::IO:
                ret = execlp("/usr/bin/stress-ng", "/usr/bin/stress-ng", "--hdd", std::to_string(description.stress.num).c_str(), "-t", std::to_string(duration).c_str(), nullptr);
                break;
            }

            if(-1 == ret) {
                throw std::runtime_error("[stress] Could not launch stress-ng. execlp failed.");
            }

            return;
        }
    }
}

stress::~stress() {
    stop();
}

void stress::stop()
{   
    system("killall stress-ng");
    system("killall nmon");
}
