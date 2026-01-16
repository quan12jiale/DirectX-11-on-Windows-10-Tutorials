//////////////////////////////////////////////////////////////////////////////////
// Filename: videoframetextureclass.h
//////////////////////////////////////////////////////////////////////////////////
#ifndef _VIDEOFRAMETEXTURECLASS_H_
#define _VIDEOFRAMETEXTURECLASS_H_

//////////////
// INCLUDES //
//////////////
#include "textureclass.h"

//////////////////////////////////////////////////////////////////////////////////
// Class name: VideoFrameTextureClass
//////////////////////////////////////////////////////////////////////////////////
class VideoFrameTextureClass : public TextureClass
{
public:
    VideoFrameTextureClass();
    VideoFrameTextureClass(const VideoFrameTextureClass&);
    ~VideoFrameTextureClass();

    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height);
    void Shutdown();

    bool UpdateFrame(ID3D11DeviceContext* deviceContext, unsigned char* frameData, int width, int height);
    ID3D11ShaderResourceView* GetTexture();
    int GetWidth();
    int GetHeight();
private:
    void ReleaseTexture();
    bool CreateTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height);

private:
    ID3D11Texture2D* m_dynamicTexture;
    ID3D11ShaderResourceView* m_dynamicTextureView;
    int m_frameWidth, m_frameHeight;
};

#endif