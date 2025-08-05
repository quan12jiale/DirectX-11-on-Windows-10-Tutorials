#pragma once
#include "06DiffuseLightWindow.h"

class ModelRenderWindow : public DiffuseLightWindow {
private:
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};
public:
	ModelRenderWindow(QRhiHelper::InitParams inInitParams);
	~ModelRenderWindow();
private:
	void InitializeDiffuseLightVertexIndexData() override;
private:
	bool LoadModel(char* filename);
	void ReleaseModel();
private:
	ModelType* m_model;
};