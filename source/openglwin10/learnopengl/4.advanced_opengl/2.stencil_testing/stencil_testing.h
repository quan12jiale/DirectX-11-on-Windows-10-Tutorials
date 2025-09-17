#pragma once
#include <QVector3D>
#include <QElapsedTimer>
#include "learnopengl/camera.h"
#include <QOpenGLWindow>
#include <QOpenGLExtraFunctions>

class Shader;
class StencilTesting : public QOpenGLWindow, protected QOpenGLExtraFunctions {
public:
	StencilTesting();
	~StencilTesting();
private:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;
	void keyPressEvent(QKeyEvent* ev) override;
	void mouseMoveEvent(QMouseEvent* ev) override;
	void wheelEvent(QWheelEvent* ev) override;
private:
	Shader* ourShader;
	Shader* ourShaderSingleColor;
	unsigned int cubeVAO, cubeVBO;
	unsigned int planeVAO, planeVBO;
	unsigned int cubeTexture;
	unsigned int floorTexture;
	QElapsedTimer elapsed_timer_;
	// camera
	Camera camera;
	bool firstMouse = true;
	float lastX = 800.0f / 2.0;
	float lastY = 600.0 / 2.0;
	// timing
	float deltaTime = 0;	// time between current frame and last frame.unit: s
	qint64 lastFrame = 0;
};
