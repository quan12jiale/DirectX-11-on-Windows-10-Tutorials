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
	using DiffuseLightWindow::DiffuseLightWindow;
	~ModelRenderWindow();
protected:
	void InitializeDiffuseLightVertexIndexData() override;
protected:
	bool LoadModel(char* filename);
	void ReleaseModel();
protected:
	ModelType* m_model;
};