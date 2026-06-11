#include "visualizer_window.h"
#include <QMessageBox>
#include <QTimer>

VisualizerWindow::VisualizerWindow(std::shared_ptr<apollo::cyber::EcNodeWrapper> nodeWrapper,
                                   std::shared_ptr<ChannelMonitor> monitor,
                                   QWidget* parent)
    : QMainWindow(parent), nodeWrapper_(nodeWrapper), monitor_(monitor) {
    
    setupUI();
    
    // Auto-refresh channel list every 2 seconds
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &VisualizerWindow::refreshChannelList);
    timer->start(2000);
    
    refreshChannelList();
}

VisualizerWindow::~VisualizerWindow() {}

void VisualizerWindow::setupUI() {
    setWindowTitle("ec_visualizer v2.2.0 - CyberRT Data Monitor");
    resize(1200, 800);
    
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    auto* mainLayout = new QVBoxLayout(centralWidget);
    
    // Channel list section
    channelList_ = new QListWidget();
    channelList_->setMinimumHeight(150);
    
    auto* listLayout = new QVBoxLayout();
    listLayout->addWidget(new QLabel("Available Channels:"));
    listLayout->addWidget(channelList_);
    
    subscribeBtn_ = new QPushButton("Subscribe Selected");
    connect(subscribeBtn_, &QPushButton::clicked, this, &VisualizerWindow::onSubscribeButtonClicked);
    listLayout->addWidget(subscribeBtn_);
    
    mainLayout->addLayout(listLayout);
    
    // Tab widget for displays
    tabWidget_ = new QTabWidget();
    imageDisplay_ = new ImageDisplay();
    pcDisplay_ = new PointCloudDisplay();
    
    tabWidget_->addTab(imageDisplay_, "Camera Images");
    tabWidget_->addTab(pcDisplay_, "Point Cloud");
    
    mainLayout->addWidget(tabWidget_);
    
    // Status bar
    statusLabel_ = new QLabel("Ready");
    statusBar()->addWidget(statusLabel_);
}

void VisualizerWindow::onChannelSelected() {}

void VisualizerWindow::onSubscribeButtonClicked() {
    auto* currentItem = channelList_->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, "No Selection", "Please select a channel first.");
        return;
    }
    
    QString channelName = currentItem->text();
    statusLabel_->setText("Subscribing to: " + channelName);
    
    // Determine channel type and subscribe
    if (channelName.contains("image") || channelName.contains("camera")) {
        nodeWrapper_->subscribeImage(channelName.toStdString(), 
            [this](const auto& msg) {
                // Update image display in GUI thread
                QMetaObject::invokeMethod(imageDisplay_, "updateImage", Qt::QueuedConnection);
            });
        tabWidget_->setCurrentIndex(0);
    } else if (channelName.contains("point") || channelName.contains("lidar") || channelName.contains("cloud")) {
        nodeWrapper_->subscribePointCloud(channelName.toStdString(),
            [this](const auto& msg) {
                QMetaObject::invokeMethod(pcDisplay_, "updatePointCloud", Qt::QueuedConnection);
            });
        tabWidget_->setCurrentIndex(1);
    }
    
    statusLabel_->setText("Subscribed to: " + channelName);
}

void VisualizerWindow::refreshChannelList() {
    channelList_->clear();
    
    auto channels = monitor_->getAvailableChannels();
    for (const auto& channel : channels) {
        channelList_->addItem(QString::fromStdString(channel));
    }
    
    statusLabel_->setText(QString("Found %1 channels").arg(channels.size()));
}
