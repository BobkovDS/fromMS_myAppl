#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <wrl.h>
#include "HelperStruct.h"


class ModelLoaderClass
{
public:
	ModelLoaderClass();
	~ModelLoaderClass();

	void GenerateDelone();
	bool LoadModelFromFile(const std::wstring& filename);
	int GetVectorSizeV();
	int GetVectorSizeI();
	void SetToBeginV();
	void SetToBeginI();
	VertexModelLoader GetNextV();
	uint32_t GetNextI();

private:
	int m_currVertextIndex = 0;
	int m_currIndexIndex = 0;
	int IndexCount;

	std::vector<VertexModelLoader> vertices;
	std::vector<std::uint32_t> indices;
	std::vector<std::uint32_t> FaceIndices;
	std::vector<std::uint32_t> LocalFaceIndices;
	std::vector<std::uint32_t> NewPolyIndices;
	std::vector<VertexModelLoader> projection; // z-coordinate does not used here

	
	void TrangulationOfPolygon(std::vector<std::uint32_t> *PolygonIndex);
	bool Check3PointsOnLine(VertexModelLoader a, VertexModelLoader b, VertexModelLoader p);
	bool FindProjectPlane(VertexModelLoader p1, VertexModelLoader p2, VertexModelLoader p3, VertexModelLoader p4);
	bool CreateMinPolygon(std::vector<std::uint32_t> *inputPolygon);
	void DividePolygon(uint32_t devidePoint, std::vector<std::uint32_t> *inputPolygon);
	int CheckConvexPolynom(std::vector<std::uint32_t> *inputPolygon); //conv2P function
	void LCShift(std::vector<std::uint32_t> *inputPolygon); 
	void RCShift(std::vector<std::uint32_t> *inputPolygon);
	void NonConvexVertex(uint32_t pNI, uint32_t pI1, uint32_t pI2);
	int DirTest(std::vector<std::uint32_t> *inputPolygon);
	float Nf2(uint32_t pNI, uint32_t pI1, uint32_t pI2); // Orientation test for point and line (or 3 points on one line/ or S of triangle)
	int CrossSegm(uint32_t p1I, uint32_t p2I, uint32_t p3I, uint32_t p4I, std::vector<std::uint32_t> *inputPolygon); // Segments cross test
};

