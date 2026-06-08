/**
 * @file test_channel_monitor.cpp
 * @brief ChannelMonitor 单元测试
 * @version 2.0.0
 */

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

#include "channel_monitor.h"

class ChannelMonitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        monitor_ = std::make_unique<ChannelMonitor>();
    }

    void TearDown() override {
        monitor_.reset();
    }

    std::unique_ptr<ChannelMonitor> monitor_;
};

/**
 * @brief 测试通道添加
 */
TEST_F(ChannelMonitorTest, AddChannel) {
    std::string channel_name = "/test/channel/1";
    
    bool success = monitor_->addChannel(channel_name);
    EXPECT_TRUE(success);
}

/**
 * @brief 测试重复通道添加
 */
TEST_F(ChannelMonitorTest, AddDuplicateChannel) {
    std::string channel_name = "/test/channel/duplicate";
    
    monitor_->addChannel(channel_name);
    bool second_add = monitor_->addChannel(channel_name);
    
    // 重复添加应返回 false 或被忽略
    EXPECT_FALSE(second_add);
}

/**
 * @brief 测试通道移除
 */
TEST_F(ChannelMonitorTest, RemoveChannel) {
    std::string channel_name = "/test/channel/remove";
    
    monitor_->addChannel(channel_name);
    bool exists_before = monitor_->hasChannel(channel_name);
    EXPECT_TRUE(exists_before);
    
    monitor_->removeChannel(channel_name);
    bool exists_after = monitor_->hasChannel(channel_name);
    EXPECT_FALSE(exists_after);
}

/**
 * @brief 测试通道存在性检查
 */
TEST_F(ChannelMonitorTest, HasChannel) {
    std::string existing_channel = "/test/channel/exists";
    std::string non_existing_channel = "/test/channel/not_exists";
    
    monitor_->addChannel(existing_channel);
    
    EXPECT_TRUE(monitor_->hasChannel(existing_channel));
    EXPECT_FALSE(monitor_->hasChannel(non_existing_channel));
}

/**
 * @brief 测试获取所有通道
 */
TEST_F(ChannelMonitorTest, GetAllChannels) {
    std::vector<std::string> channels = {
        "/camera/front",
        "/camera/rear",
        "/lidar/top",
        "/radar/front"
    };
    
    for (const auto& ch : channels) {
        monitor_->addChannel(ch);
    }
    
    auto all_channels = monitor_->getAllChannels();
    EXPECT_EQ(all_channels.size(), channels.size());
    
    for (const auto& ch : channels) {
        auto it = std::find(all_channels.begin(), all_channels.end(), ch);
        EXPECT_NE(it, all_channels.end());
    }
}

/**
 * @brief 测试消息计数更新
 */
TEST_F(ChannelMonitorTest, UpdateMessageCount) {
    std::string channel_name = "/test/channel/count";
    monitor_->addChannel(channel_name);
    
    // 初始计数应为 0
    uint64_t count = monitor_->getMessageCount(channel_name);
    EXPECT_EQ(count, 0);
    
    // 模拟接收消息
    for (int i = 0; i < 100; ++i) {
        monitor_->updateMessageCount(channel_name);
    }
    
    count = monitor_->getMessageCount(channel_name);
    EXPECT_EQ(count, 100);
}

/**
 * @brief 测试频率计算
 */
TEST_F(ChannelMonitorTest, CalculateFrequency) {
    std::string channel_name = "/test/channel/freq";
    monitor_->addChannel(channel_name);
    
    // 初始频率应为 0
    double freq = monitor_->calculateFrequency(channel_name);
    EXPECT_DOUBLE_EQ(freq, 0.0);
    
    // 模拟一段时间内的消息
    for (int i = 0; i < 50; ++i) {
        monitor_->updateMessageCount(channel_name);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    freq = monitor_->calculateFrequency(channel_name);
    // 频率应该在合理范围内（约 100Hz）
    EXPECT_GT(freq, 0.0);
    EXPECT_LT(freq, 200.0);
}

/**
 * @brief 测试通道统计信息
 */
TEST_F(ChannelMonitorTest, GetChannelStats) {
    std::string channel_name = "/test/channel/stats";
    monitor_->addChannel(channel_name);
    
    // 发送一些消息
    for (int i = 0; i < 30; ++i) {
        monitor_->updateMessageCount(channel_name);
    }
    
    auto stats = monitor_->getChannelStats(channel_name);
    EXPECT_GE(stats.message_count, 30);
    EXPECT_GE(stats.frequency_hz, 0.0);
}

/**
 * @brief 测试清空所有通道
 */
TEST_F(ChannelMonitorTest, ClearAllChannels) {
    std::vector<std::string> channels = {
        "/ch1", "/ch2", "/ch3", "/ch4", "/ch5"
    };
    
    for (const auto& ch : channels) {
        monitor_->addChannel(ch);
    }
    
    EXPECT_EQ(monitor_->getAllChannels().size(), 5);
    
    monitor_->clearAllChannels();
    
    EXPECT_EQ(monitor_->getAllChannels().size(), 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
