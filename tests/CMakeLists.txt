# 测试子目录 CMakeLists.txt
# Version: 2.0.0

# CyberNodeWrapper 测试
add_executable(test_cyber_node_wrapper test_cyber_node_wrapper.cpp)
target_link_libraries(test_cyber_node_wrapper
    cyber_visualizer
    GTest::gtest_main
    ${CYBER_LIBRARIES}
    pthread
)
add_test(NAME CyberNodeWrapperTest COMMAND test_cyber_node_wrapper)

# ChannelMonitor 测试
add_executable(test_channel_monitor test_channel_monitor.cpp)
target_link_libraries(test_channel_monitor
    cyber_visualizer
    GTest::gtest_main
    pthread
)
add_test(NAME ChannelMonitorTest COMMAND test_channel_monitor)

# 设置测试属性
set_tests_properties(CyberNodeWrapperTest PROPERTIES
    TIMEOUT 60
    LABELS "unit;wrapper"
)

set_tests_properties(ChannelMonitorTest PROPERTIES
    TIMEOUT 60
    LABELS "unit;monitor"
)
