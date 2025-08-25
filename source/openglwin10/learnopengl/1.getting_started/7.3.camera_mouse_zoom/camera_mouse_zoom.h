#pragma once
#include "applicationclass.h"
#include <QVector3D>
#include <QElapsedTimer>

class Shader;
class CameraMouseZoom : public ApplicationClass {
public:
	bool Initialize(int screenWidth, int screenHeight, HWND hwnd) override;
	void Shutdown() override;
private:
	bool Frame() override;
	void SetMsg(const MSG& msg) override;
	void processInput();
	void mouse_callback();
	void scroll_callback();
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
	bool firstMouse = true;
	float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	float pitch = 0.0f;
	float lastX = 800.0f / 2.0;
	float lastY = 600.0 / 2.0;
	float fov = 45.0f;
	// timing
	float deltaTime = 0;	// time between current frame and last frame.unit: s
	qint64 lastFrame = 0;
};
