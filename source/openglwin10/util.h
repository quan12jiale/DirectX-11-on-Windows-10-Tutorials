#pragma once
#include "openglclass.h"

class Util {
public:
	static GLuint compileShader(GLuint program, GLenum shaderStageType, const char* srcStr, OpenGLClass* m_OpenGL);
	static bool checkCompileShaderError(GLuint shader, OpenGLClass* m_OpenGL);
	static bool checkLinkProgramError(GLuint program, OpenGLClass* m_OpenGL);
};