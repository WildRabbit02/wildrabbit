#ifndef IMAGE_DISPLAY_H
#define IMAGE_DISPLAY_H

#include <QWidget>
#include <QImage>
#include <QLabel>
#include <QString>
#include <QMutex>
#include <memory>

namespace cyber_visualizer {

/**
 * @brief 图像显示组件
 * 
 * 用于显示相机图像数据，支持多种图像格式
 */
class ImageDisplay : public QWidget {
    Q_OBJECT

public:
    explicit ImageDisplay(const std::string& channel_name, QWidget* parent = nullptr);
    ~ImageDisplay();

    /**
     * @brief 更新图像数据
     * @param data 图像原始数据
     * @param width 图像宽度
     * @param height 图像高度
     * @param channels 通道数 (1=灰度，3=RGB, 4=RGBA)
     * @param timestamp 时间戳 (纳秒)
     */
    void updateImage(const void* data, int width, int height, int channels, uint64_t timestamp);

    /**
     * @brief 获取当前通道名称
     */
    std::string getChannelName() const { return channel_name_; }

    /**
     * @brief 获取最后更新时间
     */
    uint64_t getLastUpdateTime() const { return last_timestamp_; }

    /**
     * @brief 获取帧率
     */
    double getFps() const { return fps_; }

public slots:
    /**
     * @brief 清除显示
     */
    void clearDisplay();

private:
    void setupUI();
    void updateInfoLabel();
    QImage convertToQImage(const void* data, int width, int height, int channels);

    // UI 组件
    QLabel* image_label_;
    QLabel* info_label_;
    
    // 数据
    std::string channel_name_;
    uint64_t last_timestamp_{0};
    double fps_{0.0};
    int frame_count_{0};
    
    // 线程安全
    QMutex mutex_;
    QImage current_image_;
};

} // namespace cyber_visualizer

#endif // IMAGE_DISPLAY_H
