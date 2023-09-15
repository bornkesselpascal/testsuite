#ifndef STRESS_H
#define STRESS_H

#include <string>
#include "test_description.h"

class stress
{
public:
    stress(test_description description, bool start_recording = true);
    ~stress();
    void stop();

private:
    test_description m_description;

    pid_t m_pid_stress;
    pid_t m_pid_nmon;
};

#endif // STRESS_H
