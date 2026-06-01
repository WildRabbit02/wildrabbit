#ifndef VISUALIZER_WINDOW_H
#define VISUALIZER_WINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QTimer>
#include <QSplitter>
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

#include <string>
#include <vector>
#include <memory>

#include "cyber_node_wrapper.h"
#include "image_display.h"
#include "pointcloud_display.h"
#include "channel_monitor.h"

namespace cyber_visualizer {

/**
 * @brief 主可视化窗口
 */
class VisualizerWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit VisualizerWindow(QWidget* parent = nullptr);
    ~VisualizerWindow();

    /**
     * @brief 设置 CyberRT 节点包装器
     */
    void setCyberNode(std::shared_ptr<CyberNodeWrapper> node);

    /**
     * @brief 添加图像显示面板
     */
    void addImageDisplay(const std::string& channel_name);

    /**
     * @brief 添加点云显示面板
     */
    void addPointCloudDisplay(const std::string& channel_name);

    /**
     * @brief 更新通道列表
     */
    void updateChannelList();

private slots:
    /**
     * @brief 处理通道选择
     */
    void onChannelSelected(const QString& channel_name);

    /**
     * @brief 刷新通道列表
     */
    void refreshChannelList();

    /**
     * @brief 打开订阅对话框
     */
    void openSubscribeDialog();

    /**
     * @brief 切换 RTPS 模式
     */
    void toggleRtpsMode(bool checked);

    /**
     * @brief 显示关于对话框
     */
    void showAbout();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void connectSignals();

    // UI 组件
    QTabWidget* tab_widget_;
    QListWidget* channel_list_;
    ChannelMonitor* channel_monitor_;
    QStatusBar* status_bar_;
    
    // 菜单动作
    QAction* rtps_action_;
    QAction* refresh_action_;
    QAction* subscribe_action_;
    QAction* about_action_;

    // CyberRT 节点
    std::shared_ptr<CyberNodeWrapper> cyber_node_;
    
    // 显示面板列表
    std::vector<std::unique_ptr<ImageDisplay>> image_displays_;
    std::vector<std::unique_ptr<PointCloudDisplay>> pcd_displays_;
    
    // 定时器
    QTimer* refresh_timer_;
    
    // 配置
    bool rtps_enabled_{false};
    std::string multicast_ip_{"239.255.0.1"};
    int rtps_port_{9090};
};

/**
 * @brief 订阅通道对话框
 */
class SubscribeDialog : public QDialog {
    Q_OBJECT

public:
    explicit SubscribeDialog(const std::vector<ChannelInfo>& channels, QWidget* parent = nullptr);
    
    std::string getSelectedChannel() const { return selected_channel_; }
    std::string getChannelType() const { return channel_type_; }

private slots:
    void onOkClicked();
    void onChannelTypeChanged(int index);

private:
    void setupUI(const std::vector<ChannelInfo>& channels);

    QComboBox* type_combo_;
    QComboBox* channel_combo_;
    QCheckBox* image_check_;
    QCheckBox* pcd_check_;
    
    std::string selected_channel_;
    std::string channel_type_;
};

} // namespace cyber_visualizer

#endif // VISUALIZER_WINDOW_H
