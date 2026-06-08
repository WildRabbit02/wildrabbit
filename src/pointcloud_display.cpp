#include "pointcloud_display.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDateTime>

namespace cyber_visualizer {

PointCloudDisplay::PointCloudDisplay(const std::string& channel_name, QWidget* parent)
    : QOpenGLWidget(parent), channel_name_(channel_name) {
    setMinimumSize(400, 300);
    setAutoRotate(true);
}

PointCloudDisplay::~PointCloudDisplay() {
    makeCurrent();
    if (shader_program_) {
        delete shader_program_;
    }
    if (vbo_) {
        glDeleteBuffers(1, &vbo_);
    }
    if (vao_) {
        glDeleteVertexArrays(1, &vao_);
    }
    doneCurrent();
}

void PointCloudDisplay::updatePointCloud(const float* points, int point_count, uint64_t timestamp) {
    QMutexLocker locker(&mutex_);
    
    if (!points || point_count <= 0) {
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
    
    // 更新点云数据
    points_.assign(points, points + point_count * 3);
    point_count_ = point_count;
    data_updated_ = true;
    
    // 触发重绘
    update();
}

void PointCloudDisplay::setPointSize(float size) {
    point_size_ = size;
    update();
}

void PointCloudDisplay::setAutoRotate(bool enabled) {
    auto_rotate_ = enabled;
}

void PointCloudDisplay::clearDisplay() {
    QMutexLocker locker(&mutex_);
    points_.clear();
    point_count_ = 0;
    fps_ = 0.0;
    frame_count_ = 0;
    last_timestamp_ = 0;
    data_updated_ = false;
    update();
}

void PointCloudDisplay::resetView() {
    rotation_x_ = -20.0f;
    rotation_y_ = 0.0f;
    zoom_ = 50.0f;
    update();
}

void PointCloudDisplay::initializeGL() {
    initializeOpenGLFunctions();
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    
    setupShaders();
    setupBuffers();
}

void PointCloudDisplay::setupShaders() {
    shader_program_ = new QOpenGLShaderProgram(this);
    
    // 顶点着色器
    const char* vertexSource = R"(
        attribute vec3 vertex;
        uniform mat4 mvp_matrix;
        void main() {
            gl_Position = mvp_matrix * vec4(vertex, 1.0);
            gl_PointSize = 2.0;
        }
    )";
    
    // 片段着色器
    const char* fragmentSource = R"(
        void main() {
            gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
        }
    )";
    
    shader_program_->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexSource);
    shader_program_->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentSource);
    shader_program_->link();
}

void PointCloudDisplay::setupBuffers() {
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    
    shader_program_->setAttributeBuffer("vertex", GL_FLOAT, 0, 3);
    shader_program_->enableAttributeArray("vertex");
    
    glBindVertexArray(0);
}

void PointCloudDisplay::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void PointCloudDisplay::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (points_.empty() || !shader_program_->isLinked()) {
        return;
    }
    
    shader_program_->bind();
    
    // 设置 MVP 矩阵
    QMatrix4x4 projection = createProjectionMatrix();
    QMatrix4x4 view = createViewMatrix();
    QMatrix4x4 mvp = projection * view;
    shader_program_->setUniformValue("mvp_matrix", mvp);
    
    // 自动旋转
    if (auto_rotate_) {
        rotation_y_ += 0.5f;
    }
    
    // 绘制点云
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    
    // 更新缓冲区数据
    glBufferData(GL_ARRAY_BUFFER, points_.size() * sizeof(float), points_.data(), GL_DYNAMIC_DRAW);
    
    glDrawArrays(GL_POINTS, 0, point_count_);
    
    glBindVertexArray(0);
    shader_program_->release();
}

QMatrix4x4 PointCloudDisplay::createProjectionMatrix() const {
    QMatrix4x4 projection;
    projection.perspective(45.0f, static_cast<float>(width()) / height(), 0.1f, 1000.0f);
    return projection;
}

QMatrix4x4 PointCloudDisplay::createViewMatrix() const {
    QMatrix4x4 view;
    view.translate(0.0f, 0.0f, -zoom_);
    view.rotate(rotation_x_, 1.0f, 0.0f, 0.0f);
    view.rotate(rotation_y_, 0.0f, 1.0f, 0.0f);
    return view;
}

void PointCloudDisplay::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        mouse_pressed_ = true;
        last_mouse_pos_ = event->pos();
    }
}

void PointCloudDisplay::mouseMoveEvent(QMouseEvent* event) {
    if (mouse_pressed_) {
        QPoint delta = event->pos() - last_mouse_pos_;
        rotation_y_ += delta.x() * 0.5f;
        rotation_x_ += delta.y() * 0.5f;
        rotation_x_ = qBound(-89.0f, rotation_x_, 89.0f);
        last_mouse_pos_ = event->pos();
        update();
    }
}

void PointCloudDisplay::wheelEvent(QWheelEvent* event) {
    zoom_ -= event->angleDelta().y() * 0.05f;
    zoom_ = qBound(10.0f, zoom_, 200.0f);
    update();
}

} // namespace cyber_visualizer
