#include "image_display.h"

#include <QVBoxLayout>
#include <QPainter>
#include <QDateTime>
#include <QFont>
#include <QColor>
#include <QFrame>

namespace cyber_visualizer {

ImageDisplay::ImageDisplay(const std::string& channel_name, QWidget* parent)
    : QWidget(parent), channel_name_(channel_name) {
    setupUI();
}

ImageDisplay::~ImageDisplay() {
}

void ImageDisplay::setupUI() {
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // 图像标签
    image_label_ = new QLabel();
    image_label_->setAlignment(Qt::AlignCenter);
    image_label_->setMinimumSize(320, 240);
    image_label_->setStyleSheet("QLabel { background-color: #1a1a1a; color: #888; }");
    image_label_->setText(QString("Waiting for image from\n%1").arg(QString::fromStdString(channel_name_)));
    layout->addWidget(image_label_);
    
    // 信息标签
    info_label_ = new QLabel();
    info_label_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    info_label_->setStyleSheet("QLabel { color: #00ff00; background-color: rgba(0,0,0,0.7); padding: 5px; }");
    updateInfoLabel();
    layout->addWidget(info_label_);
    
    setLayout(layout);
}

void ImageDisplay::updateImage(const void* data, int width, int height, int channels, uint64_t timestamp) {
    QMutexLocker locker(&mutex_);
    
    if (!data || width <= 0 || height <= 0) {
        return;
    }
    
    // 计算帧率
    if (last_timestamp_ > 0) {
        double delta_sec = (timestamp - last_timestamp_) / 1e9;
        if (delta_sec > 0 && delta_sec < 1.0) {
            fps_ = 0.9 * fps_ + 0.1 * (1.0 / delta_sec);
        }
    }
    last_timestamp_ = timestamp;
    frame_count_++;
    
    // 转换图像
    current_image_ = convertToQImage(data, width, height, channels);
    
    // 更新显示
    QImage display_image = current_image_.scaled(
        image_label_->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    );
    
    image_label_->setPixmap(QPixmap::fromImage(display_image));
    updateInfoLabel();
}

QImage ImageDisplay::convertToQImage(const void* data, int width, int height, int channels) {
    const uchar* buffer = static_cast<const uchar*>(data);
    
    switch (channels) {
        case 1: // 灰度图
            return QImage(buffer, width, height, width, QImage::Format_Grayscale8).copy();
        
        case 3: // RGB
            // 需要转换为 QImage 的 RGB888 格式
            {
                QImage img(width, height, QImage::Format_RGB888);
                for (int y = 0; y < height; ++y) {
                    uchar* line = img.scanLine(y);
                    const uchar* src = buffer + y * width * 3;
                    for (int x = 0; x < width * 3; ++x) {
                        line[x] = src[x];
                    }
                }
                return img;
            }
        
        case 4: // RGBA
            return QImage(buffer, width, height, width * 4, QImage::Format_RGBA8888).copy();
        
        default:
            // 未知格式，返回灰度
            return QImage(buffer, width, height, width, QImage::Format_Grayscale8).copy();
    }
}

void ImageDisplay::updateInfoLabel() {
    QString info = QString(
        "<b>Channel:</b> %1<br>"
        "<b>FPS:</b> %2<br>"
        "<b>Frames:</b> %3<br>"
        "<b>Last Update:</b> %4"
    )
    .arg(QString::fromStdString(channel_name_))
    .arg(fps_, 0, 'f', 1)
    .arg(frame_count_)
    .arg(QDateTime::currentDateTime().toString("hh:mm:ss.zzz"));
    
    info_label_->setText(info);
}

void ImageDisplay::clearDisplay() {
    QMutexLocker locker(&mutex_);
    
    image_label_->clear();
    image_label_->setText(QString("Waiting for image from\n%1").arg(QString::fromStdString(channel_name_)));
    image_label_->setStyleSheet("QLabel { background-color: #1a1a1a; color: #888; }");
    
    current_image_ = QImage();
    fps_ = 0.0;
    frame_count_ = 0;
    last_timestamp_ = 0;
    
    updateInfoLabel();
}

} // namespace cyber_visualizer
