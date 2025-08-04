#pragma once
#include "applicationclass.h"

class HelloTriangleIndexed : public ApplicationClass {
public:
	 bool Initialize(int screenWidth, int screenHeight, HWND hwnd) override;
	 void Shutdown() override;
private:
	bool Frame() override;
private:
	OpenGLClass* m_OpenGL;
	unsigned int shaderProgram;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
};
