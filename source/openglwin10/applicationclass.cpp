////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"


ApplicationClass::ApplicationClass()
{
	m_OpenGL = 0;
	m_ColorShader = 0;
	m_Model = 0;
	m_Camera = 0;
	m_TextureShader = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char textureFilename[128];
	bool result;


	// Create and initialize the OpenGL object.
	m_OpenGL = new OpenGLClass;

	result = m_OpenGL->Initialize(hwnd, screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH, VSYNC_ENABLED);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize OpenGL", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the camera object.
	m_Camera = new CameraClass;

	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	m_Camera->Render();

	// Set the file name of the texture.
	strcpy(textureFilename, "../Engine/data/stone01.tga");

	// Create and initialize the model object.
	m_Model = new ModelClass;

	result = m_Model->Initialize(m_OpenGL, textureFilename, false);
	if (!result)
	{
		cout << "Error: Could not initialize the model object." << endl;
		return false;
	}

	// Create and initialize the texture shader object.
	m_TextureShader = new TextureShaderClass;

	result = m_TextureShader->Initialize(m_OpenGL);
	if (!result)
	{
		cout << "Error: Could not initialize the texture shader object." << endl;
		return false;
	}

	return true;
}


void ApplicationClass::Shutdown()
{
	// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the color shader object.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// Release the OpenGL object.
	if(m_OpenGL)
	{
		m_OpenGL->Shutdown();
		delete m_OpenGL;
		m_OpenGL = 0;
	}

	return;
}


bool ApplicationClass::Frame()
{
	bool result;


	// Render the graphics scene.
	result = Render();
	if(!result)
	{
		return false;
	}

	return true;
}

GLuint ApplicationClass::compileShader(GLuint program, GLenum shaderStageType, const char* srcStr, OpenGLClass* m_OpenGL)
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

bool ApplicationClass::checkCompileShaderError(GLuint shader, OpenGLClass* m_OpenGL)
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

bool ApplicationClass::checkLinkProgramError(GLuint program, OpenGLClass* m_OpenGL)
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


bool ApplicationClass::Render()
{
	float worldMatrix[16], viewMatrix[16], projectionMatrix[16];
	bool result;


	// Clear the buffers to begin the scene.
	m_OpenGL->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Get the world, view, and projection matrices from the opengl and camera objects.
	m_OpenGL->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_OpenGL->GetProjectionMatrix(projectionMatrix);

	// Set the texture shader as the current shader program and set the matrices that it will use for rendering.
	result = m_TextureShader->SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// Set the texture for the model in the pixel shader.
	m_Model->SetTexture(0);

	// Render the model.
	m_Model->Render();

	// Present the rendered scene to the screen.
	m_OpenGL->EndScene();

	return true;
}