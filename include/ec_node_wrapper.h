#ifndef EC_NODE_WRAPPER_H
#define EC_NODE_WRAPPER_H

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <mutex>
#include <map>

#include "cyber/cyber.h"
#include "cyber/proto/image.pb.h"
#include "cyber/proto/point_cloud.pb.h"

namespace apollo {
namespace cyber {

class EcNodeWrapper {
public:
    using ImageCallback = std::function<void(const std::shared_ptr<proto::Image>&)>;
    using PointCloudCallback = std::function<void(const std::shared_ptr<proto::PointCloud>&)>;

    explicit EcNodeWrapper(const std::string& node_name);
    ~EcNodeWrapper();

    // RTPS Configuration
    void enableRtps(const std::string& multicast_ip, int port);
    
    // Subscription methods
    bool subscribeImage(const std::string& channel_name, ImageCallback callback);
    bool subscribePointCloud(const std::string& channel_name, PointCloudCallback callback);
    
    // Channel management
    std::vector<std::string> getSubscribedChannels() const;
    void unsubscribe(const std::string& channel_name);
    
    // Statistics
    struct ChannelStats {
        uint64_t message_count = 0;
        double frequency_hz = 0.0;
        std::string last_msg_time;
    };
    ChannelStats getChannelStats(const std::string& channel_name) const;

private:
    std::shared_ptr<Node> node_;
    std::string multicast_ip_;
    int rtps_port_;
    bool rtps_enabled_ = false;
    
    mutable std::mutex mutex_;
    std::map<std::string, ChannelStats> channel_stats_;
    
    // Reader pointers (kept alive)
    std::vector<std::shared_ptr<Reader<proto::Image>>> image_readers_;
    std::vector<std::shared_ptr<Reader<proto::PointCloud>>> pc_readers_;
};

} // namespace cyber
} // namespace apollo

#endif // EC_NODE_WRAPPER_H
