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
	m_LightShader = 0;
	m_Light = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char modelFilename[128];
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

	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();

	// Set the file name of the model.
	strcpy(modelFilename, "../Engine/data/cube.txt");

	// Set the file name of the texture.
	strcpy(textureFilename, "../Engine/data/stone01.tga");

	// Create and initialize the model object.
	m_Model = new ModelClass;

	result = m_Model->Initialize(m_OpenGL, modelFilename, textureFilename, false);
	if (!result)
	{
		cout << "Error: Could not initialize the model object." << endl;
		return false;
	}

	// Create and initialize the light shader object.
	m_LightShader = new LightShaderClass;

	result = m_LightShader->Initialize(m_OpenGL);
	if (!result)
	{
		cout << "Error: Could not initialize the light shader object." << endl;
		return false;
	}
	
	// Create and initialize the light object.
	m_Light = new LightClass;

	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	return true;
}


void ApplicationClass::Shutdown()
{
	// Release the light object.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the light shader object.
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
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
	static float rotation = 360.0f;
	bool result;

	// Update the rotation variable each frame.
	rotation -= 0.0174532925f * 0.25f;
	if (rotation <= 0.0f)
	{
		rotation += 360.0f;
	}

	// Render the graphics scene.
	result = Render(rotation);
	if(!result)
	{
		return false;
	}

	return true;
}


bool ApplicationClass::Render(float rotation)
{
	float worldMatrix[16], viewMatrix[16], projectionMatrix[16], rotateMatrix[16], translateMatrix[16], scaleMatrix[16], srMatrix[16];
	float diffuseLightColor[4], lightDirection[3];
	bool result;


	// Clear the buffers to begin the scene.
	m_OpenGL->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Get the world, view, and projection matrices from the opengl and camera objects.
	m_OpenGL->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_OpenGL->GetProjectionMatrix(projectionMatrix);

	// Rotate the world matrix by the rotation value so that the triangle will spin.
	m_OpenGL->MatrixRotationY(worldMatrix, rotation);

	// Get the light properties.
	m_Light->GetDirection(lightDirection);
	m_Light->GetDiffuseColor(diffuseLightColor);

	m_OpenGL->MatrixRotationY(rotateMatrix, rotation);  // Build the rotation matrix.
	m_OpenGL->MatrixTranslation(translateMatrix, -2.0f, 0.0f, 0.0f);  // Build the translation matrix.

	// Multiply them together to create the final world transformation matrix.
	m_OpenGL->MatrixMultiply(worldMatrix, rotateMatrix, translateMatrix);

	// Set the light shader as the current shader program and set the matrices and light values that it will use for rendering.
	result = m_LightShader->SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix, lightDirection, diffuseLightColor);
	if (!result)
	{
		return false;
	}

	// Set the texture for the model in the pixel shader.
	m_Model->SetTexture(0);

	// Render the model.
	m_Model->Render();

	m_OpenGL->MatrixScale(scaleMatrix, 0.5f, 0.5f, 0.5f);  // Build the scaling matrix.
	m_OpenGL->MatrixRotationY(rotateMatrix, rotation);  // Build the rotation matrix.
	m_OpenGL->MatrixTranslation(translateMatrix, 2.0f, 0.0f, 0.0f);  // Build the translation matrix.

	// Multiply the scale, rotation, and translation matrices together to create the final world transformation matrix.
	m_OpenGL->MatrixMultiply(srMatrix, scaleMatrix, rotateMatrix);
	m_OpenGL->MatrixMultiply(worldMatrix, srMatrix, translateMatrix);

	// Set the light shader as the current shader program and set the updated matrices and light values that it will use for rendering.
	result = m_LightShader->SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix, lightDirection, diffuseLightColor);
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