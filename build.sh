#!/bin/bash

# Apollo Cyber Visualizer 编译脚本

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"

echo "=== Apollo Cyber Visualizer Build Script ==="
echo "Build directory: ${BUILD_DIR}"

# 创建构建目录
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# 配置 CMake
echo "Configuring CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local

# 编译
echo "Building..."
make -j$(nproc)

echo ""
echo "=== Build Complete ==="
echo "Executable: ${BUILD_DIR}/cyber_visualizer"
echo ""
echo "Usage:"
echo "  ${BUILD_DIR}/cyber_visualizer --help"
echo "  ${BUILD_DIR}/cyber_visualizer --list-channels"
echo "  ${BUILD_DIR}/cyber_visualizer -i /camera/channel -p /lidar/channel"
echo "  ${BUILD_DIR}/cyber_visualizer --rtps-enabled -m 239.255.0.1"
