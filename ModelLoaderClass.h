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
	uint16_t GetNextI();

private:
	int m_currVertextIndex = 0;
	int m_currIndexIndex = 0;
	int IndexCount;

	std::vector<VertexModelLoader> vertices;
	std::vector<std::uint16_t> indices;
	std::vector<std::uint16_t> FaceIndices;
	std::vector<std::uint16_t> LocalFaceIndices;
	std::vector<std::uint16_t> NewPolyIndices;
	std::vector<VertexModelLoader> projection; // z-coordinate does not used here

	
	void TrangulationOfPolygon(std::vector<std::uint16_t> *PolygonIndex);
	bool Check3PointsOnLine(VertexModelLoader a, VertexModelLoader b, VertexModelLoader p);
	bool FindProjectPlane(VertexModelLoader p1, VertexModelLoader p2, VertexModelLoader p3, VertexModelLoader p4);
	bool CreateMinPolygon(std::vector<std::uint16_t> *inputPolygon);
	void DividePolygon(uint16_t devidePoint, std::vector<std::uint16_t> *inputPolygon);
	int CheckConvexPolynom(std::vector<std::uint16_t> *inputPolygon); //conv2P function
	void LCShift(std::vector<std::uint16_t> *inputPolygon); 
	void RCShift(std::vector<std::uint16_t> *inputPolygon);
	void NonConvexVertex(uint16_t pNI, uint16_t pI1, uint16_t pI2);
	int DirTest(std::vector<std::uint16_t> *inputPolygon);
	float Nf2(uint16_t pNI, uint16_t pI1, uint16_t pI2); // Orientation test for point and line (or 3 points on one line/ or S of triangle)
	int CrossSegm(uint16_t p1I, uint16_t p2I, uint16_t p3I, uint16_t p4I, std::vector<std::uint16_t> *inputPolygon); // Segments cross test
};

