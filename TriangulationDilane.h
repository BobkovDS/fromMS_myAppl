#pragma once
#include "HelperStruct.h"

#include <Windows.h>




class TriangulationDilane {

public:
	TriangulationDilane(float leftValueOfRangeX, float leftValueOfRangeY, int widthX, int widthY, std::vector<VertexModelLoader> *inputVertices);
	~TriangulationDilane();
	void CreateTriangulation(std::vector<unsigned int> *inputIndecis);
	void DelonePrepare();
	void DeloneIt(std::vector<uint32_t> *outputIndices, int iN, int flipLevel);	

private:
	unsigned int _WidthX;
	unsigned int _WidthY;
	float LoX; // Left value of Range for X
	float LoY; // Left value of Range for Y
	int globalN = 100;// for debug
	bool setGlobal = false; //for debug
	int currentFlipLevel = 0;
	int reqFlipLevel = -1;
	std::vector<VertexModelLoader> *inPutDate;
	std::vector<uint16_t> TriangulationDilane::CreateDinamicCAHS(std::vector<uint16_t> oldCASH, int oldM);
		
	uint16_t Mod(uint16_t v);
	bool TriangulationDilane::_Nvertex(int _index, int v, int N);
	void TriangulationDilane::DebugFunction();
	double TriangulationDilane::SIN(VertexModelLoader p0, VertexModelLoader p1, VertexModelLoader p2);
	
	// Methods and structures to build lisf of triangles which are made from input list of points
	std::vector<Triangle> Triangles;
	bool Flip(int f, int s);
	std::array<int, 3> WalkToDarkOfMind(int nomer, VertexModelLoader P);
	std::array<int, 2>  GetNomers(int f, int s);
	void ExportToFile(int N);
	void ExportVerticesToFile();
	// Methods and structures to build lisf of triangles which are made from input list of points
	//std::vector<Triangle> Triangles;
	//bool Flip(int f, int s);
	//std::array<int, 3> WalkToDarkOfMind(int nomer, VertexModelLoader P);
	//std::array<int, 2>  GetNomers(int f, int s);
};
