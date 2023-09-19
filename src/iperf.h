#ifndef IPERF_H
#define IPERF_H

#include "custom_tester.h"
#include "test_description.h"
#include "test_results.h"
#include <atomic>
#include <string>
#include <thread>

class iperf
{
public:
    virtual void start() = 0;
    virtual test_results get_results() = 0;
    std::string get_current_output() { return m_iperf_output; }

protected:
    std::string m_iperf_command;
    std::string m_iperf_output;

    test_description m_description;
    test_results m_results;
};

class iperf_client : public iperf
{
public:
    iperf_client(test_description description, bool udp = true);
    void start() override;
    test_results get_results() override;

private:
    std::unique_ptr<std::thread> m_thread_ptr;

    custom_tester_client_description m_custom_description;
};

class iperf_server : public iperf
{
public:
    iperf_server(enum test_description::metadata::method method, int datagramsize, bool udp = false);
    ~iperf_server();
    void start() override;
    void load_test(test_description description);
    test_results get_results() override;

private:
    std::unique_ptr<std::thread> m_thread_ptr;
    bool m_started = false;

    custom_tester_server_description m_custom_description;
};


#endif // IPERF_H
