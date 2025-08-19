////////////////////////////////////////////////////////////////////////////////
// Filename: openglclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _OPENGLCLASS_H_
#define _OPENGLCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QWindow>
#include <string.h>
#include <math.h>


////////////////////////////////////////////////////////////////////////////////
// Class Name: OpenGLClass
////////////////////////////////////////////////////////////////////////////////
class OpenGLClass : public QOpenGLExtraFunctions
{
public:
    OpenGLClass();
    OpenGLClass(const OpenGLClass&);
    ~OpenGLClass();
    QOpenGLContext* GetGLContext() const;

    bool Initialize(HWND hwnd, int screenWidth, int screenHeight, float screenNear, float screenDepth, bool vsync);
    bool Initialize2(HWND hwnd, int screenWidth, int screenHeight, float screenNear, float screenDepth, bool vsync);
    void Shutdown();

    void BeginScene(float, float, float, float);
    void EndScene();
    bool MakeCurrent();
    void DoneCurrent();

    void BuildIdentityMatrix(float*);
    void BuildPerspectiveFovMatrix(float*, float, float, float, float);
    void BuildOrthoMatrix(float*, float,  float, float, float);

    void GetWorldMatrix(float*);
    void GetProjectionMatrix(float*);
    void GetOrthoMatrix(float*);

    void MatrixRotationX(float*, float);
    void MatrixRotationY(float*, float);
    void MatrixRotationZ(float*, float);
    void MatrixTranslation(float*, float, float, float);
    void MatrixScale(float*, float, float, float);
    void MatrixTranspose(float*, float*);
    void MatrixMultiply(float*, float*, float*);
    void MatrixInverse(float*, float*);

    void TurnZBufferOn();
    void TurnZBufferOff();

private:
    bool LoadExtensionList();

public:

private:
    //Display* m_display;
    //Window m_hwnd;
    QOpenGLContext* m_glContext = nullptr;
    QWindow* m_window = nullptr;
    float m_worldMatrix[16];
    float m_projectionMatrix[16];
    float m_orthoMatrix[16];
    float m_screenWidth, m_screenHeight;
};

#endif

