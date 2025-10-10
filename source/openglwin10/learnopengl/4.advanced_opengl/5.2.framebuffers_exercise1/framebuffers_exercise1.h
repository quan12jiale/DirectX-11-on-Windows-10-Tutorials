#pragma once
#include <QElapsedTimer>
#include "learnopengl/camera.h"
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>

class Shader;
class FrameBuffersExercise1 : public QOpenGLWidget, protected QOpenGLExtraFunctions {
public:
	FrameBuffersExercise1();
	~FrameBuffersExercise1();
private:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;
	void keyPressEvent(QKeyEvent* ev) override;
	void mouseMoveEvent(QMouseEvent* ev) override;
	void wheelEvent(QWheelEvent* ev) override;
private:
	Shader* shader;
	Shader* screenShader;
	unsigned int cubeVAO, cubeVBO;
	unsigned int planeVAO, planeVBO;
	unsigned int quadVAO, quadVBO;
	unsigned int cubeTexture;
	unsigned int floorTexture;
	unsigned int framebuffer;
	unsigned int textureColorbuffer;
	unsigned int rbo;
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
