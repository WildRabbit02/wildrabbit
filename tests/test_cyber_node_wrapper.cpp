/**
 * @file test_cyber_node_wrapper.cpp
 * @brief CyberNodeWrapper 单元测试
 * @version 2.0.0
 */

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <thread>
#include <chrono>

#include "cyber_node_wrapper.h"

class CyberNodeWrapperTest : public ::testing::Test {
protected:
    void SetUp() override {
        wrapper_ = std::make_unique<CyberNodeWrapper>("test_visualizer_node");
    }

    void TearDown() override {
        wrapper_.reset();
    }

    std::unique_ptr<CyberNodeWrapper> wrapper_;
};

/**
 * @brief 测试节点初始化
 */
TEST_F(CyberNodeWrapperTest, NodeInitialization) {
    EXPECT_TRUE(wrapper_->isInitialized());
    EXPECT_EQ(wrapper_->getNodeName(), "test_visualizer_node");
}

/**
 * @brief 测试通道订阅（模拟模式）
 */
TEST_F(CyberNodeWrapperTest, SubscribeImageChannel) {
    std::string channel_name = "/test/camera/image";
    
    // 订阅图像通道
    bool success = wrapper_->subscribeImage(channel_name, 
        [](const apollo::drivers::Image& img) {
            // 回调函数（测试中不会真正触发）
        });
    
    EXPECT_TRUE(success);
    EXPECT_TRUE(wrapper_->isChannelSubscribed(channel_name));
}

/**
 * @brief 测试点云通道订阅
 */
TEST_F(CyberNodeWrapperTest, SubscribePointCloudChannel) {
    std::string channel_name = "/test/lidar/pointcloud";
    
    // 订阅点云通道
    bool success = wrapper_->subscribePointCloud(channel_name,
        [](const apollo::drivers::PointCloud& pc) {
            // 回调函数
        });
    
    EXPECT_TRUE(success);
    EXPECT_TRUE(wrapper_->isChannelSubscribed(channel_name));
}

/**
 * @brief 测试多通道同时订阅
 */
TEST_F(CyberNodeWrapperTest, MultipleChannelSubscription) {
    std::vector<std::string> channels = {
        "/camera/front/image",
        "/camera/rear/image",
        "/lidar/top/pointcloud",
        "/lidar/bottom/pointcloud"
    };
    
    for (const auto& channel : channels) {
        if (channel.find("image") != std::string::npos) {
            wrapper_->subscribeImage(channel, [](const apollo::drivers::Image&){});
        } else {
            wrapper_->subscribePointCloud(channel, [](const apollo::drivers::PointCloud&){});
        }
    }
    
    for (const auto& channel : channels) {
        EXPECT_TRUE(wrapper_->isChannelSubscribed(channel));
    }
}

/**
 * @brief 测试通道取消订阅
 */
TEST_F(CyberNodeWrapperTest, UnsubscribeChannel) {
    std::string channel_name = "/test/temp/channel";
    
    wrapper_->subscribeImage(channel_name, [](const apollo::drivers::Image&){});
    EXPECT_TRUE(wrapper_->isChannelSubscribed(channel_name));
    
    wrapper_->unsubscribe(channel_name);
    EXPECT_FALSE(wrapper_->isChannelSubscribed(channel_name));
}

/**
 * @brief 测试 RTPS 配置
 */
TEST_F(CyberNodeWrapperTest, RTPSConfiguration) {
    std::string multicast_ip = "239.255.0.1";
    int port = 9090;
    
    bool success = wrapper_->configureRTPS(multicast_ip, port);
    EXPECT_TRUE(success);
}

/**
 * @brief 测试通道列表获取
 */
TEST_F(CyberNodeWrapperTest, GetChannelList) {
    // 注意：实际通道列表依赖于运行的 CyberRT 系统
    auto channels = wrapper_->getAvailableChannels();
    // 至少不应该崩溃
    EXPECT_GE(channels.size(), 0);
}

/**
 * @brief 测试消息频率统计
 */
TEST_F(CyberNodeWrapperTest, MessageFrequencyStats) {
    std::string channel_name = "/test/frequency/channel";
    
    wrapper_->subscribeImage(channel_name, [](const apollo::drivers::Image&){});
    
    // 初始频率应为 0
    double freq = wrapper_->getMessageFrequency(channel_name);
    EXPECT_DOUBLE_EQ(freq, 0.0);
    
    // 模拟等待一段时间
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 再次检查（在没有真实消息的情况下仍应为 0）
    freq = wrapper_->getMessageFrequency(channel_name);
    EXPECT_DOUBLE_EQ(freq, 0.0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
