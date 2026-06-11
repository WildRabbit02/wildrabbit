cmake_minimum_required(VERSION 3.10)

# Find GoogleTest
find_package(GTest REQUIRED)

# Test executables
add_executable(test_ec_node_wrapper test_ec_node_wrapper.cpp)
target_link_libraries(test_ec_node_wrapper 
    GTest::GTest 
    GTest::Main 
    ec_visualizer_core
    ${CYBER_LIBRARIES}
)

add_executable(test_channel_monitor test_channel_monitor.cpp)
target_link_libraries(test_channel_monitor 
    GTest::GTest 
    GTest::Main 
    ec_visualizer_core
)

# Register tests
add_test(NAME EcNodeWrapperTest COMMAND test_ec_node_wrapper)
add_test(NAME ChannelMonitorTest COMMAND test_channel_monitor)
