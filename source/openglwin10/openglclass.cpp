////////////////////////////////////////////////////////////////////////////////
// Filename: openglclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "openglclass.h"
#include <QScopeGuard>

OpenGLClass::OpenGLClass()
{
    m_glContext = new QOpenGLContext;
    bool res = m_glContext->create();
}


OpenGLClass::OpenGLClass(const OpenGLClass& other)
{
}


OpenGLClass::~OpenGLClass()
{
    Shutdown();
}


bool OpenGLClass::Initialize(HWND hwnd, int screenWidth, int screenHeight, float screenNear, float screenDepth, bool vsync)
{
    m_window = QWindow::fromWinId(reinterpret_cast<WId>(hwnd));
    if (!m_window)
        return false;
    m_window->setSurfaceType(QSurface::SurfaceType::OpenGLSurface);
    auto format = m_window->format();
    // Turn on or off the vertical sync depending on the input bool value.
    if (vsync)
    {
        format.setSwapInterval(1);
        //glXSwapIntervalEXT(m_display, drawable, 1);
    }
    else
    {
        format.setSwapInterval(0);
        //glXSwapIntervalEXT(m_display, drawable, 0);
    }
    m_window->setFormat(format);
    if (!this->MakeCurrent())
        return false;

    //GLXDrawable drawable;
    float fieldOfView, screenAspect;
    bool result;


    // Store the screen size.
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    // Load the OpenGL extensions that will be used by this program.
    result = LoadExtensionList();
    if(!result)
    {
        return false;
    }

    // Store copies of the display and window pointers.
    //m_display = display;
    //m_hwnd = win;

    // Set the depth buffer to be entirely cleared to 1.0 values.
    this->glClearDepthf(1.0f);

    // Enable depth testing.
    this->glEnable(GL_DEPTH_TEST);

    // Set the polygon winding to clockwise front facing for the left handed system.
    this->glFrontFace(GL_CW);

    // Enable back face culling.
    this->glEnable(GL_CULL_FACE);
    this->glCullFace(GL_BACK);

    // Get the current drawable so we can modify the vertical sync swapping.
    //drawable = glXGetCurrentDrawable();

    // Turn on or off the vertical sync depending on the input bool value.
    if(vsync)
    {
        //glXSwapIntervalEXT(m_display, drawable, 1);
    }
    else
    {
	//glXSwapIntervalEXT(m_display, drawable, 0);
    }

    this->glViewport(GLint(0), GLint(0), GLsizei(screenWidth), GLsizei(screenHeight));

    // Initialize the world/model matrix to the identity matrix.
    BuildIdentityMatrix(m_worldMatrix);

    // Set the field of view and screen aspect ratio.
    fieldOfView = 3.14159265358979323846f / 4.0f;
    screenAspect = (float)screenWidth / (float)screenHeight;

    // Build the perspective projection matrix.
    BuildPerspectiveFovMatrix(m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

    // Create an orthographic projection matrix for 2D rendering.
    BuildOrthoMatrix(m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

    return true;
}


void OpenGLClass::Shutdown()
{
    this->DoneCurrent();
    delete m_glContext;
    m_glContext = nullptr;
    delete m_window;
    m_window = nullptr;
    return;
}


void OpenGLClass::BeginScene(float red, float green, float blue, float alpha)
{
    // Set the color to clear the screen to.
    this->glClearColor(red, green, blue, alpha);

    // Clear the screen and depth buffer.
    this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return;
}


void OpenGLClass::EndScene()
{
    // Present the back buffer to the screen since rendering is complete.
    //glXSwapBuffers(m_display, m_hwnd);
    m_glContext->swapBuffers(m_window);
    return;
}

bool OpenGLClass::MakeCurrent()
{
    return m_glContext->makeCurrent(m_window);
}

void OpenGLClass::DoneCurrent()
{
    if (m_glContext)
        m_glContext->doneCurrent();
}


void OpenGLClass::BuildIdentityMatrix(float* matrix)
{
    matrix[0]  = 1.0f;
    matrix[1]  = 0.0f;
    matrix[2]  = 0.0f;
    matrix[3]  = 0.0f;

    matrix[4]  = 0.0f;
    matrix[5]  = 1.0f;
    matrix[6]  = 0.0f;
    matrix[7]  = 0.0f;

    matrix[8]  = 0.0f;
    matrix[9]  = 0.0f;
    matrix[10] = 1.0f;
    matrix[11] = 0.0f;

    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;

    return;
}


void OpenGLClass::BuildPerspectiveFovMatrix(float* matrix, float fieldOfView, float screenAspect, float screenNear, float screenDepth)
{
    matrix[0]  = 1.0f / (screenAspect * tan(fieldOfView * 0.5f));
    matrix[1]  = 0.0f;
    matrix[2]  = 0.0f;
    matrix[3]  = 0.0f;

    matrix[4]  = 0.0f;
    matrix[5]  = 1.0f / tan(fieldOfView * 0.5f);
    matrix[6]  = 0.0f;
    matrix[7]  = 0.0f;

    matrix[8]  = 0.0f;
    matrix[9]  = 0.0f;
    matrix[10] = screenDepth / (screenDepth - screenNear);
    matrix[11] = 1.0f;

    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = (-screenNear * screenDepth) / (screenDepth - screenNear);
    matrix[15] = 0.0f;

    return;
}


void OpenGLClass::BuildOrthoMatrix(float* matrix, float screenWidth,  float screenHeight, float screenNear, float screenDepth)
{
    matrix[0]  = 2.0f / screenWidth;
    matrix[1]  = 0.0f;
    matrix[2]  = 0.0f;
    matrix[3]  = 0.0f;

    matrix[4]  = 0.0f;
    matrix[5]  = 2.0f / screenHeight;
    matrix[6]  = 0.0f;
    matrix[7]  = 0.0f;

    matrix[8]  = 0.0f;
    matrix[9]  = 0.0f;
    matrix[10] = 1.0f / (screenDepth - screenNear);
    matrix[11] = 0.0f;

    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = screenNear / (screenNear - screenDepth);
    matrix[15] = 1.0f;

    return;
}


void OpenGLClass::GetWorldMatrix(float* matrix)
{
    matrix[0]  = m_worldMatrix[0];
    matrix[1]  = m_worldMatrix[1];
    matrix[2]  = m_worldMatrix[2];
    matrix[3]  = m_worldMatrix[3];

    matrix[4]  = m_worldMatrix[4];
    matrix[5]  = m_worldMatrix[5];
    matrix[6]  = m_worldMatrix[6];
    matrix[7]  = m_worldMatrix[7];

    matrix[8]  = m_worldMatrix[8];
    matrix[9]  = m_worldMatrix[9];
    matrix[10] = m_worldMatrix[10];
    matrix[11] = m_worldMatrix[11];

    matrix[12] = m_worldMatrix[12];
    matrix[13] = m_worldMatrix[13];
    matrix[14] = m_worldMatrix[14];
    matrix[15] = m_worldMatrix[15];

    return;
}


void OpenGLClass::GetProjectionMatrix(float* matrix)
{
    matrix[0]  = m_projectionMatrix[0];
    matrix[1]  = m_projectionMatrix[1];
    matrix[2]  = m_projectionMatrix[2];
    matrix[3]  = m_projectionMatrix[3];

    matrix[4]  = m_projectionMatrix[4];
    matrix[5]  = m_projectionMatrix[5];
    matrix[6]  = m_projectionMatrix[6];
    matrix[7]  = m_projectionMatrix[7];

    matrix[8]  = m_projectionMatrix[8];
    matrix[9]  = m_projectionMatrix[9];
    matrix[10] = m_projectionMatrix[10];
    matrix[11] = m_projectionMatrix[11];

    matrix[12] = m_projectionMatrix[12];
    matrix[13] = m_projectionMatrix[13];
    matrix[14] = m_projectionMatrix[14];
    matrix[15] = m_projectionMatrix[15];

    return;
}


void OpenGLClass::GetOrthoMatrix(float* matrix)
{
    matrix[0]  = m_orthoMatrix[0];
    matrix[1]  = m_orthoMatrix[1];
    matrix[2]  = m_orthoMatrix[2];
    matrix[3]  = m_orthoMatrix[3];

    matrix[4]  = m_orthoMatrix[4];
    matrix[5]  = m_orthoMatrix[5];
    matrix[6]  = m_orthoMatrix[6];
    matrix[7]  = m_orthoMatrix[7];

    matrix[8]  = m_orthoMatrix[8];
    matrix[9]  = m_orthoMatrix[9];
    matrix[10] = m_orthoMatrix[10];
    matrix[11] = m_orthoMatrix[11];

    matrix[12] = m_orthoMatrix[12];
    matrix[13] = m_orthoMatrix[13];
    matrix[14] = m_orthoMatrix[14];
    matrix[15] = m_orthoMatrix[15];

    return;
}


bool OpenGLClass::LoadExtensionList()
{
    initializeOpenGLFunctions();
    return true;
}


void OpenGLClass::MatrixRotationX(float* matrix, float angle)
{
    matrix[0]  = 1.0f;
    matrix[1]  = 0.0f;
    matrix[2]  = 0.0f;
    matrix[3]  = 0.0f;

    matrix[4]  = 0.0f;
    matrix[5]  = cosf(angle);
    matrix[6]  = sinf(angle);
    matrix[7]  = 0.0f;

    matrix[8]  = 0.0f;
    matrix[9]  = -sinf(angle);
    matrix[10] = cosf(angle);
    matrix[11] = 0.0f;

    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;

    return;
}


void OpenGLClass::MatrixRotationY(float* matrix, float angle)
{
    matrix[0]  = cosf(angle);
    matrix[1]  = 0.0f;
    matrix[2]  = -sinf(angle);
    matrix[3]  = 0.0f;

    matrix[4]  = 0.0f;
    matrix[5]  = 1.0f;
    matrix[6]  = 0.0f;
    matrix[7]  = 0.0f;

    matrix[8]  = sinf(angle);
    matrix[9]  = 0.0f;
    matrix[10] = cosf(angle);
    matrix[11] = 0.0f;

    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;

    return;
}


void OpenGLClass::MatrixRotationZ(float* matrix, float angle)
{
    matrix[0]  = cosf(angle);
    matrix[1]  = sinf(angle);
    matrix[2]  = 0.0f;
    matrix[3]  = 0.0f;

    matrix[4]  = -sinf(angle);
    matrix[5]  = cosf(angle);
    matrix[6]  = 0.0f;
    matrix[7]  = 0.0f;

    matrix[8]  = 0.0f;
    matrix[9]  = 0.0f;
    matrix[10] = 1.0f;
    matrix[11] = 0.0f;

    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;

    return;
}


void OpenGLClass::MatrixTranslation(float* matrix, float x, float y, float z)
{
    matrix[0]  = 1.0f;
    matrix[1]  = 0.0f;
    matrix[2]  = 0.0f;
    matrix[3]  = 0.0f;

    matrix[4]  = 0.0f;
    matrix[5]  = 1.0f;
    matrix[6]  = 0.0f;
    matrix[7]  = 0.0f;

    matrix[8]  = 0.0f;
    matrix[9]  = 0.0f;
    matrix[10] = 1.0f;
    matrix[11] = 0.0f;

    matrix[12] = x;
    matrix[13] = y;
    matrix[14] = z;
    matrix[15] = 1.0f;

    return;
}


void OpenGLClass::MatrixScale(float* matrix, float x, float y, float z)
{
    matrix[0]  = x;
    matrix[1]  = 0.0f;
    matrix[2]  = 0.0f;
    matrix[3]  = 0.0f;

    matrix[4]  = 0.0f;
    matrix[5]  = y;
    matrix[6]  = 0.0f;
    matrix[7]  = 0.0f;

    matrix[8]  = 0.0f;
    matrix[9]  = 0.0f;
    matrix[10] = z;
    matrix[11] = 0.0f;

    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;

    return;
}


void OpenGLClass::MatrixTranspose(float* result, float* matrix)
{
    result[0]  = matrix[0];
    result[1]  = matrix[4];
    result[2]  = matrix[8];
    result[3]  = matrix[12];

    result[4]  = matrix[1];
    result[5]  = matrix[5];
    result[6]  = matrix[9];
    result[7]  = matrix[13];

    result[8]  = matrix[2];
    result[9]  = matrix[6];
    result[10] = matrix[10];
    result[11] = matrix[14];

    result[12] = matrix[3];
    result[13] = matrix[7];
    result[14] = matrix[11];
    result[15] = matrix[15];

    return;
}


void OpenGLClass::MatrixMultiply(float* result, float* matrix1, float* matrix2)
{
    result[0]  = (matrix1[0] * matrix2[0]) + (matrix1[1] * matrix2[4]) + (matrix1[2] * matrix2[8]) + (matrix1[3] * matrix2[12]);
    result[1]  = (matrix1[0] * matrix2[1]) + (matrix1[1] * matrix2[5]) + (matrix1[2] * matrix2[9]) + (matrix1[3] * matrix2[13]);
    result[2]  = (matrix1[0] * matrix2[2]) + (matrix1[1] * matrix2[6]) + (matrix1[2] * matrix2[10]) + (matrix1[3] * matrix2[14]);
    result[3]  = (matrix1[0] * matrix2[3]) + (matrix1[1] * matrix2[7]) + (matrix1[2] * matrix2[11]) + (matrix1[3] * matrix2[15]);

    result[4]  = (matrix1[4] * matrix2[0]) + (matrix1[5] * matrix2[4]) + (matrix1[6] * matrix2[8]) + (matrix1[7] * matrix2[12]);
    result[5]  = (matrix1[4] * matrix2[1]) + (matrix1[5] * matrix2[5]) + (matrix1[6] * matrix2[9]) + (matrix1[7] * matrix2[13]);
    result[6]  = (matrix1[4] * matrix2[2]) + (matrix1[5] * matrix2[6]) + (matrix1[6] * matrix2[10]) + (matrix1[7] * matrix2[14]);
    result[7]  = (matrix1[4] * matrix2[3]) + (matrix1[5] * matrix2[7]) + (matrix1[6] * matrix2[11]) + (matrix1[7] * matrix2[15]);

    result[8]  = (matrix1[8] * matrix2[0]) + (matrix1[9] * matrix2[4]) + (matrix1[10] * matrix2[8]) + (matrix1[11] * matrix2[12]);
    result[9]  = (matrix1[8] * matrix2[1]) + (matrix1[9] * matrix2[5]) + (matrix1[10] * matrix2[9]) + (matrix1[11] * matrix2[13]);
    result[10] = (matrix1[8] * matrix2[2]) + (matrix1[9] * matrix2[6]) + (matrix1[10] * matrix2[10]) + (matrix1[11] * matrix2[14]);
    result[11] = (matrix1[8] * matrix2[3]) + (matrix1[9] * matrix2[7]) + (matrix1[10] * matrix2[11]) + (matrix1[11] * matrix2[15]);

    result[12] = (matrix1[12] * matrix2[0]) + (matrix1[13] * matrix2[4]) + (matrix1[14] * matrix2[8]) + (matrix1[15] * matrix2[12]);
    result[13] = (matrix1[12] * matrix2[1]) + (matrix1[13] * matrix2[5]) + (matrix1[14] * matrix2[9]) + (matrix1[15] * matrix2[13]);
    result[14] = (matrix1[12] * matrix2[2]) + (matrix1[13] * matrix2[6]) + (matrix1[14] * matrix2[10]) + (matrix1[15] * matrix2[14]);
    result[15] = (matrix1[12] * matrix2[3]) + (matrix1[13] * matrix2[7]) + (matrix1[14] * matrix2[11]) + (matrix1[15] * matrix2[15]);

    return;
}


void OpenGLClass::MatrixInverse(float* result, float* matrix)
{
    float inverse[16];
    float determinant;
    int i;


    inverse[0] = matrix[5]  * matrix[10] * matrix[15] -
                 matrix[5]  * matrix[11] * matrix[14] -
                 matrix[9]  * matrix[6]  * matrix[15] +
                 matrix[9]  * matrix[7]  * matrix[14] +
                 matrix[13] * matrix[6]  * matrix[11] -
                 matrix[13] * matrix[7]  * matrix[10];

    inverse[4] = -matrix[4]  * matrix[10] * matrix[15] +
                  matrix[4]  * matrix[11] * matrix[14] +
                  matrix[8]  * matrix[6]  * matrix[15] -
                  matrix[8]  * matrix[7]  * matrix[14] -
                  matrix[12] * matrix[6]  * matrix[11] +
                  matrix[12] * matrix[7]  * matrix[10];

    inverse[8] = matrix[4]  * matrix[9] * matrix[15] -
                 matrix[4]  * matrix[11] * matrix[13] -
                 matrix[8]  * matrix[5] * matrix[15] +
                 matrix[8]  * matrix[7] * matrix[13] +
                 matrix[12] * matrix[5] * matrix[11] -
                 matrix[12] * matrix[7] * matrix[9];

    inverse[12] = -matrix[4]  * matrix[9] * matrix[14] +
                   matrix[4]  * matrix[10] * matrix[13] +
                   matrix[8]  * matrix[5] * matrix[14] -
                   matrix[8]  * matrix[6] * matrix[13] -
                   matrix[12] * matrix[5] * matrix[10] +
                   matrix[12] * matrix[6] * matrix[9];

    inverse[1] = -matrix[1]  * matrix[10] * matrix[15] +
                  matrix[1]  * matrix[11] * matrix[14] +
                  matrix[9]  * matrix[2] * matrix[15] -
                  matrix[9]  * matrix[3] * matrix[14] -
                  matrix[13] * matrix[2] * matrix[11] +
                  matrix[13] * matrix[3] * matrix[10];

    inverse[5] = matrix[0]  * matrix[10] * matrix[15] -
                 matrix[0]  * matrix[11] * matrix[14] -
                 matrix[8]  * matrix[2] * matrix[15] +
                 matrix[8]  * matrix[3] * matrix[14] +
                 matrix[12] * matrix[2] * matrix[11] -
                 matrix[12] * matrix[3] * matrix[10];

    inverse[9] = -matrix[0]  * matrix[9] * matrix[15] +
                  matrix[0]  * matrix[11] * matrix[13] +
                  matrix[8]  * matrix[1] * matrix[15] -
                  matrix[8]  * matrix[3] * matrix[13] -
                  matrix[12] * matrix[1] * matrix[11] +
                  matrix[12] * matrix[3] * matrix[9];

    inverse[13] = matrix[0]  * matrix[9] * matrix[14] -
                  matrix[0]  * matrix[10] * matrix[13] -
                  matrix[8]  * matrix[1] * matrix[14] +
                  matrix[8]  * matrix[2] * matrix[13] +
                  matrix[12] * matrix[1] * matrix[10] -
                  matrix[12] * matrix[2] * matrix[9];

    inverse[2] = matrix[1]  * matrix[6] * matrix[15] -
                 matrix[1]  * matrix[7] * matrix[14] -
                 matrix[5]  * matrix[2] * matrix[15] +
                 matrix[5]  * matrix[3] * matrix[14] +
                 matrix[13] * matrix[2] * matrix[7] -
                 matrix[13] * matrix[3] * matrix[6];

    inverse[6] = -matrix[0]  * matrix[6] * matrix[15] +
                  matrix[0]  * matrix[7] * matrix[14] +
                  matrix[4]  * matrix[2] * matrix[15] -
                  matrix[4]  * matrix[3] * matrix[14] -
                  matrix[12] * matrix[2] * matrix[7] +
                  matrix[12] * matrix[3] * matrix[6];

    inverse[10] = matrix[0]  * matrix[5] * matrix[15] -
                  matrix[0]  * matrix[7] * matrix[13] -
                  matrix[4]  * matrix[1] * matrix[15] +
                  matrix[4]  * matrix[3] * matrix[13] +
                  matrix[12] * matrix[1] * matrix[7] -
                  matrix[12] * matrix[3] * matrix[5];

    inverse[14] = -matrix[0]  * matrix[5] * matrix[14] +
                   matrix[0]  * matrix[6] * matrix[13] +
                   matrix[4]  * matrix[1] * matrix[14] -
                   matrix[4]  * matrix[2] * matrix[13] -
                   matrix[12] * matrix[1] * matrix[6] +
                   matrix[12] * matrix[2] * matrix[5];

    inverse[3] = -matrix[1] * matrix[6] * matrix[11] +
                  matrix[1] * matrix[7] * matrix[10] +
                  matrix[5] * matrix[2] * matrix[11] -
                  matrix[5] * matrix[3] * matrix[10] -
                  matrix[9] * matrix[2] * matrix[7] +
                  matrix[9] * matrix[3] * matrix[6];

    inverse[7] = matrix[0] * matrix[6] * matrix[11] -
                 matrix[0] * matrix[7] * matrix[10] -
                 matrix[4] * matrix[2] * matrix[11] +
                 matrix[4] * matrix[3] * matrix[10] +
                 matrix[8] * matrix[2] * matrix[7] -
                 matrix[8] * matrix[3] * matrix[6];

    inverse[11] = -matrix[0] * matrix[5] * matrix[11] +
                   matrix[0] * matrix[7] * matrix[9] +
                   matrix[4] * matrix[1] * matrix[11] -
                   matrix[4] * matrix[3] * matrix[9] -
                   matrix[8] * matrix[1] * matrix[7] +
                   matrix[8] * matrix[3] * matrix[5];

    inverse[15] = matrix[0] * matrix[5] * matrix[10] -
                  matrix[0] * matrix[6] * matrix[9] -
                  matrix[4] * matrix[1] * matrix[10] +
                  matrix[4] * matrix[2] * matrix[9] +
                  matrix[8] * matrix[1] * matrix[6] -
                  matrix[8] * matrix[2] * matrix[5];

    determinant = (matrix[0] * inverse[0]) + (matrix[1] * inverse[4]) + (matrix[2] * inverse[8]) + (matrix[3] * inverse[12]);

    // Prevent divide by zero.
    if(determinant == 0.0f)
    {
        determinant = 0.00001f;
    }

    determinant = 1.0f / determinant;

    for(i=0; i<16; i++)
    {
        result[i] = inverse[i] * determinant;
    }

    return;
}


void OpenGLClass::TurnZBufferOn()
{
    // Enable depth testing.
    this->glEnable(GL_DEPTH_TEST);

    return;
}


void OpenGLClass::TurnZBufferOff()
{
    // Disable depth testing.
    this->glDisable(GL_DEPTH_TEST);

    return;
}
