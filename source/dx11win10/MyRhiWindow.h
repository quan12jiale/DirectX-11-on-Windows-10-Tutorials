#pragma once
#include "QRhiWindow.h"
#include <DirectXMath.h>
using namespace DirectX;

#ifdef ENABLE_QRHI
class MyRhiWindow : public QRhiWindow {
private:
	QRhiSignal mSigInit;										//用于初始化的信号
	QRhiSignal mSigSubmit;										//用于提交资源的信号
	QScopedPointer<QRhiShaderResourceBindings> mShaderBindings;		//描述符集布局绑定
	QScopedPointer<QRhiGraphicsPipeline> mPipeline;					//图形渲染管线
private:
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
	MyRhiWindow(QRhiHelper::InitParams inInitParams);
private:
	void InitializeData();
	void InitializeMatrix();
	bool InitializeBuffers();
protected:
	void onRenderTick() override;
};
#endif
