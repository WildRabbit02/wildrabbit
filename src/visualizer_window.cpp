#include "visualizer_window.h"
#include <QApplication>
#include <QMessageBox>
#include <QToolBar>
#include <QDebug>

namespace cyber_visualizer {

VisualizerWindow::VisualizerWindow(QWidget* parent)
    : QMainWindow(parent) {
    setupUI();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    connectSignals();
    
    // 启动刷新定时器
    refresh_timer_ = new QTimer(this);
    connect(refresh_timer_, &QTimer::timeout, this, &VisualizerWindow::refreshChannelList);
    refresh_timer_->start(2000);  // 每 2 秒刷新一次
}

VisualizerWindow::~VisualizerWindow() {
    if (cyber_node_) {
        cyber_node_->shutdown();
    }
}

void VisualizerWindow::setupUI() {
    setWindowTitle("Apollo Cyber Visualizer");
    setMinimumSize(1280, 720);
    
    // 创建主分割器
    auto main_splitter = new QSplitter(Qt::Horizontal, this);
    
    // 左侧面板 - 通道列表和监控
    auto left_widget = new QWidget();
    auto left_layout = new QVBoxLayout(left_widget);
    
    channel_list_ = new QListWidget();
    channel_list_->setMaximumWidth(300);
    left_layout->addWidget(new QLabel("<b>Available Channels</b>"));
    left_layout->addWidget(channel_list_);
    
    channel_monitor_ = new ChannelMonitor();
    left_layout->addWidget(new QLabel("<b>Channel Monitor</b>"));
    left_layout->addWidget(channel_monitor_);
    
    // 右侧面板 - 显示区域
    tab_widget_ = new QTabWidget();
    tab_widget_->setTabsClosable(true);
    
    main_splitter->addWidget(left_widget);
    main_splitter->addWidget(tab_widget_);
    main_splitter->setStretchFactor(0, 1);
    main_splitter->setStretchFactor(1, 3);
    
    setCentralWidget(main_splitter);
}

void VisualizerWindow::setupMenuBar() {
    auto menu_bar = menuBar();
    
    // 文件菜单
    auto file_menu = menu_bar->addMenu("&File");
    subscribe_action_ = file_menu->addAction("&Subscribe Channel...");
    subscribe_action_->setShortcut(QKeySequence("Ctrl+S"));
    file_menu->addSeparator();
    auto exit_action = file_menu->addAction("E&xit");
    exit_action->setShortcut(QKeySequence("Ctrl+Q"));
    connect(exit_action, &QAction::triggered, qApp, &QApplication::quit);
    
    // 视图菜单
    auto view_menu = menu_bar->addMenu("&View");
    refresh_action_ = view_menu->addAction("&Refresh Channels");
    refresh_action_->setShortcut(QKeySequence("F5"));
    view_menu->addSeparator();
    rtps_action_ = view_menu->addAction("Enable &RTPS Mode");
    rtps_action_->setCheckable(true);
    
    // 帮助菜单
    auto help_menu = menu_bar->addMenu("&Help");
    about_action_ = help_menu->addAction("&About");
}

void VisualizerWindow::setupToolBar() {
    auto toolbar = addToolBar("Main Toolbar");
    toolbar->addAction(subscribe_action_);
    toolbar->addAction(refresh_action_);
    toolbar->addSeparator();
    toolbar->addWidget(new QLabel("  RTPS: "));
    auto rtps_checkbox = new QCheckBox();
    rtps_checkbox->setChecked(rtps_enabled_);
    connect(rtps_checkbox, &QCheckBox::toggled, this, &VisualizerWindow::toggleRtpsMode);
    toolbar->addWidget(rtps_checkbox);
}

void VisualizerWindow::setupStatusBar() {
    status_bar_ = statusBar();
    status_bar_->showMessage("Ready - Connect to CyberRT channels");
}

void VisualizerWindow::connectSignals() {
    connect(channel_list_, &QListWidget::itemDoubleClicked, 
            [this](QListWidgetItem* item) {
                onChannelSelected(item->text());
            });
    connect(subscribe_action_, &QAction::triggered, this, &VisualizerWindow::openSubscribeDialog);
    connect(refresh_action_, &QAction::triggered, this, &VisualizerWindow::refreshChannelList);
    connect(about_action_, &QAction::triggered, this, &VisualizerWindow::showAbout);
    connect(tab_widget_, &QTabWidget::tabCloseRequested, 
            [this](int index) {
                tab_widget_->removeTab(index);
            });
}

void VisualizerWindow::setCyberNode(std::shared_ptr<CyberNodeWrapper> node) {
    cyber_node_ = node;
    if (channel_monitor_) {
        channel_monitor_->setCyberNode(node);
    }
    refreshChannelList();
}

void VisualizerWindow::addImageDisplay(const std::string& channel_name) {
    if (!cyber_node_) return;
    
    auto display = std::make_unique<ImageDisplay>(channel_name);
    
    // 订阅通道
    cyber_node_->subscribeImage(channel_name, 
        [display_ptr = display.get()](const void* data, int width, int height, int channels, uint64_t timestamp) {
            display_ptr->updateImage(data, width, height, channels, timestamp);
        }
    );
    
    // 添加到标签页
    int index = tab_widget_->addTab(display.release(), QString("Image: %1").arg(QString::fromStdString(channel_name)));
    tab_widget_->setCurrentIndex(index);
    
    image_displays_.push_back(std::unique_ptr<ImageDisplay>(dynamic_cast<ImageDisplay*>(tab_widget_->widget(index))));
    
    status_bar_->showMessage(QString("Subscribed to image channel: %1").arg(QString::fromStdString(channel_name)));
}

void VisualizerWindow::addPointCloudDisplay(const std::string& channel_name) {
    if (!cyber_node_) return;
    
    auto display = std::make_unique<PointCloudDisplay>(channel_name);
    
    // 订阅通道
    cyber_node_->subscribePointCloud(channel_name,
        [display_ptr = display.get()](const float* points, int point_count, uint64_t timestamp) {
            display_ptr->updatePointCloud(points, point_count, timestamp);
        }
    );
    
    // 添加到标签页
    int index = tab_widget_->addTab(display.release(), QString("PointCloud: %1").arg(QString::fromStdString(channel_name)));
    tab_widget_->setCurrentIndex(index);
    
    pcd_displays_.push_back(std::unique_ptr<PointCloudDisplay>(dynamic_cast<PointCloudDisplay*>(tab_widget_->widget(index))));
    
    status_bar_->showMessage(QString("Subscribed to pointcloud channel: %1").arg(QString::fromStdString(channel_name)));
}

void VisualizerWindow::onChannelSelected(const QString& channel_name) {
    openSubscribeDialog();
}

void VisualizerWindow::refreshChannelList() {
    channel_list_->clear();
    
    if (!cyber_node_) return;
    
    auto channels = cyber_node_->getAvailableChannels();
    for (const auto& ch : channels) {
        channel_list_->addItem(QString::fromStdString(ch.name));
    }
    
    if (channel_monitor_) {
        channel_monitor_->refreshChannels();
    }
}

void VisualizerWindow::openSubscribeDialog() {
    if (!cyber_node_) return;
    
    auto channels = cyber_node_->getAvailableChannels();
    SubscribeDialog dialog(channels, this);
    
    if (dialog.exec() == QDialog::Accepted) {
        std::string channel = dialog.getSelectedChannel();
        std::string type = dialog.getChannelType();
        
        if (type == "image") {
            addImageDisplay(channel);
        } else if (type == "pointcloud") {
            addPointCloudDisplay(channel);
        }
    }
}

void VisualizerWindow::toggleRtpsMode(bool checked) {
    rtps_enabled_ = checked;
    if (cyber_node_) {
        cyber_node_->setRtpsMode(checked, multicast_ip_, rtps_port_);
    }
    status_bar_->showMessage(checked ? "RTPS mode enabled" : "RTPS mode disabled");
}

void VisualizerWindow::showAbout() {
    QMessageBox::about(this, "About Cyber Visualizer",
        "<h2>Apollo Cyber Visualizer</h2>"
        "<p>Version 1.0.0</p>"
        "<p>A standalone visualization tool for Apollo CyberRT.</p>"
        "<p>Features:</p>"
        "<ul>"
        "<li>Real-time camera image display</li>"
        "<li>3D point cloud visualization</li>"
        "<li>Channel monitoring</li>"
        "<li>Cross-host communication via RTPS</li>"
        "</ul>"
        "<p>Supports subscribing to data from multiple hosts.</p>"
    );
}

// SubscribeDialog implementation
SubscribeDialog::SubscribeDialog(const std::vector<ChannelInfo>& channels, QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Subscribe to Channel");
    setMinimumWidth(400);
    setupUI(channels);
}

void SubscribeDialog::setupUI(const std::vector<ChannelInfo>& channels) {
    auto layout = new QVBoxLayout(this);
    
    // 通道类型选择
    auto type_layout = new QHBoxLayout();
    type_layout->addWidget(new QLabel("Channel Type:"));
    type_combo_ = new QComboBox();
    type_combo_->addItem("Image", "image");
    type_combo_->addItem("PointCloud", "pointcloud");
    type_combo_->addItem("Any", "any");
    type_layout->addWidget(type_combo_);
    layout->addLayout(type_layout);
    
    // 通道选择
    auto channel_layout = new QHBoxLayout();
    channel_layout->addWidget(new QLabel("Channel:"));
    channel_combo_ = new QComboBox();
    channel_combo_->setEditable(true);
    for (const auto& ch : channels) {
        channel_combo_->addItem(QString::fromStdString(ch.name));
    }
    channel_layout->addWidget(channel_combo_);
    layout->addLayout(channel_layout);
    
    // 快捷选项
    auto quick_layout = new QHBoxLayout();
    image_check_ = new QCheckBox("Filter Images Only");
    pcd_check_ = new QCheckBox("Filter PointClouds Only");
    quick_layout->addWidget(image_check_);
    quick_layout->addWidget(pcd_check_);
    layout->addLayout(quick_layout);
    
    // 按钮
    auto button_layout = new QHBoxLayout();
    button_layout->addStretch();
    auto ok_button = new QPushButton("OK");
    auto cancel_button = new QPushButton("Cancel");
    button_layout->addWidget(ok_button);
    button_layout->addWidget(cancel_button);
    layout->addLayout(button_layout);
    
    connect(ok_button, &QPushButton::clicked, this, &SubscribeDialog::onOkClicked);
    connect(cancel_button, &QPushButton::clicked, this, &QDialog::reject);
    connect(type_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &SubscribeDialog::onChannelTypeChanged);
}

void SubscribeDialog::onOkClicked() {
    selected_channel_ = channel_combo_->currentText().toStdString();
    channel_type_ = type_combo_->currentData().toString().toStdString();
    accept();
}

void SubscribeDialog::onChannelTypeChanged(int index) {
    // 可以根据类型过滤通道列表
}

} // namespace cyber_visualizer
