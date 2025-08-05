#pragma once
#include "MyRhiWindow.h"

class DiffuseLightWindow : public MyRhiWindow {
public:
	using MyRhiWindow::MyRhiWindow;
private:
	virtual void InitializeDiffuseLightVertexIndexData();
	bool InitializeDiffuseLight();
	void RenderDiffuseLight();
protected:
	struct DiffuseLightVertexType
	{
		XMFLOAT4 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};
	struct DiffuseLightBufferType
	{
		XMFLOAT4 diffuseLightColor;
		XMFLOAT3 lightDirection;
	};
	QVector<DiffuseLightVertexType> m_diffuseLightVertexData;
	QScopedPointer<QRhiBuffer> m_diffuseLightBuffer;
protected:
	void onRenderTick() override;
};