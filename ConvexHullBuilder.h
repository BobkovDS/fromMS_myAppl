#pragma once
#include "HelperStruct.h"

class ConvexHullBuilder
{
	//or Vertex
public:
	ConvexHullBuilder();
	~ConvexHullBuilder();

	void Build_ConvexHull(std::vector<VertexModelLoader> *inPutDate);
	std::vector<VertexModelLoader> _result_convex_hull;

private:
	VertexModelLoader _leftPoint;
	VertexModelLoader _rightPoint;
	//
	std::vector<VertexModelLoader> _lowerPartition;
	std::vector<VertexModelLoader> _upperPartition;
	std::vector<VertexModelLoader> _lowerHull;
	std::vector<VertexModelLoader> _upperHull;
	

	bool Error = true;

	
	void SortVector(std::vector<VertexModelLoader> *inPutDate);
	void Build_HullPartition(std::vector<VertexModelLoader> *inPutDate, int factor);
		
};