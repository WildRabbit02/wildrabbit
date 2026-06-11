#include "channel_monitor.h"
#include <iostream>
#include <chrono>

// Note: In a real implementation, this would use CyberRT's discovery API
// to actively monitor channels. For now, it simulates channel discovery.

ChannelMonitor::ChannelMonitor() {}

ChannelMonitor::~ChannelMonitor() {
    stop();
}

void ChannelMonitor::start() {
    running_ = true;
    monitorThread_ = std::thread(&ChannelMonitor::monitorLoop, this);
    std::cout << "[ChannelMonitor] Started\n";
}

void ChannelMonitor::stop() {
    running_ = false;
    if (monitorThread_.joinable()) {
        monitorThread_.join();
    }
    std::cout << "[ChannelMonitor] Stopped\n";
}

std::vector<std::string> ChannelMonitor::getAvailableChannels() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return std::vector<std::string>(channels_.begin(), channels_.end());
}

bool ChannelMonitor::hasChannel(const std::string& channel_name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return channels_.find(channel_name) != channels_.end();
}

std::vector<ChannelMonitor::ChannelInfo> ChannelMonitor::getChannelDetails() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return channelDetails_;
}

void ChannelMonitor::monitorLoop() {
    // Simulated channel discovery
    // In production, use: cyber::node::GetChannelList() or similar API
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    if (running_) {
        std::lock_guard<std::mutex> lock(mutex_);
        // Add some example channels for demonstration
        channels_.insert("/camera/left");
        channels_.insert("/camera/right");
        channels_.insert("/lidar/points");
        
        channelDetails_.clear();
        for (const auto& ch : channels_) {
            ChannelInfo info;
            info.name = ch;
            info.message_type = ch.find("image") != std::string::npos ? "Image" : "PointCloud";
            info.msg_count = 0;
            info.frequency = 0.0;
            channelDetails_.push_back(info);
        }
        
        std::cout << "[ChannelMonitor] Discovered " << channels_.size() << " channels\n";
    }
}
