#include <gtest/gtest.h>
#include "ec_node_wrapper.h"

class EcNodeWrapperTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(EcNodeWrapperTest, NodeInitialization) {
    EcNodeWrapper wrapper;
    EXPECT_TRUE(wrapper.init("test_node"));
}

TEST_F(EcNodeWrapperTest, ChannelSubscription) {
    EcNodeWrapper wrapper;
    wrapper.init("test_node");
    // Test image channel subscription
    EXPECT_TRUE(wrapper.subscribeImage("/camera/test", nullptr));
    // Test pointcloud channel subscription  
    EXPECT_TRUE(wrapper.subscribePointCloud("/lidar/test", nullptr));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
