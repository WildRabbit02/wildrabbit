#ifndef IMAGE_DISPLAY_H
#define IMAGE_DISPLAY_H

#include <QWidget>
#include <QLabel>
#include <QImage>

class ImageDisplay : public QWidget {
    Q_OBJECT

public:
    explicit ImageDisplay(QWidget* parent = nullptr);
    
public slots:
    void updateImage();

private:
    QLabel* imageLabel_;
    QImage currentImage_;
};

#endif // IMAGE_DISPLAY_H
