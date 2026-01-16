# DirectX 11 视频帧渲染教程

## 概述

本教程展示了如何使用DirectX 11将视频的一帧渲染到画面上。我们创建了一个完整的视频帧渲染系统，包括动态纹理更新和实时渲染功能。

## 核心组件

### 1. VideoFrameTextureClass
继承自TextureClass，专门处理视频帧的纹理更新。

**主要功能：**
- 动态纹理创建和管理
- 视频帧数据更新
- 支持不同尺寸的视频帧

**关键方法：**
```cpp
// 初始化视频纹理
bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height);

// 更新视频帧
bool UpdateFrame(ID3D11DeviceContext* deviceContext, unsigned char* frameData, int width, int height);

// 获取纹理视图
ID3D11ShaderResourceView* GetTexture();
```

### 2. VideoRendererClass
管理视频帧的渲染流程。

**主要功能：**
- 顶点和索引缓冲区管理
- 视频帧渲染
- 支持不同宽高比的视频

**关键方法：**
```cpp
// 初始化渲染器
bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

// 渲染视频帧
bool Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

// 更新视频帧
bool UpdateVideoFrame(ID3D11DeviceContext* deviceContext, unsigned char* frameData, int width, int height);

// 初始化视频纹理
bool InitializeVideoTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height);
```

### 3. 集成到ApplicationClass
在现有的DirectX 11应用程序中集成了视频渲染功能。

## 使用方法

### 基本使用流程

1. **初始化视频渲染器：**
```cpp
// 在ApplicationClass::Initialize中
m_VideoRenderer = new VideoRendererClass;
m_VideoRenderer->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());

// 初始化视频纹理
m_VideoRenderer->InitializeVideoTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), 640, 480);
```

2. **更新视频帧：**
```cpp
// 在每一帧中更新视频数据
unsigned char* frameData = /* 从视频解码器获取的RGBA数据 */;
m_VideoRenderer->UpdateVideoFrame(m_Direct3D->GetDeviceContext(), frameData, width, height);
```

3. **渲染视频帧：**
```cpp
// 在渲染循环中
XMMATRIX worldMatrix = XMMatrixIdentity();
XMMATRIX viewMatrix = /* 摄像机视图矩阵 */;
XMMATRIX projectionMatrix = /* 投影矩阵 */;

m_VideoRenderer->Render(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
```

### 视频帧数据格式

视频帧数据应该是RGBA格式（每像素4字节）：
- R: 红色通道 (0-255)
- G: 绿色通道 (0-255)  
- B: 蓝色通道 (0-255)
- A: Alpha通道 (0-255)

### 支持的功能

1. **动态尺寸调整：** 自动处理不同尺寸的视频帧
2. **宽高比保持：** 自动调整显示区域以保持视频原始宽高比
3. **实时更新：** 支持每帧更新视频内容
4. **内存管理：** 自动管理纹理资源

## 技术实现细节

### DirectX 11 纹理创建

```cpp
// 创建动态纹理
D3D11_TEXTURE2D_DESC textureDesc;
textureDesc.Usage = D3D11_USAGE_DYNAMIC;
textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

device->CreateTexture2D(&textureDesc, NULL, &m_dynamicTexture);
```

### 动态顶点缓冲区创建

**重要修复：** 为了支持运行时更新顶点缓冲区（如调整视频尺寸），必须使用动态顶点缓冲区：

```cpp
// 创建动态顶点缓冲区
D3D11_BUFFER_DESC vertexBufferDesc;
vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;                    // 关键：动态使用
vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;       // 关键：允许CPU写入
vertexBufferDesc.MiscFlags = 0;
vertexBufferDesc.StructureByteStride = 0;

device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
```

### 纹理数据更新

```cpp
// 映射纹理进行写入
D3D11_MAPPED_SUBRESOURCE mappedResource;
deviceContext->Map(m_dynamicTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

// 复制帧数据
memcpy(mappedData, frameData, rowPitch * height);

// 解除映射
deviceContext->Unmap(m_dynamicTexture, 0);
```

### 顶点缓冲区更新

使用`D3D11_MAP_WRITE_DISCARD`标志来高效更新动态顶点缓冲区：

```cpp
// 映射顶点缓冲区进行写入
D3D11_MAPPED_SUBRESOURCE mappedResource;
deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

// 更新顶点数据
VertexType* verticesPtr = (VertexType*)mappedResource.pData;
// ... 更新顶点位置和纹理坐标

// 解除映射
deviceContext->Unmap(m_vertexBuffer, 0);
```

### 顶点缓冲区管理

视频渲染器使用三角形列表来渲染视频帧：
- 2个三角形组成一个矩形
- 纹理坐标映射到0-1范围
- 支持宽高比调整

## 扩展建议

1. **视频解码集成：** 可以集成FFmpeg或其他视频解码库
2. **多视频源：** 支持多个视频同时渲染
3. **视频特效：** 添加滤镜、转场等效果
4. **性能优化：** 使用多线程处理视频解码
5. **格式支持：** 支持YUV等其他视频格式

## 注意事项

1. 视频帧数据必须是RGBA格式
2. 确保在主线程中调用渲染方法
3. 注意内存管理，避免内存泄漏
4. 考虑视频帧率与渲染帧率的同步