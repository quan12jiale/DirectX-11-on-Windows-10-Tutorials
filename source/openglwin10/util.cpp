#include "util.h"
#include <iostream>

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

unsigned int Util::loadTexture(const char const* path, OpenGLClass* m_OpenGL)
{
	unsigned int textureID;
	m_OpenGL->glGenTextures(1, &textureID);

	QImage image(path);
	if (!image.isNull())
	{
		if (image.format() != QImage::Format_RGB888) {
			image = image.convertToFormat(QImage::Format_RGB888);
		}

		m_OpenGL->glBindTexture(GL_TEXTURE_2D, textureID);
		m_OpenGL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.constBits());
		m_OpenGL->glGenerateMipmap(GL_TEXTURE_2D);

		m_OpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		m_OpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		m_OpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		m_OpenGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
	}

	return textureID;
}
