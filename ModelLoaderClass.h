#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <wrl.h>
#include <vector>
struct VertexModelLoader {
	float x, y, z;
};

class ModelLoaderClass
{
public:
	ModelLoaderClass();
	~ModelLoaderClass();

	bool LoadModelFromFile(const std::wstring& filename);
	int GetVectorSizeV();
	int GetVectorSizeI();
	void SetToBeginV();
	void SetToBeginI();
	VertexModelLoader GetNextV();
	uint16_t GetNextI();

private:
	int m_currVertextIndex = 0;
	int m_currIndexIndex = 0;
	int IndexCount;

	std::vector<VertexModelLoader> vertices;
	std::vector<std::uint16_t> indices;
};

