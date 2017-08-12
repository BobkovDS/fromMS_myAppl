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
		c.x = x - point.x;
		c.y = y - point.y;
		c.z = z - point.z;
		return c;
	}

	float operator % (const VertexModelLoader &point) const { //scalar proizvedenie - Dot product
		float c;
		c = x*point.x + y*point.y + z*point.z;
		
		return c;
	}
	VertexModelLoader operator * (const VertexModelLoader &point) const { //Vectornoe proizvedenie - Cross product
		VertexModelLoader c;
		c.x = y*point.z - z*point.y;
		c.y =-1*(x*point.z - z*point.x);
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
	std::vector<std::uint16_t> FaceIndices;
	std::vector<std::uint16_t> NewPolyIndices;
	std::vector<VertexModelLoader> projection; // z-coordinate does not used here

	void TrangulationOfPolygon(std::vector<std::uint16_t> *PolygonIndex);
	bool Check3PointsOnLine(VertexModelLoader a, VertexModelLoader b, VertexModelLoader p);
	bool FindProjectPlane(VertexModelLoader p1, VertexModelLoader p2, VertexModelLoader p3, VertexModelLoader p4);
	bool CreateMinPolygon(std::vector<std::uint16_t> *inputPolygon);
	int CheckConvexPolynom(std::vector<std::uint16_t> *inputPolygon); //conv2P function
	void LCShift(std::vector<std::uint16_t> *inputPolygon); 
};

