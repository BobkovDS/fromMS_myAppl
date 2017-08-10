#include "ModelLoaderClass.h"
#include <array>

using namespace std;
ModelLoaderClass::ModelLoaderClass()
{
}

ModelLoaderClass::~ModelLoaderClass()
{
}

bool ModelLoaderClass::LoadModelFromFile(const std::wstring& filename)
{

	std::ifstream inputFile(filename);
	std::string inputLine;
	std::wstring outPutLine;
	int VertexCount = 0;
	if (inputFile)
	{
		while (std::getline(inputFile, inputLine))
		{
			std::istringstream inStr(inputLine);
			VertexModelLoader inVertex;
			
			std::string firstSymbol;

			inStr >> firstSymbol;
			if (firstSymbol == "v")
			{
				inStr >> inVertex.x >> inVertex.y >> inVertex.z;
				VertexCount++;

				vertices.push_back(inVertex);
				//outPutLine = std::to_wstring(inVertex.x) + std::to_wstring(inVertex.y) + std::to_wstring(inVertex.z) + L"\n";
				//OutputDebugString(outPutLine.c_str());
			}
			else if (firstSymbol == "f")
			{

				uint16_t tmpIndex = 0;
				std::string tmpString;
				std::string delimiter = "//";
				std::vector<std::uint16_t> tmpIndices;

				while (inStr.good())
				{
					inStr >> tmpString;
					tmpString = tmpString.substr(0, tmpString.find(delimiter));
					tmpIndex = std::stoi(tmpString)-1;
					tmpIndices.push_back(tmpIndex);
				}

				if (tmpIndices.size() >= 3) // at least we have 1 triangle
				{
					//TrangulationOfPolygon(&tmpIndices);
					int axis = FindProjectPlane(vertices.at(0), vertices.at(1), vertices.at(2), vertices.at(3));

					for (int i = 0; i < (tmpIndices.size() - 2); i++) // i= FaceCount = VerticesCount-2
					{
						indices.push_back(tmpIndices.at(0)); //Vertex #1;
						indices.push_back(tmpIndices.at(i+1)); //Vertex #2;
						indices.push_back(tmpIndices.at(i+2)); //Vertex #3;
					}
				}

			}
			
		}
		
		//OutputDebugString(std::to_wstring(vertices.size()).c_str());
		return true;
	}
	else
	{
		std::wstring ErrorMessage;
		ErrorMessage = L"File " + filename + L" not found\n";
		OutputDebugString(ErrorMessage.c_str());

		return false;
	}
	
}

// for Vertices vector
void ModelLoaderClass::SetToBeginV()
{
	m_currVertextIndex = 0;
}

int ModelLoaderClass::GetVectorSizeV()
{
	return vertices.size();
}

VertexModelLoader ModelLoaderClass::GetNextV()
{
	return vertices.at(m_currVertextIndex++);
}

// for Indicies Vector
void ModelLoaderClass::SetToBeginI()
{
	m_currIndexIndex = 0;
}

int ModelLoaderClass::GetVectorSizeI()
{
	return indices.size();
}

uint16_t ModelLoaderClass::GetNextI()
{
	return indices.at(m_currIndexIndex++);
}

void ModelLoaderClass::TrangulationOfPolygon(vector<std::uint16_t> *PolygonIndex)
{
	vector<std::uint16_t> tmpVector;

	int isize = PolygonIndex->size();
	int m = 0;

	tmpVector.push_back(0);
	for (int i = 1; i < isize; i++)
	{
		VertexModelLoader a, b;
		a = vertices.at(PolygonIndex->at(m));
		b = vertices.at(PolygonIndex->at(i));

		if (a != b)
		{
			if (m == 0)
			{
				m++;
				tmpVector.push_back(m);
			}
			else
			{
				Check3PointsOnLine(vertices.at(PolygonIndex->at(0)), vertices.at(m), vertices.at(i));
			}
		}
	}

}

bool ModelLoaderClass::Check3PointsOnLine(VertexModelLoader a, VertexModelLoader b, VertexModelLoader p)
{
	VertexModelLoader A,B,C;
	C.x = 0;
	C.y = 0;
	C.z = 0;

	A = p - a;
	B = b - a;
	return (A*B == C);
}

int ModelLoaderClass::FindProjectPlane(VertexModelLoader p1, VertexModelLoader p2, VertexModelLoader p3, VertexModelLoader p4)
{
	//Find Normal
	// -a: Check input vectors are coplanar. For that scalar triple product = 0
	// abc = (a x b) * c, where a x b - cross product, and b * c - dot product
	// a = p1 - p2
	// b = p2 - p3
	// c = p3 - p4
	
	VertexModelLoader a, b, c;
	a = p2 - p1;
	b = p3 - p2;
	c = p4 - p3;

	float stp = (a*b) % c;

	if (stp <= 0.00000000001) // vectors are complanar
	{
		a = p4 - p2;
		b = p3 - p1;
		c = a*b; // find Ortogonal vetor to this 2 vectors
		
		float modC = sqrt(c.x*c.x + c.y*c.y + c.z*c.z); 
		c.x = abs(c.x / modC); //normalize
		c.y = abs(c.y / modC);
		c.z = abs(c.z / modC);

		array<float, 3> forSort = { c.x,c.y,c.z };
		
		int axis = 3;
		for (int i = 0; i < 2; i++)
		{
			if (forSort[i] > forSort[i+1])
			{
				float tmpValue = forSort[i+1];
				forSort[i + 1] = forSort[i];
				forSort[i] = tmpValue;
				if (axis > (i + 1) ) axis = i + 1;
			}
		}

		return axis;
	}
	else return 0;
}