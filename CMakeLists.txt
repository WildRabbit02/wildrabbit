cmake_minimum_required(VERSION 3.10)
project(cyber_visualizer VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 测试支持选项
option(BUILD_TESTING "Build unit tests" OFF)

# 查找 Apollo CyberRT
find_package(cyber REQUIRED)
find_package(Protobuf REQUIRED)

# 查找 Qt5
find_package(Qt5 REQUIRED COMPONENTS Widgets OpenGL)

# 查找 OpenGL
find_package(OpenGL REQUIRED)

# 包含目录
include_directories(
    ${CYBER_INCLUDE_DIRS}
    ${Protobuf_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5OpenGL_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    include
)

# 源文件
set(SOURCES
    src/main.cpp
    src/visualizer_window.cpp
    src/image_display.cpp
    src/pointcloud_display.cpp
    src/channel_monitor.cpp
    src/cyber_node_wrapper.cpp
)

# 头文件
set(HEADERS
    include/visualizer_window.h
    include/image_display.h
    include/pointcloud_display.h
    include/channel_monitor.h
    include/cyber_node_wrapper.h
)

# 创建可执行文件
add_executable(cyber_visualizer ${SOURCES} ${HEADERS})

# 链接库
target_link_libraries(cyber_visualizer
    ${CYBER_LIBRARIES}
    ${Protobuf_LIBRARIES}
    Qt5::Widgets
    Qt5::OpenGL
    ${OPENGL_LIBRARIES}
    pthread
)

# 安装目标
install(TARGETS cyber_visualizer DESTINATION bin)

# ==================== 测试配置 ====================
if(BUILD_TESTING)
    # 查找 GoogleTest
    find_package(GTest QUIET)
    
    if(NOT GTEST_FOUND)
        # 如果系统未安装 GTest，使用 FetchContent 下载
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.12.1
        )
        FetchContent_MakeAvailable(googletest)
    endif()
    
    # 启用测试
    enable_testing()
    
    # 测试辅助目录
    add_subdirectory(tests)
endif()
