#ifndef CHANNEL_MONITOR_H
#define CHANNEL_MONITOR_H

#include <string>
#include <vector>
#include <set>
#include <thread>
#include <atomic>
#include <mutex>

class ChannelMonitor {
public:
    ChannelMonitor();
    ~ChannelMonitor();
    
    void start();
    void stop();
    
    std::vector<std::string> getAvailableChannels() const;
    bool hasChannel(const std::string& channel_name) const;
    
    struct ChannelInfo {
        std::string name;
        std::string message_type;
        uint64_t msg_count;
        double frequency;
    };
    
    std::vector<ChannelInfo> getChannelDetails() const;

private:
    void monitorLoop();
    
    std::thread monitorThread_;
    std::atomic<bool> running_{false};
    mutable std::mutex mutex_;
    std::set<std::string> channels_;
    std::vector<ChannelInfo> channelDetails_;
};

#endif // CHANNEL_MONITOR_H
