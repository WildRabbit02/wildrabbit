#include <gtest/gtest.h>
#include "channel_monitor.h"

class ChannelMonitorTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ChannelMonitorTest, ChannelDiscovery) {
    ChannelMonitor monitor;
    monitor.addChannel("/camera/left", "apollo::drivers::image");
    monitor.addChannel("/lidar/points", "apollo::drivers::PointCloud");
    
    EXPECT_TRUE(monitor.hasChannel("/camera/left"));
    EXPECT_TRUE(monitor.hasChannel("/lidar/points"));
    EXPECT_EQ(monitor.getChannelCount(), 2);
}

TEST_F(ChannelMonitorTest, ChannelRemoval) {
    ChannelMonitor monitor;
    monitor.addChannel("/temp", "std::string");
    EXPECT_TRUE(monitor.hasChannel("/temp"));
    
    monitor.removeChannel("/temp");
    EXPECT_FALSE(monitor.hasChannel("/temp"));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
