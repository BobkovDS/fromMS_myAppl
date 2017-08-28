#include "ConvexHullBuilder.h"

using namespace std;


ConvexHullBuilder::ConvexHullBuilder()
{
}

ConvexHullBuilder::~ConvexHullBuilder()
{
}

void ConvexHullBuilder::Build_ConvexHull(vector<VertexModelLoader> *inPutDate)
{
	SortVector(inPutDate);

	_leftPoint = inPutDate->at(0);
	_rightPoint = inPutDate->at(inPutDate->size() - 1);

	vector<VertexModelLoader>::reverse_iterator revVectorIterator;
	VertexModelLoader betwPoint;
	VertexModelLoader a;
	VertexModelLoader b;

	for (int i = 1; i < inPutDate->size() - 1; i++)
	{
		//prbar.Value = i;
		betwPoint = inPutDate->at(i);
		a = _leftPoint - _rightPoint;
		b = betwPoint - _rightPoint;

		float det = a^b;
		if (det < 0) _upperPartition.push_back(betwPoint);
		else _lowerPartition.push_back(betwPoint);
	}

	//MessageBox.Show("This is partition");
	Build_HullPartition(&_upperPartition, -1);
	Build_HullPartition(&_lowerPartition, 1);

	revVectorIterator = _lowerHull.rbegin(); /// -- check how Iterator works, but here we don't need it

	for (int i = _lowerPartition.size() - 2; i > 0; i--)
		_upperPartition.push_back(_lowerPartition.at(i));

	_result_convex_hull = _upperPartition;
}

void ConvexHullBuilder::Build_HullPartition(vector<VertexModelLoader> *inPutDate, int factor)
{
	// factor: 1 for Upper, -1 for Lower;
	vector<VertexModelLoader> tmpVector;
	VertexModelLoader a;
	VertexModelLoader b;

	tmpVector.push_back(_leftPoint);
	inPutDate->push_back(_rightPoint);

	int pos = inPutDate->size();
	while (pos > 0)
	{
		tmpVector.push_back(inPutDate->at(inPutDate->size()- pos));

		
		while ( tmpVector.size() >= 3)
		{
			int tmpVectorSize = tmpVector.size();

			a = tmpVector.at(tmpVectorSize - 3) - tmpVector.at(tmpVectorSize - 1);
			b = tmpVector.at(tmpVectorSize - 2) - tmpVector.at(tmpVectorSize - 1);

			if ((factor * (a^b)) <= 0)
			{
				for (int i = tmpVectorSize - 2; i < tmpVectorSize-1; i++)
					tmpVector.at(i) = tmpVector.at(i + 1);
				tmpVector.resize(tmpVectorSize - 1);
			}
			else break;
		}
		pos--;

	}

	inPutDate->swap(tmpVector);
}
void ConvexHullBuilder::SortVector(std::vector<VertexModelLoader> *inPutDate)
{
	VertexModelLoader tPoint;
	for (int i = 0; i < inPutDate->size()-1; i++)
	{
		float MaxValue = inPutDate->at(i).x;		
		int IndexForMaxValue = i;
		for (int j = 1; j < inPutDate->size()-i; j++)
		{
			if (inPutDate->at(j).x < inPutDate->at(j-1).x)
			{
				tPoint = inPutDate->at(j);
				inPutDate->at(j) = inPutDate->at(j-1);
				inPutDate->at(j-1) = tPoint;
				j--;
			}

		}
	}
}