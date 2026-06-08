#ifndef CHANNEL_MONITOR_H
#define CHANNEL_MONITOR_H

#include <QWidget>
#include <QTableWidget>
#include <QTimer>
#include <QStringList>
#include <vector>
#include <string>
#include <memory>

#include "cyber_node_wrapper.h"

namespace cyber_visualizer {

/**
 * @brief 通道监控组件
 * 
 * 显示所有可用通道的实时状态，包括消息频率、类型等
 */
class ChannelMonitor : public QWidget {
    Q_OBJECT

public:
    explicit ChannelMonitor(QWidget* parent = nullptr);
    ~ChannelMonitor();

    /**
     * @brief 设置 CyberRT 节点包装器
     */
    void setCyberNode(std::shared_ptr<CyberNodeWrapper> node);

    /**
     * @brief 刷新通道列表
     */
    void refreshChannels();

    /**
     * @brief 清除所有数据
     */
    void clear();

public slots:
    /**
     * @brief 自动更新通道信息
     */
    void updateChannelInfo();

private:
    void setupUI();
    void populateTable(const std::vector<ChannelInfo>& channels);
    QString formatFrequency(double hz);
    QString formatTimestamp(uint64_t timestamp_ns);

    // UI 组件
    QTableWidget* table_widget_;
    QTimer* update_timer_;
    
    // 数据
    std::shared_ptr<CyberNodeWrapper> cyber_node_;
    std::vector<ChannelInfo> cached_channels_;
    
    // 统计信息
    int total_channels_{0};
    int active_channels_{0};
};

} // namespace cyber_visualizer

#endif // CHANNEL_MONITOR_H
