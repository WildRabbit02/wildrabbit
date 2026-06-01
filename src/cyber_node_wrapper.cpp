#include "cyber_node_wrapper.h"

#include "cyber/proto/channel_control.pb.h"
#include "cyber/common/global_config.h"
#include "modules/drivers/camera/proto/image.pb.h"
#include "modules/drivers/pointcloud/proto/point_cloud.pb.h"

#include <iostream>
#include <chrono>

namespace cyber_visualizer {

CyberNodeWrapper::CyberNodeWrapper(const std::string& node_name) {
    // 默认配置从环境变量或默认路径加载
}

CyberNodeWrapper::~CyberNodeWrapper() {
    shutdown();
}

bool CyberNodeWrapper::init() {
    if (!apollo::cyber::OK()) {
        apollo::cyber::Init("cyber_visualizer");
    }
    
    node_ = apollo::cyber::CreateNode("visualizer_node");
    if (!node_) {
        std::cerr << "Failed to create cyber node" << std::endl;
        return false;
    }
    
    std::cout << "Cyber node created successfully" << std::endl;
    return true;
}

std::vector<ChannelInfo> CyberNodeWrapper::getAvailableChannels() {
    std::vector<ChannelInfo> channels;
    
    // 使用 CyberRT 的 ChannelControl 获取通道列表
    auto channel_control = apollo::cyber::service::ChannelControl::Instance();
    if (!channel_control) {
        return channels;
    }
    
    apollo::cyber::proto::ChannelControlMessage ctrl_msg;
    channel_control->GetChannelList(&ctrl_msg);
    
    for (const auto& channel : ctrl_msg.channels()) {
        ChannelInfo info;
        info.name = channel.channel_name();
        info.message_type = channel.message_type();
        info.message_count = 0;
        info.last_timestamp = 0;
        info.frequency = 0.0;
        channels.push_back(info);
    }
    
    return channels;
}

bool CyberNodeWrapper::subscribeImage(const std::string& channel_name, ImageCallback callback) {
    if (!node_) {
        std::cerr << "Node not initialized" << std::endl;
        return false;
    }
    
    image_callbacks_[channel_name] = callback;
    
    auto reader = node_->CreateReader<apollo::drivers::Image>(
        channel_name,
        [this, channel_name](const std::shared_ptr<apollo::drivers::Image>& msg) {
            handleImageMessage(channel_name, msg.get());
        }
    );
    
    if (!reader) {
        std::cerr << "Failed to create reader for channel: " << channel_name << std::endl;
        return false;
    }
    
    readers_.push_back(reader);
    std::cout << "Subscribed to image channel: " << channel_name << std::endl;
    return true;
}

bool CyberNodeWrapper::subscribePointCloud(const std::string& channel_name, PointCloudCallback callback) {
    if (!node_) {
        std::cerr << "Node not initialized" << std::endl;
        return false;
    }
    
    pcd_callbacks_[channel_name] = callback;
    
    auto reader = node_->CreateReader<apollo::drivers::PointCloud>(
        channel_name,
        [this, channel_name](const std::shared_ptr<apollo::drivers::PointCloud>& msg) {
            handlePointCloudMessage(channel_name, msg.get());
        }
    );
    
    if (!reader) {
        std::cerr << "Failed to create reader for channel: " << channel_name << std::endl;
        return false;
    }
    
    readers_.push_back(reader);
    std::cout << "Subscribed to pointcloud channel: " << channel_name << std::endl;
    return true;
}

void CyberNodeWrapper::spin() {
    running_ = true;
    std::cout << "Starting cyber node spin..." << std::endl;
    apollo::cyber::WaitForShutdown();
    running_ = false;
}

void CyberNodeWrapper::shutdown() {
    running_ = false;
    readers_.clear();
    image_callbacks_.clear();
    pcd_callbacks_.clear();
    node_.reset();
}

void CyberNodeWrapper::setRtpsMode(bool enabled, const std::string& multicast_ip, int port) {
    rtps_enabled_ = enabled;
    multicast_ip_ = multicast_ip;
    rtps_port_ = port;
    
    if (enabled) {
        std::cout << "RTPS mode enabled: " << multicast_ip << ":" << port << std::endl;
        // 注意：实际 RTPS 配置需要在 cyber_conf.xml 中设置
        // 这里只是标记模式，具体配置由 CyberRT 自动处理
    }
}

void CyberNodeWrapper::handleImageMessage(const std::string& channel_name, const void* raw_data) {
    auto it = image_callbacks_.find(channel_name);
    if (it == image_callbacks_.end() || !raw_data) {
        return;
    }
    
    const auto* img = static_cast<const apollo::drivers::Image*>(raw_data);
    
    // 转换图像格式并调用回调
    it->second(
        img->data().data(),
        img->width(),
        img->height(),
        img->encoding() == "mono8" ? 1 : (img->encoding() == "rgb8" ? 3 : 4),
        img->header().timestamp_sec() * 1000000000ULL + img->header().timestamp_nsec()
    );
}

void CyberNodeWrapper::handlePointCloudMessage(const std::string& channel_name, const void* raw_data) {
    auto it = pcd_callbacks_.find(channel_name);
    if (it == pcd_callbacks_.end() || !raw_data) {
        return;
    }
    
    const auto* pcd = static_cast<const apollo::drivers::PointCloud*>(raw_data);
    
    // 转换点云格式并调用回调
    std::vector<float> points;
    points.reserve(pcd->point_size() * 3);
    
    for (const auto& point : pcd->point()) {
        points.push_back(point.x());
        points.push_back(point.y());
        points.push_back(point.z());
    }
    
    it->second(
        points.data(),
        static_cast<int>(points.size() / 3),
        pcd->header().timestamp_sec() * 1000000000ULL + pcd->header().timestamp_nsec()
    );
}

} // namespace cyber_visualizer
