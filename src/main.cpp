#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <iostream>
#include <memory>

#include "visualizer_window.h"
#include "cyber_node_wrapper.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Cyber Visualizer");
    app.setOrganizationName("Apollo Auto");
    
    // 命令行参数解析
    QCommandLineParser parser;
    parser.setApplicationDescription("Apollo CyberRT 数据可视化工具 - 用于监控和显示实时数据通道");
    parser.addHelpOption();
    parser.addVersionOption();
    
    QCommandLineOption imageChannelOption(
        QStringList() << "i" << "image-channel",
        "订阅的图像通道名称",
        "channel_name"
    );
    parser.addOption(imageChannelOption);
    
    QCommandLineOption pcdChannelOption(
        QStringList() << "p" << "pcd-channel",
        "订阅的点云通道名称",
        "channel_name"
    );
    parser.addOption(pcdChannelOption);
    
    QCommandLineOption rtpsOption(
        QStringList() << "r" << "rtps-enabled",
        "启用 RTPS 跨主机模式"
    );
    parser.addOption(rtpsOption);
    
    QCommandLineOption multicastIpOption(
        QStringList() << "m" << "multicast-ip",
        "RTPS 多播 IP 地址 (默认：239.255.0.1)",
        "ip_address",
        "239.255.0.1"
    );
    parser.addOption(multicastIpOption);
    
    QCommandLineOption portOption(
        QStringList() << "port",
        "RTPS 端口号 (默认：9090)",
        "port",
        "9090"
    );
    parser.addOption(portOption);
    
    QCommandLineOption listChannelsOption(
        QStringList() << "l" << "list-channels",
        "列出所有可用通道并退出"
    );
    parser.addOption(listChannelsOption);
    
    parser.process(app);
    
    // 创建 CyberRT 节点
    auto cyber_node = std::make_shared<cyber_visualizer::CyberNodeWrapper>();
    
    // 配置 RTPS 模式
    if (parser.isSet(rtpsOption)) {
        cyber_node->setRtpsMode(
            true,
            parser.value(multicastIpOption).toStdString(),
            parser.value(portOption).toInt()
        );
    }
    
    // 初始化 CyberRT
    if (!cyber_node->init()) {
        std::cerr << "Failed to initialize CyberRT node" << std::endl;
        return -1;
    }
    
    // 仅列出通道
    if (parser.isSet(listChannelsOption)) {
        std::cout << "\n=== Available CyberRT Channels ===\n" << std::endl;
        auto channels = cyber_node->getAvailableChannels();
        if (channels.empty()) {
            std::cout << "No channels found." << std::endl;
        } else {
            for (const auto& ch : channels) {
                std::cout << "  Channel: " << ch.name 
                          << "\n    Type: " << ch.message_type << std::endl;
            }
        }
        return 0;
    }
    
    // 创建主窗口
    cyber_visualizer::VisualizerWindow window;
    window.setCyberNode(cyber_node);
    window.show();
    
    // 自动订阅指定的通道
    if (parser.isSet(imageChannelOption)) {
        window.addImageDisplay(parser.value(imageChannelOption).toStdString());
    }
    if (parser.isSet(pcdChannelOption)) {
        window.addPointCloudDisplay(parser.value(pcdChannelOption).toStdString());
    }
    
    return app.exec();
}
