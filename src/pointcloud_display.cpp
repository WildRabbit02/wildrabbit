#include "pointcloud_display.h"
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

PointCloudDisplay::PointCloudDisplay(QWidget* parent) 
    : QOpenGLWidget(parent), rotation_(0.0f) {}

void PointCloudDisplay::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(2.0f);
}

void PointCloudDisplay::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void PointCloudDisplay::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    QMatrix4x4 model;
    model.rotate(rotation_, 0.0f, 1.0f, 0.0f);
    rotation_ += 0.5f;
    
    QMatrix4x4 view;
    view.translate(0.0f, 0.0f, -10.0f);
    
    QMatrix4x4 projection;
    projection.perspective(45.0f, (float)width() / height(), 0.1f, 100.0f);
    
    // Draw points (placeholder - real implementation would use vertex buffers)
    glBegin(GL_POINTS);
    glColor3f(0.0f, 1.0f, 0.0f);
    for (const auto& point : points_) {
        glVertex3f(point.x(), point.y(), point.z());
    }
    glEnd();
}

void PointCloudDisplay::updatePointCloud() {
    // Placeholder: In real implementation, parse CyberRT PointCloud message
    // and populate points_ vector
    update(); // Trigger repaint
}
