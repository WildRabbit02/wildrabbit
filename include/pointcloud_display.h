#ifndef POINTCLOUD_DISPLAY_H
#define POINTCLOUD_DISPLAY_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QVector3D>

class PointCloudDisplay : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit PointCloudDisplay(QWidget* parent = nullptr);
    
public slots:
    void updatePointCloud();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    std::vector<QVector3D> points_;
    float rotation_ = 0.0f;
};

#endif // POINTCLOUD_DISPLAY_H
