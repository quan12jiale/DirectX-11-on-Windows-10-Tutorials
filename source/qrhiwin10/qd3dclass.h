#pragma once
#ifdef ENABLE_QRHI
#include "QRhiHelper.h"

class QD3DClass
{
public:
    QD3DClass();
    ~QD3DClass();

    bool Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear);
    void Shutdown();

    void BeginScene(float red, float green, float blue, float alpha);
    void EndScene();
protected:
    void onRenderTick(float red, float green, float blue, float alpha);
protected:
    QRhiHelper::InitParams mInitParams;
    QSharedPointer<QRhi> mRhi;
    QScopedPointer<QRhiSwapChain> mSwapChain;
    QScopedPointer<QRhiRenderBuffer> mDSBuffer;
    QScopedPointer<QRhiRenderPassDescriptor> mSwapChainPassDesc;
    QWindow* m_window = nullptr;
};
#endif
