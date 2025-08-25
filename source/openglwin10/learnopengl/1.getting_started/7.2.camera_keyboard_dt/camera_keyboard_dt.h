#pragma once
#include "applicationclass.h"
#include <QVector3D>
#include <QElapsedTimer>

class Shader;
// dt: Delta Time
class CameraKeyboardDt : public ApplicationClass {
public:
	bool Initialize(int screenWidth, int screenHeight, HWND hwnd) override;
	void Shutdown() override;
private:
	bool Frame() override;
	void SetMsg(const MSG& msg) override;
	void processInput();
private:
	OpenGLClass* m_OpenGL;
	Shader* ourShader;
	unsigned int VBO, VAO;
	unsigned int instanceVBO;
	unsigned int texture1, texture2;
	QVector<QVector3D> cubePositions;
	QElapsedTimer elapsed_timer_;
	MSG m_msg;
	// camera
	QVector3D cameraPos = QVector3D(0.0f, 0.0f, 3.0f);
	QVector3D cameraFront = QVector3D(0.0f, 0.0f, -1.0f);
	QVector3D cameraUp = QVector3D(0.0f, 1.0f, 0.0f);
	// timing
	float deltaTime = 0;	// time between current frame and last frame.unit: s
	qint64 lastFrame = 0;
};
