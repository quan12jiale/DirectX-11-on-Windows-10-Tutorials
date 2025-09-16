#pragma once
#include "openglclass.h"

class Util {
public:
	static GLuint compileShader(GLuint program, GLenum shaderStageType, const char* srcStr, QOpenGLExtraFunctions* m_OpenGL);
	static bool checkCompileShaderError(GLuint shader, QOpenGLExtraFunctions* m_OpenGL);
	static bool checkLinkProgramError(GLuint program, QOpenGLExtraFunctions* m_OpenGL);
	static unsigned int loadTexture(const char const* path, QOpenGLExtraFunctions* m_OpenGL);
};