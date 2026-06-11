#include "image_display.h"
#include <QVBoxLayout>

ImageDisplay::ImageDisplay(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    
    imageLabel_ = new QLabel("No image received");
    imageLabel_->setAlignment(Qt::AlignCenter);
    imageLabel_->setMinimumSize(640, 480);
    imageLabel_->setStyleSheet("QLabel { background-color: #333; color: #fff; }");
    
    layout->addWidget(imageLabel_);
}

void ImageDisplay::updateImage() {
    // Placeholder: In real implementation, convert CyberRT Image message to QImage
    if (!currentImage_.isNull()) {
        imageLabel_->setPixmap(QPixmap::fromImage(currentImage_));
    } else {
        imageLabel_->setText("Receiving image data...");
    }
}
