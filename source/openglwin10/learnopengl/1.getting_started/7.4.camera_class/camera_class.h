#pragma once
#include "applicationclass.h"
#include <QVector3D>
#include <QElapsedTimer>
#include "learnopengl/camera.h"

class Shader;
class LearnCameraClass : public ApplicationClass {
public:
	LearnCameraClass();
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
	Camera camera;
	bool firstMouse = true;
	float lastX = 800.0f / 2.0;
	float lastY = 600.0 / 2.0;
	// timing
	float deltaTime = 0;	// time between current frame and last frame.unit: s
	qint64 lastFrame = 0;
};
