////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_

#include <windows.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "openglclass.h"
#include "colorshaderclass.h"
#include "modelclass.h"
#include "cameraclass.h"
#include "textureshaderclass.h"

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;


////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	virtual bool Initialize(int, int, HWND);
	virtual void Shutdown();
	bool Frame();
protected:
	static GLuint compileShader(GLuint program, GLenum shaderStageType, const char* srcStr, OpenGLClass* m_OpenGL);
	static bool checkCompileShaderError(GLuint shader, OpenGLClass* m_OpenGL);
	static bool checkLinkProgramError(GLuint program, OpenGLClass* m_OpenGL);
private:
	virtual bool Render();

private:
	OpenGLClass* m_OpenGL;
	ColorShaderClass* m_ColorShader;
	ModelClass* m_Model;
	CameraClass* m_Camera;
	TextureShaderClass* m_TextureShader;
};

#endif