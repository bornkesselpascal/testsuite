#include "test_control.h"
#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include "test_control_client.h"
#include "test_control_server.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

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
std::vector<std::string> select_path(testsuite_type type);

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

        switch (mode)
        {
        case CLASSIC:
        {
            std::vector<std::string> client_paths = select_path(testsuite_type::CLIENT);

            std::cout << std::endl << std::endl << "PERFORMING THE FOLLOWING TESTS:" << std::endl;
            for (const std::string& client_path: client_paths) {
                std::cout << client_path << std::endl;
            }

            for (const std::string& client_path: client_paths) {
                start_client(client_path, affinity);
                sleep(3);
            }

            break;
        }
        case PARALLEL:
        {
            std::string filename = "/testsuite/config/client.xml";
            std::vector<std::string> filepaths = test_control_parser::read_main_XML(filename, testsuite_type::CLIENT);
            parallel_execution(type, filepaths, affinity);
            break;
        }
        }

        break;
    }
    case testsuite_type::SERVER:
    {
        switch (mode)
        {
        case CLASSIC:
        {
            std::vector<std::string> server_paths = select_path(testsuite_type::SERVER);

            std::cout << std::endl << std::endl << "PERFORMING THE FOLLOWING TESTS:" << std::endl;
            std::string server_path = server_paths.back();
            std::cout << server_path << std::endl;

            start_server(server_path, affinity);
            break;
        }
        case PARALLEL:
        {
            std::string filename = "/testsuite/config/server.xml";
            std::vector<std::string> filepaths = test_control_parser::read_main_XML(filename, testsuite_type::SERVER);
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
        cores = "8-15,24-31";

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
        cores = "8-15";

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

std::vector<std::string> select_path(testsuite_type type)
{
    std::string base_path = "/testsuite/config";
    std::vector<std::string> result;

    // Display all folders in the config directory
    std::vector<std::string> folders;
    for (const auto& entry: std::filesystem::directory_iterator(base_path)) {
        if (entry.is_directory()) {
            folders.push_back(entry.path().filename().string());
        }
    }
    std::sort(folders.begin(), folders.end());
    for (size_t i = 0; i < folders.size(); i++) {
        std::cout << i+1 << " : " << folders[i] << std::endl;
    }

    // Get the user choice
    int choice1 = 0;
    std::cout << "Folder number: ";
    std::cin >> choice1;

    // Validate the choice and ask for XML file
    if ((choice1 > 0 && choice1 <= folders.size()) || choice1 == -1) {
        int displayed_path = (choice1 == -1) ? 0 : (choice1 - 1);

        std::string type_path = (type == testsuite_type::CLIENT) ? "client" : "server";
        std::string folder_path = base_path + "/" + folders[displayed_path] + "/" + type_path;
        std::cout << std::endl << std::endl;

        std::vector<std::string> xml_files;
        for (const auto& entry: std::filesystem::directory_iterator(folder_path)) {
            if (entry.path().extension() == ".xml") {
                xml_files.push_back(entry.path().filename().string());
            }
        }
        std::sort(xml_files.begin(), xml_files.end());
        for (size_t i = 0; i < xml_files.size(); i++) {
            std::cout << i+1 << " : " << xml_files[i] << std::endl;
        }

        // Get the user choice
        int choice2 = 0;
        std::cout << "File number (for " << folder_path <<"): ";
        std::cin >> choice2;

        // Validate the choice
        if (choice2 > 0 && choice2 <= xml_files.size()) {
            if (choice1 == -1) {
                for (const std::string& folder: folders) {
                    result.push_back(base_path + "/" + folder + "/" + type_path + + "/" + xml_files[choice2 - 1]);
                }
            }
            else {
                result.push_back(folder_path + "/" + xml_files[choice2 - 1]);
            }
        }
        else {
            std::cout << std::endl << std::endl << "Invalid choice (" << choice2 << ")." << std::endl << std::endl;
            return select_path(type);
        }
    }
    else {
        std::cout << std::endl << std::endl << "Invalid choice." << std::endl << std::endl;
        return select_path(type);
    }

    return result;
}
