#pragma once
#include "applicationclass.h"
#include <QElapsedTimer>

class Shader;
// https://github.com/JoeyDeVries/LearnOpenGL/tree/master
class CoordinateSystemsDepth : public ApplicationClass {
public:
	bool Initialize(int screenWidth, int screenHeight, HWND hwnd) override;
	void Shutdown() override;
private:
	bool Frame() override;
private:
	OpenGLClass* m_OpenGL;
	Shader* ourShader;
	unsigned int VBO, VAO;
	unsigned int texture1, texture2;
	QElapsedTimer elapsed_timer_;
};
