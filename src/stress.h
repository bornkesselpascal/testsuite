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
};

#endif // STRESS_H
