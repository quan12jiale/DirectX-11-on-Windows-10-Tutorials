//////////////////////////////////////////////////////////////////////////////////
// Filename: videorendererclass.cpp
//////////////////////////////////////////////////////////////////////////////////
#include "videorendererclass.h"

VideoRendererClass::VideoRendererClass()
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
    m_videoTexture = 0;
    m_textureShader = 0;
    m_videoWidth = 0;
    m_videoHeight = 0;
    m_isInitialized = false;
}

VideoRendererClass::VideoRendererClass(const VideoRendererClass& other)
{
}

VideoRendererClass::~VideoRendererClass()
{
}

bool VideoRendererClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    bool result;

    // Initialize the vertex and index buffers
    result = InitializeBuffers(device);
    if (!result)
    {
        return false;
    }

    // Create the video texture object
    m_videoTexture = new VideoFrameTextureClass;
    if (!m_videoTexture)
    {
        return false;
    }

    // Create the texture shader object
    m_textureShader = new TextureShaderClass;
    if (!m_textureShader)
    {
        return false;
    }

    // Initialize the texture shader
    result = m_textureShader->Initialize(device, GetConsoleWindow());
    if (!result)
    {
        return false;
    }

    m_isInitialized = true;
    return true;
}

void VideoRendererClass::Shutdown()
{
    // Release the texture shader
    if (m_textureShader)
    {
        m_textureShader->Shutdown();
        delete m_textureShader;
        m_textureShader = 0;
    }

    // Release the video texture
    if (m_videoTexture)
    {
        delete m_videoTexture;
        m_videoTexture = 0;
    }

    // Release the vertex and index buffers
    ShutdownBuffers();

    m_isInitialized = false;
}

bool VideoRendererClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
    bool result;

    if (!m_isInitialized)
    {
        return false;
    }

    // Put the vertex and index buffers on the graphics pipeline to prepare them for drawing
    RenderBuffers(deviceContext);

    // Render the video frame using the texture shader
    ID3D11ShaderResourceView* textureView = m_videoTexture->GetTexture();
    result = m_textureShader->Render(deviceContext, m_indexCount, worldMatrix, viewMatrix, projectionMatrix, textureView);
    if (!result)
    {
        return false;
    }

    return true;
}

bool VideoRendererClass::InitializeVideoTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)
{
    bool result;

    // Initialize the video texture with the specified dimensions
    result = m_videoTexture->Initialize(device, deviceContext, width, height);
    if (!result)
    {
        return false;
    }

    m_videoWidth = width;
    m_videoHeight = height;

    return true;
}

bool VideoRendererClass::UpdateVideoFrame(ID3D11DeviceContext* deviceContext, unsigned char* frameData, int width, int height)
{
    bool result;

    if (!m_isInitialized)
    {
        return false;
    }

    // Update the video texture with new frame data
    result = m_videoTexture->UpdateFrame(deviceContext, frameData, width, height);
    if (!result)
    {
        return false;
    }

    // Update buffer positions if video size changed
    if (width != m_videoWidth || height != m_videoHeight)
    {
        m_videoWidth = width;
        m_videoHeight = height;
        result = UpdateBuffers(deviceContext);
        if (!result)
        {
            return false;
        }
    }

    return true;
}

bool VideoRendererClass::InitializeBuffers(ID3D11Device* device)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;
    float left, right, top, bottom;

    // Set the number of vertices in the buffer
    m_vertexCount = 6;

    // Set the number of indices in the buffer
    m_indexCount = 6;

    // Create the vertex array
    vertices = new VertexType[m_vertexCount];
    if (!vertices)
    {
        return false;
    }

    // Create the index array
    indices = new unsigned long[m_indexCount];
    if (!indices)
    {
        return false;
    }

    // Calculate the screen coordinates of the video frame (centered)
    left = -1.0f;
    right = 1.0f;
    top = 1.0f;
    bottom = -1.0f;

    // First triangle
    vertices[0].position = XMFLOAT3(left, top, 0.0f);    // Top-left
    vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

    vertices[1].position = XMFLOAT3(right, bottom, 0.0f); // Bottom-right
    vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

    vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom-left
    vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

    // Second triangle
    vertices[3].position = XMFLOAT3(left, top, 0.0f);     // Top-left
    vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

    vertices[4].position = XMFLOAT3(right, top, 0.0f);    // Top-right
    vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

    vertices[5].position = XMFLOAT3(right, bottom, 0.0f); // Bottom-right
    vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

    // Load the index array with data
    indices[0] = 0;  // First triangle
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 3;  // Second triangle
    indices[4] = 4;
    indices[5] = 1;

    // Set up the description of the dynamic vertex buffer
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Now create the vertex buffer
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // Set up the description of the static index buffer
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // Release the arrays now that the vertex and index buffers have been created and loaded
    delete[] vertices;
    vertices = 0;

    delete[] indices;
    indices = 0;

    return true;
}

void VideoRendererClass::ShutdownBuffers()
{
    // Release the index buffer
    if (m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = 0;
    }

    // Release the vertex buffer
    if (m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = 0;
    }
}

bool VideoRendererClass::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
    VertexType* vertices;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    VertexType* verticesPtr;
    HRESULT result;
    float aspectRatio, left, right, top, bottom;

    // Calculate aspect ratio
    aspectRatio = (float)m_videoWidth / (float)m_videoHeight;

    // Calculate normalized device coordinates for the video frame
    // Keep aspect ratio by adjusting width
    if (aspectRatio > 1.0f)
    {
        // Wide video - adjust width
        left = -aspectRatio;
        right = aspectRatio;
        top = 1.0f;
        bottom = -1.0f;
    }
    else
    {
        // Tall video - adjust height
        left = -1.0f;
        right = 1.0f;
        top = 1.0f / aspectRatio;
        bottom = -1.0f / aspectRatio;
    }

    // Create the vertex array
    vertices = new VertexType[m_vertexCount];
    if (!vertices)
    {
        return false;
    }

    // First triangle
    vertices[0].position = XMFLOAT3(left, top, 0.0f);
    vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

    vertices[1].position = XMFLOAT3(right, bottom, 0.0f);
    vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

    vertices[2].position = XMFLOAT3(left, bottom, 0.0f);
    vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

    // Second triangle
    vertices[3].position = XMFLOAT3(left, top, 0.0f);
    vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

    vertices[4].position = XMFLOAT3(right, top, 0.0f);
    vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

    vertices[5].position = XMFLOAT3(right, bottom, 0.0f);
    vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

    // Lock the vertex buffer so it can be written to
    result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        delete[] vertices;
        return false;
    }

    // Get a pointer to the data in the vertex buffer
    verticesPtr = (VertexType*)mappedResource.pData;

    // Copy the data into the vertex buffer
    memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

    // Unlock the vertex buffer
    deviceContext->Unmap(m_vertexBuffer, 0);

    // Release the vertex array
    delete[] vertices;

    return true;
}

void VideoRendererClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride;
    unsigned int offset;

    // Set vertex buffer stride and offset
    stride = sizeof(VertexType);
    offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}