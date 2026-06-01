#ifndef POINTCLOUD_DISPLAY_H
#define POINTCLOUD_DISPLAY_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QString>
#include <QMutex>
#include <vector>
#include <memory>

namespace cyber_visualizer {

class PointCloudDisplay : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit PointCloudDisplay(const std::string& channel_name, QWidget* parent = nullptr);
    ~PointCloudDisplay();

    void updatePointCloud(const float* points, int point_count, uint64_t timestamp);
    std::string getChannelName() const { return channel_name_; }
    uint64_t getLastUpdateTime() const { return last_timestamp_; }
    void setPointSize(float size);
    void setAutoRotate(bool enabled);

public slots:
    void clearDisplay();
    void resetView();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void setupShaders();
    void setupBuffers();
    void updateBuffers();
    void drawPointCloud();
    QMatrix4x4 createProjectionMatrix() const;
    QMatrix4x4 createViewMatrix() const;

    QOpenGLShaderProgram* shader_program_{nullptr};
    GLuint vbo_{0};
    GLuint vao_{0};
    float point_size_{2.0f};
    float rotation_x_{-20.0f};
    float rotation_y_{0.0f};
    float zoom_{50.0f};
    bool auto_rotate_{true};
    QPoint last_mouse_pos_;
    bool mouse_pressed_{false};
    std::string channel_name_;
    std::vector<float> points_;
    int point_count_{0};
    uint64_t last_timestamp_{0};
    double fps_{0.0};
    int frame_count_{0};
    QMutex mutex_;
    bool data_updated_{false};
};

} // namespace cyber_visualizer

#endif // POINTCLOUD_DISPLAY_H
