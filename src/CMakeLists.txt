cmake_minimum_required(VERSION 3.5)
project(TestSuite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Git and if we can't find it, disable the related options
find_package(Git QUIET)
if(GIT_FOUND)
  message("Git found: ${GIT_EXECUTABLE}")
  # Get the current commit hash
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  message("Commit_ID: ${GIT_COMMIT_HASH}")
else()
  set(GIT_COMMIT_HASH "0")
endif()

# Pass the commit hash to the compiler
add_definitions("-DCOMMIT_HASH=\"${GIT_COMMIT_HASH}\"")

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
    )

target_link_libraries(TestSuite PRIVATE pugixml)

include(GNUInstallDirs)
install(TARGETS TestSuite
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
