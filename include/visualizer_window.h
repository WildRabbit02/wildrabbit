#ifndef VISUALIZER_WINDOW_H
#define VISUALIZER_WINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

#include <memory>
#include "ec_node_wrapper.h"
#include "channel_monitor.h"
#include "image_display.h"
#include "pointcloud_display.h"

class VisualizerWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit VisualizerWindow(std::shared_ptr<apollo::cyber::EcNodeWrapper> nodeWrapper,
                              std::shared_ptr<ChannelMonitor> monitor,
                              QWidget* parent = nullptr);
    ~VisualizerWindow();

private slots:
    void onChannelSelected();
    void onSubscribeButtonClicked();
    void refreshChannelList();

private:
    void setupUI();
    
    std::shared_ptr<apollo::cyber::EcNodeWrapper> nodeWrapper_;
    std::shared_ptr<ChannelMonitor> monitor_;
    
    QListWidget* channelList_;
    QTabWidget* tabWidget_;
    QPushButton* subscribeBtn_;
    QLabel* statusLabel_;
    
    ImageDisplay* imageDisplay_;
    PointCloudDisplay* pcDisplay_;
};

#endif // VISUALIZER_WINDOW_H
