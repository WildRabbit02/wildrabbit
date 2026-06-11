cmake_minimum_required(VERSION 3.14)
project(ec_visualizer VERSION 2.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Options
option(BUILD_TESTING "Build unit tests" OFF)
option(BUILD_FOXGLOVE_BRIDGE "Build Foxglove WebSocket bridge" ON)

# CyberRT Configuration
if(DEFINED ENV{CYBER_ROOT})
    set(CYBER_ROOT_DEFAULT $ENV{CYBER_ROOT})
else()
    set(CYBER_ROOT_DEFAULT "/usr/local/cyber")
endif()

set(CYBER_ROOT ${CYBER_ROOT_DEFAULT} CACHE PATH "Path to CyberRT installation")
set(CYBER_INCLUDE_DIR "${CYBER_ROOT}/include" CACHE PATH "CyberRT include directory")
set(CYBER_LIB_DIR "${CYBER_ROOT}/lib" CACHE PATH "CyberRT library directory")

# Find dependencies
find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL REQUIRED)
find_package(Protobuf REQUIRED)
find_package(Eigen3 REQUIRED)

# Include directories
include_directories(
    ${CYBER_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/include
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5OpenGL_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
)

# Link directories
link_directories(${CYBER_LIB_DIR})

# Source files
set(SOURCES
    src/main.cpp
    src/ec_node_wrapper.cpp
    src/visualizer_window.cpp
    src/image_display.cpp
    src/pointcloud_display.cpp
    src/channel_monitor.cpp
)

if(BUILD_FOXGLOVE_BRIDGE)
    list(APPEND SOURCES src/foxglove_bridge.cpp)
    add_definitions(-DENABLE_FOXGLOVE_BRIDGE)
endif()

# Executable
add_executable(ec_visualizer ${SOURCES})

target_link_libraries(ec_visualizer
    cyber
    protobuf-lite
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::OpenGL
    GL
    pthread
)

# Set RPATH
set_target_properties(ec_visualizer PROPERTIES
    BUILD_RPATH "${CYBER_LIB_DIR}"
    INSTALL_RPATH "${CYBER_LIB_DIR}"
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

# Configuration script
configure_file(
    ${PROJECT_SOURCE_DIR}/cmake/ec_visualizer_env.sh.in
    ${PROJECT_BINARY_DIR}/ec_visualizer_env.sh
    @ONLY
)

# Testing
if(BUILD_TESTING)
    enable_testing()
    find_package(GTest REQUIRED)
    add_subdirectory(tests)
endif()

# Install
install(TARGETS ec_visualizer DESTINATION bin)
install(DIRECTORY config/ DESTINATION etc/ec_visualizer)
install(FILES ${PROJECT_BINARY_DIR}/ec_visualizer_env.sh DESTINATION bin)
