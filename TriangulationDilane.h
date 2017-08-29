#pragma once
#include "HelperStruct.h"

#include <Windows.h>







class TriangulationDilane {

public:
	TriangulationDilane();
	~TriangulationDilane();
	void CreateTriangulation(std::vector<VertexModelLoader> *inPutDate);
	void DelonePrepare();
	void DeloneIt(std::vector<VertexModelLoader> *inPutDate);

private:
	std::vector<Triangle> Triangles;
	int _Width = 9;

	bool Flip(int f, int s);
	
	std::array<int, 3> WalkToDarkOfMind(int nomer, VertexModelLoader P);
	std::array<int, 2>  GetNomers(int f, int s);
	uint16_t Mod(uint16_t v);
	bool TriangulationDilane::_Nvertex(int _index, int v, int N);
	void TriangulationDilane::DebugFunction();
	double TriangulationDilane::SIN(VertexModelLoader p0, VertexModelLoader p1, VertexModelLoader p2);
	std::vector<uint16_t> TriangulationDilane::CreateDinamicCAHS(std::vector<uint16_t> oldCASH, int oldM);
};
