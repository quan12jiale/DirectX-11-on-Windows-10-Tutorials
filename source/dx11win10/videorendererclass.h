//////////////////////////////////////////////////////////////////////////////////
// Filename: videorendererclass.h
//////////////////////////////////////////////////////////////////////////////////
#ifndef _VIDEORENDERERCLASS_H_
#define _VIDEORENDERERCLASS_H_

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include "videoframetextureclass.h"
#include "textureshaderclass.h"

//////////////////////////////////////////////////////////////////////////////////
// Class name: VideoRendererClass
//////////////////////////////////////////////////////////////////////////////////
class VideoRendererClass
{
private:
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
    };

public:
    VideoRendererClass();
    VideoRendererClass(const VideoRendererClass&);
    ~VideoRendererClass();

    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
    void Shutdown();
    bool Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

    bool UpdateVideoFrame(ID3D11DeviceContext* deviceContext, unsigned char* frameData, int width, int height);
    bool InitializeVideoTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height);

    int GetVideoWidth() const { return m_videoWidth; }
    int GetVideoHeight() const { return m_videoHeight; }

private:
    bool InitializeBuffers(ID3D11Device* device);
    void ShutdownBuffers();
    bool UpdateBuffers(ID3D11DeviceContext* deviceContext);
    void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
    ID3D11Buffer* m_vertexBuffer;
    ID3D11Buffer* m_indexBuffer;
    int m_vertexCount;
    int m_indexCount;

    VideoFrameTextureClass* m_videoTexture;
    TextureShaderClass* m_textureShader;

    int m_videoWidth, m_videoHeight;
    bool m_isInitialized;
};

#endif