#include "test_control.h"
#include <iostream>
#include <unistd.h>
#include <fstream>
#include "test_control_client.h"
#include "test_control_server.h"

#include <iostream>
#include <fstream>
#include <string>

#ifndef COMMIT_HASH
#define COMMIT_HASH "0000000"
#endif

enum mode
{
    CLASSIC,
    PARALLEL
};
enum affinity
{
    DISABLED,
    ENABLED,
    INVERSED
};

bool get_user_input(testsuite_type &type, mode &mode, bool &realtime, affinity &affinity);
void start_client(std::string path, affinity affinity);
void start_server(std::string path, affinity affinity);
void parallel_execution(testsuite_type type, std::vector<std::string> filepaths, affinity affinity);
std::string select_path(const std::vector<std::string>& filepaths);

int main()
{
    std::cout << "TestSuite | TestSuite Coordinator (v_" << COMMIT_HASH << ")[" << getpid() << "]" << std::endl
              << std::endl;

    if(0 != geteuid()) {
        std::cerr << "[error][coordinator#1] Please run TestSuite with root privileges." << std::endl;
        return 1;
    }

    testsuite_type type;
    mode mode;
    bool realtime;
    affinity affinity;
    if (!get_user_input(type, mode, realtime, affinity))
    {
        std::cerr << std::endl << "[error][coordinator#2] Invalid user input." << std::endl;
        return 1;
    }

    pid_t pid = getpid();
    if(realtime) {
        if(0 != system(("chrt -f -p 99 " + std::to_string(pid)).c_str())) {
            return 1;
        }
    }
    else {
        if(0 != system(("chrt -o -p 0 " + std::to_string(pid)).c_str())) {
            return 1;
        }
    }

    switch (type)
    {
    case testsuite_type::CLIENT:
    {
        std::string filename = "/testsuite/config/client.xml";
        std::vector<std::string> filepaths = test_control_parser::read_main_XML(filename, testsuite_type::CLIENT);

        switch (mode)
        {
        case CLASSIC:
        {
            start_client(select_path(filepaths), affinity);
            break;
        }
        case PARALLEL:
        {
            parallel_execution(type, filepaths, affinity);
            break;
        }
        }

        break;
    }
    case testsuite_type::SERVER:
    {
        std::string filename = "/testsuite/config/server.xml";
        std::vector<std::string> filepaths = test_control_parser::read_main_XML(filename, testsuite_type::SERVER);

        switch (mode)
        {
        case CLASSIC:
        {
            start_server(select_path(filepaths), affinity);
            break;
        }
        case PARALLEL:
        {
            parallel_execution(type, filepaths, affinity);
            break;
        }
        }

        break;
    }
    }

    sleep(1);
    std::cout << "Press [control][c] to end TestSuite Coordinator...";
    char input;
    std::cin >> input;
    return 0;
}

bool get_user_input(testsuite_type &type, mode &mode, bool &realtime, affinity &affinity)
{
    // ASK FOR CLIENT OR SERVER
    std::cout << "Do you want to start a Client or Server? (c/s): ";
    char input;
    std::cin >> input;
    if (input == 'c')
    {
        type = testsuite_type::CLIENT;
    }
    else if (input == 's')
    {
        type = testsuite_type::SERVER;
    }
    else
    {
        return false;
    }

    // ASK FOR MODE (CLASSIC OR PARALLEL)
    std::cout << "Do you want to start in Classic or Parallel mode? (c/p): ";
    std::cin >> input;
    if (input == 'c')
    {
        mode = CLASSIC;
    }
    else if (input == 'p')
    {
        mode = PARALLEL;
    }
    else
    {
        return false;
    }

    // ASK FOR REALTIME
    std::cout << "Do you want to start the tests as real time processes? (y/n): ";
    std::cin >> input;
    if (input == 'y')
    {
        realtime = true;
    }
    else if (input == 'n')
    {
        realtime = false;
    }
    else
    {
        return false;
    }

    // ASK FOR AFFINITY
    std::cout << "Do you want to enable cpu affinity? (d/e/i): ";
    std::cin >> input;
    if (input == 'd')
    {
        affinity = DISABLED;
    }
    else if (input == 'e')
    {
        affinity = ENABLED;
    }
    else if (input == 'i')
    {
        affinity = INVERSED;
    }
    else
    {
        return false;
    }

    return true;
}

void start_client(std::string path, affinity affinity)
{   
    int process_id = getpid();

    std::string cores = "0-32";
    if(affinity != DISABLED) {
        if((path.find('1') != std::string::npos) || (path.find('4') != std::string::npos)) {
            if(affinity == ENABLED) {
                cores = "8-15,24-31"; // Socket 2
            }
            else if (affinity == INVERSED) {
                cores = "0-7,16-23"; // Socket 1
            }
        }
        else if((path.find('2') != std::string::npos) || (path.find('3') != std::string::npos)) {
            if(affinity == ENABLED) {
                cores = "0-7,16-23"; // Socket 1
            }
            else if (affinity == INVERSED) {
                cores = "8-15,24-31"; // Socket 2
            }
        }
        else {
            std::cerr << "[error][coordinator#3] Could not determine cores." << std::endl;
            return;
        }

        if(0 != system(("taskset -cp " + cores + " " + std::to_string(process_id)).c_str())) {
            return;
        }
    }

    std::cout << "Launched client... [" << std::to_string(process_id) << "][" << path << "] on Cores " << cores << std::endl;

    client_description current_description = test_control_parser::read_client_from_XML(path);
    test_control_client current_client(current_description);
    current_client.run();
}

void start_server(std::string path, affinity affinity)
{
    int process_id = getpid();

    std::string cores = "0-32";
    if(affinity != DISABLED) {
        if((path.find('1') != std::string::npos) || (path.find('4') != std::string::npos)) {
            if(affinity == ENABLED) {
                cores = "8-15,24-31"; // Socket 2
            }
            else if (affinity == INVERSED) {
                cores = "0-7,16-23"; // Socket 1
            }
        }
        else if((path.find('2') != std::string::npos) || (path.find('3') != std::string::npos)) {
            if(affinity == ENABLED) {
                cores = "0-7,16-23"; // Socket 1
            }
            else if (affinity == INVERSED) {
                cores = "8-15,24-31"; // Socket 2
            }
        }
        else {
            std::cerr << "[error][coordinator#3] Could not determine cores." << std::endl;
            return;
        }

        if(0 != system(("taskset -cp " + cores + " " + std::to_string(process_id)).c_str())) {
            return;
        }
    }

    std::cout << "Launched server... [" << std::to_string(process_id) << "][" << path << "] on Cores " << cores << std::endl;

    server_description current_description = test_control_parser::read_server_from_XML(path);
    test_control_server current_server(current_description);
    current_server.run();
}

void parallel_execution(testsuite_type type, std::vector<std::string> filepaths, affinity affinity)
{
    // Iterate through all client descriptions with index i
    for (int i = 0; i < filepaths.size(); i++)
    {
        switch (fork())
        {
        case -1:
        {
            // Error
            exit(1);
        }
        case 0:
        {
            switch (type)
            {
            case testsuite_type::CLIENT:
            {
                start_client(filepaths[i], affinity);
                break;
            }
            case testsuite_type::SERVER:
            {
                start_server(filepaths[i], affinity);
                break;
            }
            }
            exit(0);
        }
        default:
        {
            // Parent
            break;
        }
        }
    }
}

std::string select_path(const std::vector<std::string>& filepaths)
{
    int choice = 0;

    // Display the paths with numbers
    for (size_t i = 0; i < filepaths.size(); i++)
    {
        std::cout << (i + 1) << " : " << filepaths[i] << std::endl;
    }

    // Ask for the user input
    std::cout << "Description number: " << std::endl;
    std::cin >> choice;

    // Validate the choice
    if (choice > 0 && choice <= filepaths.size())
    {
        return filepaths[choice - 1];
    }
    else
    {
        return filepaths[0]; // Invalid choice defaults 0.
    }
}
