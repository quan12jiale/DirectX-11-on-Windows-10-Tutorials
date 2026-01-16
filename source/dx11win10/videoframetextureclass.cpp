//////////////////////////////////////////////////////////////////////////////////
// Filename: videoframetextureclass.cpp
//////////////////////////////////////////////////////////////////////////////////
#include "videoframetextureclass.h"

VideoFrameTextureClass::VideoFrameTextureClass()
{
    m_dynamicTexture = 0;
    m_dynamicTextureView = 0;
    m_frameWidth = 0;
    m_frameHeight = 0;
}

VideoFrameTextureClass::VideoFrameTextureClass(const VideoFrameTextureClass& other)
{
}

VideoFrameTextureClass::~VideoFrameTextureClass()
{
}

bool VideoFrameTextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)
{
    bool result;

    // Store the frame dimensions
    m_frameWidth = width;
    m_frameHeight = height;

    // Create the dynamic texture for video frames
    result = CreateTexture(device, deviceContext, width, height);
    if (!result)
    {
        return false;
    }

    return true;
}

void VideoFrameTextureClass::Shutdown()
{
    // Release the dynamic texture view
    if (m_dynamicTextureView)
    {
        m_dynamicTextureView->Release();
        m_dynamicTextureView = 0;
    }

    // Release the dynamic texture
    if (m_dynamicTexture)
    {
        m_dynamicTexture->Release();
        m_dynamicTexture = 0;
    }

    // Call base class shutdown
    TextureClass::Shutdown();
}

bool VideoFrameTextureClass::UpdateFrame(ID3D11DeviceContext* deviceContext, unsigned char* frameData, int width, int height)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    unsigned char* mappedData;
    int rowPitch;

    // Check if the texture needs to be recreated due to size change
    if (width != m_frameWidth || height != m_frameHeight)
    {
        // Recreate texture with new dimensions
        ReleaseTexture();
        m_frameWidth = width;
        m_frameHeight = height;
        
        // Get device and recreate texture
        ID3D11Device* device;
        deviceContext->GetDevice(&device);
        bool result = CreateTexture(device, deviceContext, width, height);
        device->Release();
        
        if (!result)
        {
            return false;
        }
    }

    // Calculate row pitch (4 bytes per pixel for RGBA)
    rowPitch = width * 4 * sizeof(unsigned char);

    // Map the texture for writing
    result = deviceContext->Map(m_dynamicTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        return false;
    }

    // Get pointer to mapped texture data
    mappedData = (unsigned char*)mappedResource.pData;

    // Copy frame data to texture (assuming frame data is RGBA format)
    memcpy(mappedData, frameData, rowPitch * height);

    // Unmap the texture
    deviceContext->Unmap(m_dynamicTexture, 0);

    return true;
}

ID3D11ShaderResourceView* VideoFrameTextureClass::GetTexture()
{
    // Return the dynamic texture view instead of base class texture
    return m_dynamicTextureView;
}

int VideoFrameTextureClass::GetWidth()
{
    return m_frameWidth;
}

int VideoFrameTextureClass::GetHeight()
{
    return m_frameHeight;
}

void VideoFrameTextureClass::ReleaseTexture()
{
    if (m_dynamicTextureView)
    {
        m_dynamicTextureView->Release();
        m_dynamicTextureView = 0;
    }

    if (m_dynamicTexture)
    {
        m_dynamicTexture->Release();
        m_dynamicTexture = 0;
    }
}

bool VideoFrameTextureClass::CreateTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)
{
    HRESULT result;
    D3D11_TEXTURE2D_DESC textureDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

    // Setup the description of the dynamic texture
    ZeroMemory(&textureDesc, sizeof(textureDesc));
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DYNAMIC;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    textureDesc.MiscFlags = 0;

    // Create the dynamic texture
    result = device->CreateTexture2D(&textureDesc, NULL, &m_dynamicTexture);
    if (FAILED(result))
    {
        return false;
    }

    // Setup the shader resource view description
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    // Create the shader resource view for the dynamic texture
    result = device->CreateShaderResourceView(m_dynamicTexture, &srvDesc, &m_dynamicTextureView);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}