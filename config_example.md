# Apollo Cyber Visualizer 配置指南

## 跨主机数据订阅配置

### 场景说明
- 主机 A (192.168.1.108): 运行容器 `ec_opencv_a9`，发布两路相机图像
- 主机 B (192.168.1.102): 运行容器 `ec_ds`，发布实时点云数据
- 主机 C (本机): 运行 `cyber_visualizer` 订阅并显示数据

### 步骤 1: 确认发布端通道名称

在发布端主机上执行：
```bash
# 主机 A - 查看相机通道
docker exec -it ec_opencv_a9 cyber_channel list

# 主机 B - 看点云通道
docker exec -it ec_ds cyber_channel list
```

记录通道名称，例如：
- `/apollo/drivers/camera/front/image`
- `/apollo/drivers/camera/rear/image`
- `/apollo/sensors/lidar/top/pointcloud`

### 步骤 2: 配置 RTPS 传输

在所有主机（发布端和订阅端）的容器中，确保 `cyber_conf.xml` 包含以下配置：

```xml
<?xml version="1.0" encoding="UTF-8"?>
<cyber_config>
    <transport_config>
        <rtps_transport>
            <enabled>true</enabled>
            <multicast_ip>239.255.0.1</multicast_ip>
            <port>9090</port>
        </rtps_transport>
    </transport_config>
</cyber_config>
```

### 步骤 3: 配置网络防火墙

在所有主机上执行：
```bash
# 允许 UDP 多播流量
sudo ufw allow 9090/udp

# 允许局域网通信
sudo ufw allow from 192.168.1.0/24

# 如果使用 iptables
sudo iptables -A INPUT -p udp --dport 9090 -j ACCEPT
sudo iptables -A OUTPUT -p udp --dport 9090 -j ACCEPT
```

### 步骤 4: 启动 Visualizer

在本机（订阅端）启动 visualizer：

```bash
# 方式 1: 使用 Host 网络模式运行容器
docker run -it --net=host your_container ./cyber_visualizer \
    --rtps-enabled \
    -m 239.255.0.1 \
    -i /apollo/drivers/camera/front/image \
    -p /apollo/sensors/lidar/top/pointcloud

# 方式 2: 直接在宿主机运行
./cyber_visualizer \
    --rtps-enabled \
    --multicast-ip 239.255.0.1 \
    --image-channel /apollo/drivers/camera/front/image \
    --pcd-channel /apollo/sensors/lidar/top/pointcloud
```

### 步骤 5: 验证连接

1. 启动后，在 UI 中查看通道列表是否显示远程主机的通道
2. 双击通道或菜单选择 "Subscribe Channel" 订阅
3. 观察图像和点云是否正常显示
4. 查看状态栏的 FPS 信息确认数据流正常

## 常见问题排查

### 问题 1: 看不到远程通道
- 检查所有主机的 RTPS 配置是否一致
- 确认防火墙已正确配置
- 测试网络连通性：`ping 192.168.1.108` 和 `ping 192.168.1.102`
- 使用 `cyber_channel list` 确认本地能看到通道

### 问题 2: 订阅后无数据显示
- 确认通道名称完全匹配（区分大小写）
- 检查消息类型是否匹配（Image/PointCloud）
- 查看终端输出是否有错误信息

### 问题 3: 高延迟或丢帧
- 检查网络带宽是否充足
- 降低图像分辨率或点云密度
- 考虑使用有线网络连接代替 WiFi

## 命令行参数参考

```
Usage: cyber_visualizer [options]

Options:
  -h, --help                 显示帮助信息
  -v, --version              显示版本信息
  -i, --image-channel <name> 订阅图像通道
  -p, --pcd-channel <name>   订阅点云通道
  -r, --rtps-enabled         启用 RTPS 跨主机模式
  -m, --multicast-ip <ip>    RTPS 多播 IP (默认：239.255.0.1)
  --port <port>              RTPS 端口 (默认：9090)
  -l, --list-channels        列出所有可用通道并退出
```
