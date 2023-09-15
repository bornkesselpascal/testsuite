#ifndef TEST_SCENARIO_H
#define TEST_SCENARIO_H

#include "iperf.h"
#include <memory>
#include "stress.h"
#include "test_results.h"

class test_scenario
{
public:
    test_scenario(struct test_description description);
    virtual bool start() = 0;
    virtual bool stop()  = 0;
    test_results get_results() { return m_results; }

protected:
    void write_log(bool error = false, std::string error_message = "");
    virtual std::string get_type() const = 0;

    std::unique_ptr<stress> m_stress_ptr;
    test_description m_description;
    test_results m_results;
};

class test_scenario_client : public test_scenario
{
public:
    test_scenario_client(test_description description);
    bool start() override;
    bool stop() override;

protected:
    void write_log(bool error = false, std::string error_message = "");
    virtual std::string get_type() const override { return "client"; }

private:
    std::unique_ptr<iperf_client> m_client_ptr;

};

class test_scenario_server : public test_scenario
{
public:
    test_scenario_server(test_description description, std::shared_ptr<iperf_server> server_ptr);
    bool start() override;
    bool stop() override;

protected:
    virtual std::string get_type() const override { return "server"; }

private:
    std::shared_ptr<iperf_server> m_server_ptr;

};


#endif // TEST_SCENARIO_H
