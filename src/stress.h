#ifndef STRESS_H
#define STRESS_H

#include <memory>
#include <string>
#include <thread>
#include "test_description.h"

class stress
{
public:
    stress(test_description description, bool start_recording = true);
    ~stress();
    void stop();

private:
    stress_type m_type;
};

#endif // STRESS_H
