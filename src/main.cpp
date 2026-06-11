#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>

#include "cyber/cyber.h"
#include "cyber/proto/point_cloud.pb.h"
#include "cyber/proto/image.pb.h"

#include "ec_node_wrapper.h"
#include "visualizer_window.h"
#include "channel_monitor.h"

using namespace apollo::cyber;

void printUsage(const char* program) {
    std::cout << "Usage: " << program << " [options]\n"
              << "Options:\n"
              << "  --rtps-enabled          Enable RTPS transport for cross-host communication\n"
              << "  --multicast-ip <ip>     Set RTPS multicast IP (default: 239.255.0.1)\n"
              << "  --port <port>           Set RTPS port (default: 9090)\n"
              << "  --mode <mode>           Run mode: 'local' (default) or 'foxglove'\n"
              << "  --ws-port <port>        WebSocket port for Foxglove (default: 8765)\n"
              << "  --help                  Show this help message\n";
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    bool rtpsEnabled = false;
    std::string multicastIp = "239.255.0.1";
    int rtpsPort = 9090;
    std::string mode = "local";
    int wsPort = 8765;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--rtps-enabled") {
            rtpsEnabled = true;
        } else if (arg == "--multicast-ip" && i + 1 < argc) {
            multicastIp = argv[++i];
        } else if (arg == "--port" && i + 1 < argc) {
            rtpsPort = std::stoi(argv[++i]);
        } else if (arg == "--mode" && i + 1 < argc) {
            mode = argv[++i];
        } else if (arg == "--ws-port" && i + 1 < argc) {
            wsPort = std::stoi(argv[++i]);
        } else if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
    }

    std::cout << "=== ec_visualizer v2.2.0 ===\n";
    std::cout << "CyberRT Root: " << (std::getenv("CYBER_ROOT") ? std::getenv("CYBER_ROOT") : "(not set)") << "\n";
    
    if (rtpsEnabled) {
        std::cout << "RTPS Mode: ENABLED\n";
        std::cout << "  Multicast IP: " << multicastIp << "\n";
        std::cout << "  Port: " << rtpsPort << "\n";
    } else {
        std::cout << "RTPS Mode: DISABLED (local only)\n";
    }
    std::cout << "Mode: " << mode << "\n";
    if (mode == "foxglove") {
        std::cout << "WebSocket Port: " << wsPort << "\n";
    }
    std::cout << "==========================\n\n";

    // Initialize CyberRT
    if (!Init(argc, argv)) {
        std::cerr << "Failed to initialize CyberRT\n";
        return -1;
    }

    // Create node wrapper
    auto nodeWrapper = std::make_shared<EcNodeWrapper>("ec_visualizer_node");
    if (rtpsEnabled) {
        nodeWrapper->enableRtps(multicastIp, rtpsPort);
    }

    // Create channel monitor
    auto monitor = std::make_shared<ChannelMonitor>();
    monitor->start();

    // Create visualizer window
    VisualizerWindow window(nodeWrapper, monitor);
    window.show();

#ifdef ENABLE_FOXGLOVE_BRIDGE
    if (mode == "foxglove") {
        // Start Foxglove bridge in separate thread
        std::thread foxgloveThread([nodeWrapper, wsPort]() {
            // Foxglove bridge implementation would go here
            std::cout << "Foxglove bridge started on port " << wsPort << "\n";
        });
        foxgloveThread.detach();
    }
#endif

    // Run Qt event loop
    QApplication app(argc, argv);
    return app.exec();
}
