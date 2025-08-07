#include "073DModelRenderWindow.h"
#include <fstream>
using namespace std;

ModelRenderWindow::~ModelRenderWindow()
{
	ReleaseModel();
}

void ModelRenderWindow::InitializeDiffuseLightVertexIndexData()
{
	char modelFilename[128];
	// Set the file name of the model.
	strcpy_s(modelFilename, "../Engine/data/cube.txt");
	LoadModel(modelFilename);

	// Create the vertex array.
	m_diffuseLightVertexData.resize(m_vertexCount);

	// Create the index array.
	m_indexData.resize(m_indexCount);

	// Load the vertex array and index array with data.
	for (int i = 0; i < m_vertexCount; i++)
	{
		m_diffuseLightVertexData[i].position = XMFLOAT4(m_model[i].x, m_model[i].y, m_model[i].z, 1.0f);
		m_diffuseLightVertexData[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		m_diffuseLightVertexData[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		m_indexData[i] = i;
	}
}

bool ModelRenderWindow::LoadModel(char* filename)
{
	ifstream fin;
	char input;
	int i;

	// Open the model file.
	fin.open(filename);

	// If it could not open the file then exit.
	if (fin.fail())
	{
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> m_vertexCount;

	// Set the number of indices to be the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the model using the vertex count that was read in.
	m_model = new ModelType[m_vertexCount];

	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// Close the model file.
	fin.close();

	return true;
}

void ModelRenderWindow::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}
}
