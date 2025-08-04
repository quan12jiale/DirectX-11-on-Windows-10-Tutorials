#include "util.h"

GLuint Util::compileShader(GLuint program, GLenum shaderStageType, const char* srcStr, OpenGLClass* m_OpenGL)
{
	GLuint shader = m_OpenGL->glCreateShader(shaderStageType);
	m_OpenGL->glShaderSource(shader, 1, &srcStr, nullptr);
	m_OpenGL->glCompileShader(shader);
	if (!checkCompileShaderError(shader, m_OpenGL)) {
		return shader;
	}
	m_OpenGL->glAttachShader(program, shader);
	return shader;
}

bool Util::checkCompileShaderError(GLuint shader, OpenGLClass* m_OpenGL)
{
	GLint compiled = 0;
	m_OpenGL->glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (compiled != GL_TRUE) {
		GLint infoLogLength = 0;
		m_OpenGL->glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		QByteArray log;
		if (infoLogLength > 1) {
			GLsizei length = 0;
			log.resize(infoLogLength);
			m_OpenGL->glGetShaderInfoLog(shader, infoLogLength, &length, log.data());
		}
		qWarning("Failed to compile shader: %s", log.constData());
		return false;
	}
	return true;
}

bool Util::checkLinkProgramError(GLuint program, OpenGLClass* m_OpenGL)
{
	GLint linked = 0;
	m_OpenGL->glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (linked != GL_TRUE) {
		GLint infoLogLength = 0;
		m_OpenGL->glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		QByteArray log;
		if (infoLogLength > 1) {
			GLsizei length = 0;
			log.resize(infoLogLength);
			m_OpenGL->glGetProgramInfoLog(program, infoLogLength, &length, log.data());
		}
		qWarning("Failed to link shader program: %s", log.constData());
		return false;
	}
	return true;
}
