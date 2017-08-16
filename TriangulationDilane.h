#pragma once
#include "HelperStruct.h"

class TriangulationDilane {

public:
	TriangulationDilane();
	~TriangulationDilane();
	void CreateTriangulation(std::vector<VertexModelLoader> *inPutDate);
	std::array<uint16_t, 2> WalkToDarkOfMind(int nomer, VertexModelLoader P);

private:
	vector<Triangle> Triangles;
	int _Width = 600;

	bool Flip(int f, int s);
	void DeloneIt(std::vector<VertexModelLoader> *inPutDate);
	void DelonePrepare();
	array<uint16_t, 2>  GetNomers(int f, int s);
	uint16_t Mod(uint16_t v)
};
