#ifndef STRESS_H
#define STRESS_H

#include <memory>
#include <string>
#include <thread>
#include "custom_stressor.h"
#include "test_description.h"

class stress
{
public:
    stress(test_description description, bool start_recording = true);
    ~stress();
    void stop();

private:
    std::unique_ptr<custom_stressor_network> stressor_network;
};

#endif // STRESS_H
