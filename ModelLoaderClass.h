#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <wrl.h>
#include <vector>
struct VertexModelLoader {
	float x, y, z;
	
	bool operator != (const VertexModelLoader &point) const {
		return (this->x != point.x || this->y != point.y || this->z != point.z);
	}

	bool operator == (const VertexModelLoader &point) const {
		return (this->x == point.x && this->y == point.y && this->z == point.z);
	}

	VertexModelLoader operator - (const VertexModelLoader &point) const {
		VertexModelLoader c;
		c.x = point.x - x;
		c.y = point.x - y;
		c.z = point.x - z;
		return c;
	}

	VertexModelLoader operator * (const VertexModelLoader &point) const { //Vectornoe proizvedenie
		VertexModelLoader c;
		c.x = y*point.z - z*point.y;
		c.y = x*point.z - z*point.x;
		c.z = x*point.y - y*point.x;
		return c;
	}

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

	void TrangulationOfPolygon(std::vector<std::uint16_t> *PolygonIndex);
	bool Check3PointsOnLine(VertexModelLoader a, VertexModelLoader b, VertexModelLoader p);
};

