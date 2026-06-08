#include "channel_monitor.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QDateTime>

namespace cyber_visualizer {

ChannelMonitor::ChannelMonitor(QWidget* parent)
    : QWidget(parent) {
    setupUI();
    
    update_timer_ = new QTimer(this);
    connect(update_timer_, &QTimer::timeout, this, &ChannelMonitor::updateChannelInfo);
    update_timer_->start(1000);  // 每秒更新一次
}

ChannelMonitor::~ChannelMonitor() {
}

void ChannelMonitor::setupUI() {
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    table_widget_ = new QTableWidget();
    table_widget_->setColumnCount(4);
    table_widget_->setHorizontalHeaderLabels(QStringList() 
        << "Channel Name" << "Message Type" << "Frequency" << "Last Update");
    table_widget_->horizontalHeader()->setStretchLastSection(true);
    table_widget_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    table_widget_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_widget_->setAlternatingRowColors(true);
    
    layout->addWidget(table_widget_);
    setLayout(layout);
}

void ChannelMonitor::setCyberNode(std::shared_ptr<CyberNodeWrapper> node) {
    cyber_node_ = node;
    refreshChannels();
}

void ChannelMonitor::refreshChannels() {
    if (!cyber_node_) return;
    
    cached_channels_ = cyber_node_->getAvailableChannels();
    populateTable(cached_channels_);
}

void ChannelMonitor::populateTable(const std::vector<ChannelInfo>& channels) {
    table_widget_->setRowCount(static_cast<int>(channels.size()));
    
    int row = 0;
    for (const auto& ch : channels) {
        table_widget_->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(ch.name)));
        table_widget_->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(ch.message_type)));
        table_widget_->setItem(row, 2, new QTableWidgetItem(formatFrequency(ch.frequency)));
        table_widget_->setItem(row, 3, new QTableWidgetItem(formatTimestamp(ch.last_timestamp)));
        row++;
    }
    
    total_channels_ = static_cast<int>(channels.size());
    active_channels_ = 0;
    
    for (const auto& ch : channels) {
        if (ch.frequency > 0) {
            active_channels_++;
        }
    }
}

void ChannelMonitor::updateChannelInfo() {
    if (!cyber_node_) return;
    
    // 更新频率统计（简化实现）
    for (auto& ch : cached_channels_) {
        // 实际应用中需要根据消息计数计算频率
        if (ch.message_count > 0) {
            ch.frequency = ch.message_count;  // 简化处理
        }
    }
    
    populateTable(cached_channels_);
}

void ChannelMonitor::clear() {
    table_widget_->setRowCount(0);
    cached_channels_.clear();
    total_channels_ = 0;
    active_channels_ = 0;
}

QString ChannelMonitor::formatFrequency(double hz) {
    if (hz <= 0) {
        return "--";
    }
    return QString("%1 Hz").arg(hz, 0, 'f', 1);
}

QString ChannelMonitor::formatTimestamp(uint64_t timestamp_ns) {
    if (timestamp_ns == 0) {
        return "--";
    }
    
    qint64 ms = timestamp_ns / 1000000;
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(ms);
    return dt.toString("hh:mm:ss.zzz");
}

} // namespace cyber_visualizer
