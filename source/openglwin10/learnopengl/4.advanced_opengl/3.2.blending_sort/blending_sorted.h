#pragma once
#include <QElapsedTimer>
#include "learnopengl/camera.h"
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>

class Shader;
class BlendingSorted : public QOpenGLWidget, protected QOpenGLExtraFunctions {
public:
	BlendingSorted();
	~BlendingSorted();
private:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;
	void keyPressEvent(QKeyEvent* ev) override;
	void mouseMoveEvent(QMouseEvent* ev) override;
	void wheelEvent(QWheelEvent* ev) override;
private:
	Shader* ourShader;
	unsigned int cubeVAO, cubeVBO;
	unsigned int planeVAO, planeVBO;
	unsigned int transparentVAO, transparentVBO;
	unsigned int cubeTexture;
	unsigned int floorTexture;
	unsigned int transparentTexture;
	QVector<QVector3D> windows;
	QElapsedTimer elapsed_timer_;
	// camera
	Camera camera;
	bool firstMouse = true;
	float lastX = 800.0f / 2.0;
	float lastY = 600.0f / 2.0;
	// timing
	float deltaTime = 0;	// time between current frame and last frame.unit: s
	qint64 lastFrame = 0;
};

