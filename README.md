# Apollo Cyber Visualizer - Standalone Version

[![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)](https://github.com/apollo/cyber_visualizer)
[![License](https://img.shields.io/badge/license-Apache%202.0-green.svg)](LICENSE)

一个独立的 Apollo CyberRT 数据可视化工具，用于监控和显示实时数据通道。

## v2.0.0 新功能

- ✅ **完整的单元测试套件** - 基于 GoogleTest 的测试框架
- ✅ **自动化测试脚本** - 一键运行所有测试
- ✅ **增强的跨主机支持** - 优化的 RTPS/DDS 配置
- ✅ **改进的频率统计** - 更精确的消息频率计算
- ✅ **CMake 测试集成** - `ctest` 原生支持

## 功能特性

- 订阅并显示实时相机图像（支持多路）
- 订阅并显示实时点云数据
- 通道监控面板（显示所有可用通道及消息频率）
- 跨主机数据订阅支持（通过 RTPS/DDS）
- 基于 Qt + OpenGL 的高性能渲染

## 依赖要求

- Apollo CyberRT (已安装)
- Qt5 (QtWidgets, QtOpenGL)
- OpenGL
- CMake >= 3.10
- GoogleTest (可选，用于单元测试)

## 编译方法

### 标准编译

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### 启用测试编译

```bash
mkdir build && cd build
cmake .. -DBUILD_TESTING=ON
make -j$(nproc)
```

## 使用方法

```bash
# 基本使用
./cyber_visualizer

# 指定订阅的通道
./cyber_visualizer --image_channel /camera/front/image --pcd_channel /lidar/top/pointcloud

# 启用跨主机模式
./cyber_visualizer --rtps_enabled --multicast_ip 239.255.0.1

# 列出所有可用通道
./cyber_visualizer --list_channels
```

## 运行测试

### 方法一：使用测试脚本（推荐）

```bash
chmod +x test_runner.sh
./test_runner.sh
```

### 方法二：使用 CTest

```bash
cd build
ctest --verbose
```

### 方法三：直接运行测试二进制文件

```bash
cd build/tests
./test_cyber_node_wrapper
./test_channel_monitor
```

## 配置说明

### 跨主机配置

在订阅端和发布端的 `cyber_conf.xml` 中确保启用 RTPS 传输：

```xml
<transport_config>
    <rtps_transport>
        <enabled>true</enabled>
        <multicast_ip>239.255.0.1</multicast_ip>
        <port>9090</port>
    </rtps_transport>
</transport_config>
```

### 防火墙设置

```bash
sudo ufw allow 9090/udp
sudo ufw allow from 192.168.1.0/24
```

## 支持的通道类型

- `apollo::drivers::Image` - 相机图像
- `apollo::drivers::PointCloud` - 激光雷达点云
- `apollo::common::Header` - 通道头信息监控

## 项目结构

```
cyber_visualizer/
├── CMakeLists.txt          # 主 CMake 配置
├── README.md               # 项目文档
├── test_runner.sh          # 测试运行脚本
├── include/                # 头文件目录
│   ├── cyber_node_wrapper.h
│   ├── visualizer_window.h
│   ├── image_display.h
│   ├── pointcloud_display.h
│   └── channel_monitor.h
├── src/                    # 源代码目录
│   ├── main.cpp
│   ├── cyber_node_wrapper.cpp
│   ├── visualizer_window.cpp
│   ├── image_display.cpp
│   ├── pointcloud_display.cpp
│   └── channel_monitor.cpp
└── tests/                  # 单元测试目录
    ├── CMakeLists.txt
    ├── test_cyber_node_wrapper.cpp
    └── test_channel_monitor.cpp
```

## 跨主机使用示例

### 场景描述
- 主机 A (192.168.1.108): 运行 `ec_opencv_a9` 容器，发布两路相机图像
- 主机 B (192.168.1.102): 运行 `ec_ds` 容器，发布点云数据
- 主机 C: 运行 cyber_visualizer 进行监控

### 配置步骤

1. **在所有主机上配置 cyber_conf.xml**

2. **开放防火墙端口**
```bash
# 在所有主机上执行
sudo ufw allow 9090/udp
```

3. **启动视觉化器**
```bash
./cyber_visualizer --rtps_enabled \
    --multicast_ip 239.255.0.1 \
    --image_channel /camera/front/image \
    --image_channel /camera/rear/image \
    --pcd_channel /lidar/pointcloud
```

## 版本历史

### v2.0.0 (当前版本)
- 添加完整的单元测试套件
- 新增自动化测试脚本
- 改进 CMake 配置，支持测试构建选项
- 优化跨主机通信性能

### v1.0.0
- 初始版本发布
- 基础图像和点云显示功能
- 通道监控功能

## 许可证

Apache License 2.0