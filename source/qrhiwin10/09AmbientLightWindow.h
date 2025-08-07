#pragma once
#include "073DModelRenderWindow.h"

class AmbientLightWindow : public ModelRenderWindow {
protected:
	struct AmbientLightBufferType
	{
		XMFLOAT4 diffuseLightColor;
		XMFLOAT4 ambientLightColor;
		XMFLOAT3 lightDirection;
	};
	QScopedPointer<QRhiBuffer> m_ambientLightBuffer;
public:
	using ModelRenderWindow::ModelRenderWindow;
private:
	bool InitializeAmbientLight();
	void RenderAmbientLight();
protected:
	void onRenderTick() override;
};