#ifndef CYBER_NODE_WRAPPER_H
#define CYBER_NODE_WRAPPER_H

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <map>
#include <mutex>
#include <atomic>

#include "cyber/cyber.h"
#include "cyber/proto/channel_control.pb.h"

namespace cyber_visualizer {

// 通道信息结构
struct ChannelInfo {
    std::string name;
    std::string message_type;
    uint64_t message_count;
    uint64_t last_timestamp;
    double frequency;  // Hz
};

// 图像数据回调类型
using ImageCallback = std::function<void(const void* data, int width, int height, int channels, uint64_t timestamp)>;

// 点云数据回调类型  
using PointCloudCallback = std::function<void(const float* points, int point_count, uint64_t timestamp)>;

/**
 * @brief CyberRT Node 封装类
 * 
 * 提供简化的节点创建、通道订阅和数据显示功能
 */
class CyberNodeWrapper {
public:
    CyberNodeWrapper(const std::string& node_name = "visualizer_node");
    ~CyberNodeWrapper();

    /**
     * @brief 初始化 CyberRT 系统
     * @return 成功返回 true
     */
    bool init();

    /**
     * @brief 获取所有可用通道列表
     * @return 通道信息列表
     */
    std::vector<ChannelInfo> getAvailableChannels();

    /**
     * @brief 订阅图像通道
     * @param channel_name 通道名称
     * @param callback 数据回调函数
     * @return 成功返回 true
     */
    bool subscribeImage(const std::string& channel_name, ImageCallback callback);

    /**
     * @brief 订阅点云通道
     * @param channel_name 通道名称
     * @param callback 数据回调函数
     * @return 成功返回 true
     */
    bool subscribePointCloud(const std::string& channel_name, PointCloudCallback callback);

    /**
     * @brief 启动节点主循环
     */
    void spin();

    /**
     * @brief 停止节点
     */
    void shutdown();

    /**
     * @brief 检查节点是否运行中
     */
    bool isRunning() const { return running_.load(); }

    /**
     * @brief 设置 RTPS 跨主机模式
     * @param enabled 是否启用
     * @param multicast_ip 多播 IP 地址
     * @param port 端口号
     */
    void setRtpsMode(bool enabled, const std::string& multicast_ip = "239.255.0.1", int port = 9090);

private:
    std::shared_ptr<apollo::cyber::Node> node_;
    std::vector<std::shared_ptr<apollo::cyber::ReaderBase>> readers_;
    std::map<std::string, ImageCallback> image_callbacks_;
    std::map<std::string, PointCloudCallback> pcd_callbacks_;
    
    std::mutex channel_mutex_;
    std::atomic<bool> running_{false};
    
    bool rtps_enabled_{false};
    std::string multicast_ip_;
    int rtps_port_{9090};

    // 内部回调处理
    void handleImageMessage(const std::string& channel_name, const void* raw_data);
    void handlePointCloudMessage(const std::string& channel_name, const void* raw_data);
};

} // namespace cyber_visualizer

#endif // CYBER_NODE_WRAPPER_H
