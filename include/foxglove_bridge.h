#ifndef FOXGLOVE_BRIDGE_H
#define FOXGLOVE_BRIDGE_H

#include <memory>
#include <string>
#include <thread>
#include <atomic>

#include "ec_node_wrapper.h"

class FoxgloveBridge {
public:
    explicit FoxgloveBridge(std::shared_ptr<apollo::cyber::EcNodeWrapper> nodeWrapper, int port = 8765);
    ~FoxgloveBridge();
    
    void start();
    void stop();
    
    bool isRunning() const { return running_; }

private:
    void serverLoop();
    
    std::shared_ptr<apollo::cyber::EcNodeWrapper> nodeWrapper_;
    int port_;
    std::thread serverThread_;
    std::atomic<bool> running_{false};
};

#endif // FOXGLOVE_BRIDGE_H
