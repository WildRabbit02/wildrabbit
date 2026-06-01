#!/bin/bash
# test_runner.sh - 运行所有单元测试
# Version: 2.0.0

set -e

echo "=========================================="
echo "Apollo Cyber Visualizer v2.0.0 - 测试套件"
echo "=========================================="

BUILD_DIR="build_test"
TEST_BINARY_DIR="${BUILD_DIR}/tests"

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 检查依赖
check_dependencies() {
    echo -e "${YELLOW}检查依赖项...${NC}"
    
    if ! command -v cmake &> /dev/null; then
        echo -e "${RED}错误：未找到 cmake${NC}"
        exit 1
    fi
    
    if ! command -v g++ &> /dev/null; then
        echo -e "${RED}错误：未找到 g++${NC}"
        exit 1
    fi
    
    # 检查 GoogleTest
    if ! pkg-config --exists gtest 2>/dev/null; then
        echo -e "${YELLOW}警告：未通过 pkg-config 找到 gtest，尝试继续...${NC}"
    fi
    
    echo -e "${GREEN}依赖检查完成${NC}"
}

# 创建测试构建目录
setup_build() {
    echo -e "${YELLOW}设置测试构建环境...${NC}"
    
    if [ -d "${BUILD_DIR}" ]; then
        echo "清理旧的构建目录..."
        rm -rf "${BUILD_DIR}"
    fi
    
    mkdir -p "${BUILD_DIR}"
    cd "${BUILD_DIR}"
    
    echo "配置 CMake (启用测试)..."
    cmake .. -DBUILD_TESTING=ON
    
    cd ..
    echo -e "${GREEN}构建环境准备完成${NC}"
}

# 编译项目
build_project() {
    echo -e "${YELLOW}编译项目...${NC}"
    
    cd "${BUILD_DIR}"
    make -j$(nproc)
    cd ..
    
    echo -e "${GREEN}编译完成${NC}"
}

# 运行单元测试
run_tests() {
    echo -e "${YELLOW}运行单元测试...${NC}"
    
    if [ ! -d "${TEST_BINARY_DIR}" ]; then
        echo -e "${RED}错误：测试二进制目录不存在${NC}"
        exit 1
    fi
    
    local test_count=0
    local pass_count=0
    local fail_count=0
    
    # 查找并运行所有测试
    for test_binary in ${TEST_BINARY_DIR}/test_*; do
        if [ -x "$test_binary" ]; then
            test_name=$(basename "$test_binary")
            echo ""
            echo -e "${YELLOW}运行测试：${test_name}${NC}"
            echo "------------------------------------------"
            
            test_count=$((test_count + 1))
            
            if "$test_binary" --gtest_color=yes; then
                pass_count=$((pass_count + 1))
                echo -e "${GREEN}✓ ${test_name} 通过${NC}"
            else
                fail_count=$((fail_count + 1))
                echo -e "${RED}✗ ${test_name} 失败${NC}"
            fi
        fi
    done
    
    echo ""
    echo "=========================================="
    echo "测试结果汇总"
    echo "=========================================="
    echo "总测试数：${test_count}"
    echo -e "${GREEN}通过：${pass_count}${NC}"
    if [ ${fail_count} -gt 0 ]; then
        echo -e "${RED}失败：${fail_count}${NC}"
        exit 1
    else
        echo -e "${GREEN}全部测试通过！${NC}"
    fi
}

# 运行集成测试（如果有真实 CyberRT 环境）
run_integration_test() {
    echo -e "${YELLOW}运行集成测试（可选）...${NC}"
    
    # 检查是否有运行的 CyberRT 系统
    if command -v cyber_channel &> /dev/null; then
        echo "检测到 CyberRT 环境，运行集成测试..."
        
        # 列出可用通道
        echo "可用通道列表:"
        cyber_channel list
        
        echo -e "${GREEN}集成测试完成${NC}"
    else
        echo -e "${YELLOW}跳过集成测试：未检测到 CyberRT 环境${NC}"
    fi
}

# 清理
cleanup() {
    echo -e "${YELLOW}清理临时文件...${NC}"
    # 可以选择保留构建目录以便调试
    # rm -rf "${BUILD_DIR}"
    echo "构建目录保留在：${BUILD_DIR}"
}

# 主函数
main() {
    local start_time=$(date +%s)
    
    check_dependencies
    setup_build
    build_project
    run_tests
    run_integration_test
    
    local end_time=$(date +%s)
    local duration=$((end_time - start_time))
    
    echo ""
    echo "=========================================="
    echo -e "${GREEN}所有测试完成！耗时：${duration}秒${NC}"
    echo "=========================================="
}

# 执行
main "$@"
