////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_Light = 0;
	m_VideoRenderer = 0;
	m_IsVideoInitialized = false;
	m_FrameCounter = 0;
	m_TestFrameData = 0;
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


	// Create and initialize the Direct3D object.
	m_Direct3D = new D3DClass;

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// Set the file name of the model.
	strcpy_s(modelFilename, "../Engine/data/sphere.txt");

	// Set the file name of the texture file that we will be loading.
	strcpy_s(textureFilename, "../Engine/data/stone01.tga");

	// Create and initialize the model object.
	m_Model = new ModelClass;

	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the light shader object.
	m_LightShader = new LightShaderClass;

	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the light object.
	m_Light = new LightClass;

	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(1.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);

	// Create and initialize the video renderer
	m_VideoRenderer = new VideoRendererClass;
	if (!m_VideoRenderer)
	{
		return false;
	}

	result = m_VideoRenderer->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	// Initialize test video frame data (640x480 RGBA)
	int frameSize = 640 * 480 * 4;
	m_TestFrameData = new unsigned char[frameSize];
	if (!m_TestFrameData)
	{
		return false;
	}

	// Initialize with test pattern
	CreateTestFrameData(m_TestFrameData, 640, 480);
	m_IsVideoInitialized = true;	
	return true;
}


void ApplicationClass::Shutdown()
{
	// Release the video renderer object.
	if (m_VideoRenderer)
	{
		m_VideoRenderer->Shutdown();
		delete m_VideoRenderer;
		m_VideoRenderer = 0;
	}

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

	// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	// Release test frame data
	if (m_TestFrameData)
	{
		delete[] m_TestFrameData;
		m_TestFrameData = 0;
	}

	return;
}


void ApplicationClass::CreateTestFrameData(unsigned char* frameData, int width, int height)
{
	int frameSize = width * height * 4;

	// Create a test pattern with moving colors
	for (int i = 0; i < frameSize; i += 4)
	{
		int x = (i / 4) % width;
		int y = (i / 4) / width;

		// Create a gradient pattern
		frameData[i] = (unsigned char)((x * 255) / width);     // Red channel
		frameData[i + 1] = (unsigned char)((y * 255) / height); // Green channel
		frameData[i + 2] = 128;                                // Blue channel
		frameData[i + 3] = 255;                                // Alpha channel
	}
}


void ApplicationClass::UpdateTestFrameData(unsigned char* frameData, int width, int height, int frameCounter)
{
	int frameSize = width * height * 4;

	// Create an animated test pattern
	for (int i = 0; i < frameSize; i += 4)
	{
		int x = (i / 4) % width;
		int y = (i / 4) / width;

		// Create moving wave pattern
		float wave = sinf((float)x / 20.0f + frameCounter * 0.1f) * cosf((float)y / 20.0f + frameCounter * 0.1f);
		float intensity = (wave + 1.0f) * 127.5f;

		frameData[i] = (unsigned char)(intensity);     // Red channel
		frameData[i + 1] = (unsigned char)(255 - intensity); // Green channel
		frameData[i + 2] = (unsigned char)((x + y + frameCounter) % 256); // Blue channel
		frameData[i + 3] = 255;                        // Alpha channel
	}
}


bool ApplicationClass::Frame()
{
	static float rotation = 0.0f;
	bool result;

	// Update the rotation variable each frame.
	rotation -= 0.0174532925f * 0.25f;
	if (rotation < 0.0f)
	{
		rotation += 360.0f;
	}

	// Update video frame if video is initialized
	if (m_IsVideoInitialized && m_VideoRenderer)
	{
		UpdateTestFrameData(m_TestFrameData, 640, 480, m_FrameCounter++);
		
		result = m_VideoRenderer->UpdateVideoFrame(m_Direct3D->GetDeviceContext(), m_TestFrameData, 640, 480);
		if (!result)
		{
			return false;
		}
	}

	// Render the graphics scene.
	result = Render(rotation);
	if (!result)
	{
		return false;
	}

	return true;
}


bool ApplicationClass::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Rotate the world matrix by the rotation value so that the model will spin.
	worldMatrix = XMMatrixRotationY(rotation);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// Render the model using the light shader.
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(),
								   m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
								   m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	if (!result)
	{
		return false;
	}

	// Render the video frame if initialized
	if (m_IsVideoInitialized && m_VideoRenderer)
	{
		// Create a simple transformation matrix for the video (no rotation, just position it in front of camera)
		XMMATRIX videoWorldMatrix = XMMatrixIdentity();
		
		// Render the video frame
		result = m_VideoRenderer->Render(m_Direct3D->GetDeviceContext(), videoWorldMatrix, viewMatrix, projectionMatrix);
		if (!result)
		{
			return false;
		}
	}

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}