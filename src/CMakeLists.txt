cmake_minimum_required(VERSION 3.5)

project(TestSuite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib/pugixml-1.13)

add_executable(TestSuite testsuite.cpp
    iperf.h iperf.cpp
    stress.h stress.cpp
    test_scenario.h test_scenario.cpp
    metrics.h metrics.cpp
    test_description.h test_description.cpp
    test_results.h test_results.cpp
    test_control_client.h test_control_client.cpp
    communication.h communication.cpp
    test_control_server.h test_control_server.cpp
    test_control.h test_control.cpp
    custom_tester.h custom_tester.cpp
    helpers.h helpers.cpp
    custom_stressor.h custom_stressor.cpp
    )

target_link_libraries(TestSuite PRIVATE pugixml)

include(GNUInstallDirs)
install(TARGETS TestSuite
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
