#pragma once
#include "HelperStruct.h"

class ConvexHullBuilder
{
	//or Vertex
public:
	ConvexHullBuilder(std::vector<VertexModelLoader> *inPutDate);
	~ConvexHullBuilder();

	void Build_ConvexHull();
	std::vector<unsigned int> _result_convex_hull;

private:
	std::vector<VertexModelLoader> *inputVertices;

	VertexModelLoader _leftPoint;
	VertexModelLoader _rightPoint;
	//
	std::vector<unsigned int> _lowerPartition;
	std::vector<unsigned int> _upperPartition;
	std::vector<unsigned int> _lowerHull;
	
	bool Error = true;
		
	void SortVector(std::vector<VertexModelLoader> *inPutDate);
	void Build_HullPartition(std::vector<unsigned int> *inPutDate, int factor);
		
};