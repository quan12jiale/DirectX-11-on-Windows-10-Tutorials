////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_

#include <qt_windows.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "openglclass.h"
#include "colorshaderclass.h"
#include "modelclass.h"
#include "cameraclass.h"
#include "lightshaderclass.h"
#include "lightclass.h"

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
	virtual bool Frame();

private:
	bool Render(float rotation);

private:
	OpenGLClass* m_OpenGL;
	ColorShaderClass* m_ColorShader;
	ModelClass* m_Model;
	CameraClass* m_Camera;
	LightShaderClass* m_LightShader;
	LightClass* m_Light;
};

#endif