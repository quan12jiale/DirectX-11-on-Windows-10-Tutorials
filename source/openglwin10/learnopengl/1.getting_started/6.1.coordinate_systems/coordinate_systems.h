#pragma once
#include "applicationclass.h"

class Shader;
// https://github.com/JoeyDeVries/LearnOpenGL/tree/master
class CoordinateSystems : public ApplicationClass {
public:
	bool Initialize(int screenWidth, int screenHeight, HWND hwnd) override;
	void Shutdown() override;
private:
	bool Frame() override;
private:
	OpenGLClass* m_OpenGL;
	Shader* ourShader;
	unsigned int VBO, VAO, EBO;
	unsigned int texture1, texture2;
	qint64 init_timestamp;
};
