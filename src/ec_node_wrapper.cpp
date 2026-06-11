#include "ec_node_wrapper.h"
#include <iostream>
#include <chrono>

namespace apollo {
namespace cyber {

EcNodeWrapper::EcNodeWrapper(const std::string& node_name) 
    : multicast_ip_("239.255.0.1"), rtps_port_(9090) {
    node_ = CreateNode(node_name);
    if (!node_) {
        throw std::runtime_error("Failed to create CyberRT node");
    }
    std::cout << "[EcNodeWrapper] Node '" << node_name << "' created successfully\n";
}

EcNodeWrapper::~EcNodeWrapper() {
    node_.reset();
}

void EcNodeWrapper::enableRtps(const std::string& multicast_ip, int port) {
    multicast_ip_ = multicast_ip;
    rtps_port_ = port;
    rtps_enabled_ = true;
    std::cout << "[EcNodeWrapper] RTPS enabled: " << multicast_ip_ << ":" << rtps_port_ << "\n";
}

bool EcNodeWrapper::subscribeImage(const std::string& channel_name, ImageCallback callback) {
    if (!node_) return false;
    
    auto reader = node_->CreateReader<proto::Image>(channel_name, 
        [this, channel_name, callback](const std::shared_ptr<proto::Image>& msg) {
            std::lock_guard<std::mutex> lock(mutex_);
            channel_stats_[channel_name].message_count++;
            auto now = std::chrono::system_clock::now();
            auto time_t_now = std::chrono::system_clock::to_time_t(now);
            channel_stats_[channel_name].last_msg_time = std::ctime(&time_t_now);
            // Calculate frequency (simplified)
            channel_stats_[channel_name].frequency_hz += 1.0;
            
            if (callback) callback(msg);
        });
    
    if (reader) {
        std::lock_guard<std::mutex> lock(mutex_);
        image_readers_.push_back(reader);
        channel_stats_[channel_name] = ChannelStats{};
        std::cout << "[EcNodeWrapper] Subscribed to image channel: " << channel_name << "\n";
        return true;
    }
    return false;
}

bool EcNodeWrapper::subscribePointCloud(const std::string& channel_name, PointCloudCallback callback) {
    if (!node_) return false;
    
    auto reader = node_->CreateReader<proto::PointCloud>(channel_name,
        [this, channel_name, callback](const std::shared_ptr<proto::PointCloud>& msg) {
            std::lock_guard<std::mutex> lock(mutex_);
            channel_stats_[channel_name].message_count++;
            auto now = std::chrono::system_clock::now();
            auto time_t_now = std::chrono::system_clock::to_time_t(now);
            channel_stats_[channel_name].last_msg_time = std::ctime(&time_t_now);
            channel_stats_[channel_name].frequency_hz += 1.0;
            
            if (callback) callback(msg);
        });
    
    if (reader) {
        std::lock_guard<std::mutex> lock(mutex_);
        pc_readers_.push_back(reader);
        channel_stats_[channel_name] = ChannelStats{};
        std::cout << "[EcNodeWrapper] Subscribed to point cloud channel: " << channel_name << "\n";
        return true;
    }
    return false;
}

std::vector<std::string> EcNodeWrapper::getSubscribedChannels() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> channels;
    for (const auto& pair : channel_stats_) {
        channels.push_back(pair.first);
    }
    return channels;
}

void EcNodeWrapper::unsubscribe(const std::string& channel_name) {
    std::lock_guard<std::mutex> lock(mutex_);
    channel_stats_.erase(channel_name);
    // Note: In a real implementation, we would remove the specific reader
    std::cout << "[EcNodeWrapper] Unsubscribed from: " << channel_name << "\n";
}

EcNodeWrapper::ChannelStats EcNodeWrapper::getChannelStats(const std::string& channel_name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = channel_stats_.find(channel_name);
    if (it != channel_stats_.end()) {
        return it->second;
    }
    return ChannelStats{};
}

} // namespace cyber
} // namespace apollo
