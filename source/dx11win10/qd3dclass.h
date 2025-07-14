#pragma once
#ifdef ENABLE_QRHI
#include "d3dclass.h"
#include "QRhiHelper.h"

class QD3DClass : public D3DClass
{
public:
    QD3DClass();
    ~QD3DClass();

    bool Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear) override;
    void Shutdown() override;

    void BeginScene(float red, float green, float blue, float alpha) override;
    void EndScene() override;
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
