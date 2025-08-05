#pragma once
#include "QRhiWindow.h"
#include <DirectXMath.h>
using namespace DirectX;

#ifdef ENABLE_QRHI
class MyRhiWindow : public QRhiWindow {
protected:
	QRhiSignal mSigInit;										//用于初始化的信号
	QRhiSignal mSigSubmit;										//用于提交资源的信号
	QScopedPointer<QRhiShaderResourceBindings> mShaderBindings;		//描述符集布局绑定
	QScopedPointer<QRhiGraphicsPipeline> mPipeline;					//图形渲染管线
protected:
	struct VertexType
	{
		XMFLOAT4 position;
		XMFLOAT4 color;
	};
	QScopedPointer<QRhiBuffer> m_vertexBuffer; 
	QScopedPointer<QRhiBuffer> m_indexBuffer;
	QScopedPointer<QRhiBuffer> m_matrixBuffer;
	int m_vertexCount, m_indexCount;
	QVector<VertexType> m_vertexData;
	QVector<quint32> m_indexData;
	QMatrix4x4 world;
	QMatrix4x4 view;
	QMatrix4x4 projection;
public:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;

		friend QDataStream& operator<<(QDataStream& out, const TargaHeader& header);
		friend QDataStream& operator>>(QDataStream& in, TargaHeader& header);
	};
public:
	MyRhiWindow(QRhiHelper::InitParams inInitParams);
	~MyRhiWindow();
protected:
	void InitializeData();
	void InitializeMatrix();
	bool InitializeBuffers();
protected:
	void InitializeTextureVertexIndexData();
	bool InitializeTexture();
	void RenderTexture();
	bool LoadTarga32Bit(const char* filename);
	unsigned char* m_targaData = nullptr;
	int m_width, m_height;
	struct TextureVertexType
	{
		XMFLOAT4 position;
		XMFLOAT2 texture;
	};
	QVector<TextureVertexType> m_textureVertexData;
	QImage mImage;
	QScopedPointer<QRhiTexture> mTexture;
	QScopedPointer<QRhiSampler> mSampler;
protected:
	void onRenderTick() override;
};
#endif
