#include "custom_tester.h"
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
enum type
{
    CLIENT,
    SERVER
};

bool get_user_input(type &type, mode &mode);
void start_client(std::string path);
void start_server(std::string path);
void parallel_execution(type type, std::vector<std::string> filepaths);

int main()
{
    std::cout << "TestSuite (v" << COMMIT_HASH << ")" << std::endl
              << std::endl;

    type type;
    mode mode;
    if (!get_user_input(type, mode))
    {
        return 1;
    }

    switch (type)
    {
    case CLIENT:
    {
        std::string filename = "/testsuite/client_master.xml";
        std::vector<std::string> filepaths = test_control_parser::read_client_main_XML(filename);

        switch (mode)
        {
        case CLASSIC:
        {
            if (filepaths.size() > 0)
            {
                start_client(filepaths[0]);
            }
            break;
        }
        case PARALLEL:
        {
            parallel_execution(type, filepaths);
            break;
        }
        }

        break;
    }
    case SERVER:
    {
        std::string filename = "/testsuite/server_master.xml";
        std::vector<std::string> filepaths = test_control_parser::read_server_main_XML(filename);

        switch (mode)
        {
        case CLASSIC:
        {
            if (filepaths.size() > 0)
            {
                start_server(filepaths[0]);
            }
            break;
        }
        case PARALLEL:
        {
            parallel_execution(type, filepaths);
            break;
        }
        }

        break;
    }
    }
}

bool get_user_input(type &type, mode &mode)
{
    // ASK FOR CLIENT OR SERVER
    std::cout << "Do you want to start a Client or Server? (c/s): ";
    char input;
    std::cin >> input;
    if (input == 'c')
    {
        type = CLIENT;
    }
    else if (input == 's')
    {
        type = SERVER;
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

    return true;
}

void start_client(std::string path)
{
    int process_id = getpid();
    std::cout << "Launched client... [" << std::to_string(process_id) << "][" << path << "]" << std::endl;

    client_description current_description = test_control_parser::read_client_from_XML(path);
    test_control_client current_client(current_description);
    current_client.run();
}

void start_server(std::string path)
{
    int process_id = getpid();
    std::cout << "Launched server... [" << std::to_string(process_id) << "][" << path << "]" << std::endl;

    server_description current_description = test_control_parser::read_server_from_XML(path);
    test_control_server current_server(current_description);
    current_server.run();
}

void parallel_execution(type type, std::vector<std::string> filepaths)
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
            case CLIENT:
            {
                start_client(filepaths[i]);
                break;
            }
            case SERVER:
            {
                start_server(filepaths[i]);
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
