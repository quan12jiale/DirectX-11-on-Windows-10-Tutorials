#pragma once
#include "applicationclass.h"
#include <QVector3D>
#include <QElapsedTimer>

class Shader;
// https://github.com/JoeyDeVries/LearnOpenGL/tree/master
class CameraCircle : public ApplicationClass {
public:
	bool Initialize(int screenWidth, int screenHeight, HWND hwnd) override;
	void Shutdown() override;
private:
	bool Frame() override;
private:
	OpenGLClass* m_OpenGL;
	Shader* ourShader;
	unsigned int VBO, VAO;
	unsigned int instanceVBO;
	unsigned int texture1, texture2;
	QVector<QVector3D> cubePositions;
	QElapsedTimer elapsed_timer_;
};
