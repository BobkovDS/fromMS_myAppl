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
				FaceIndices.clear();

				while (inStr.good())
				{
					inStr >> tmpString;
					tmpString = tmpString.substr(0, tmpString.find(delimiter));
					tmpIndex = std::stoi(tmpString)-1;
					FaceIndices.push_back(tmpIndex);
				}

				if (FaceIndices.size() >= 3) // at least we have 1 triangle
				{
					//TrangulationOfPolygon(&tmpIndices);
					vector<std::uint16_t> outputPolygon;
					if (FindProjectPlane(vertices.at(0), vertices.at(1), vertices.at(2), vertices.at(3))) // !#? should be modification to use Indices
						CreateMinPolygon(&FaceIndices);

					for (int i = 0; i < (FaceIndices.size() - 2); i++) // i= FaceCount = VerticesCount-2
					{
						indices.push_back(FaceIndices.at(0)); //Vertex #1;
						indices.push_back(FaceIndices.at(i+1)); //Vertex #2;
						indices.push_back(FaceIndices.at(i+2)); //Vertex #3;
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

	CreateMinPolygon(PolygonIndex); //minimaze of polygon

	if (PolygonIndex->size() == 3)
	{// if this polygon is triangle, save it
		for (int i = 0; i < 3; i++)
			NewPolyIndices.push_back(PolygonIndex->at(i));
	}
	else 
	{
		// if polygon is convex polygon
		if (CheckConvexPolynom(PolygonIndex) == 1)
		{
			vector<std::uint16_t> Polygon1;
			vector<std::uint16_t> Polygon2;
			
			for (int i = 0; i < 3; i++)
				Polygon1.push_back(PolygonIndex->at(i));
			
			Polygon2.push_back(PolygonIndex->at(0));
			for (int i = 2; i < PolygonIndex->size(); i++)
				Polygon2.push_back(PolygonIndex->at(i));

			TrangulationOfPolygon(&Polygon1);
			TrangulationOfPolygon(&Polygon2);
		}
		else
		{// if polygon is not convex polygon
			LCShift(PolygonIndex);
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

bool ModelLoaderClass::FindProjectPlane(VertexModelLoader p1, VertexModelLoader p2, VertexModelLoader p3, VertexModelLoader p4)
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

		if (axis == 3) //do projection on XY plane
		{
			for (int i = 0; i < FaceIndices.size(); i++)
			{
				VertexModelLoader tmpPoint;
				tmpPoint.x = vertices.at(FaceIndices.at(i)).x;
				tmpPoint.y = vertices.at(FaceIndices.at(i)).y;
				projection.push_back(tmpPoint);
			}
		}
		else
			if (axis == 2) //do projection on XZ plane
			{
				for (int i = 0; i < FaceIndices.size(); i++)
				{
					VertexModelLoader tmpPoint;
					tmpPoint.x = vertices.at(FaceIndices.at(i)).x;
					tmpPoint.y = vertices.at(FaceIndices.at(i)).z;
					projection.push_back(tmpPoint);
				}
			}
			else
				if (axis == 1) //do projection on ZY plane
				{
					for (int i = 0; i < FaceIndices.size(); i++)
					{
						VertexModelLoader tmpPoint;
						tmpPoint.x = vertices.at(FaceIndices.at(i)).z;
						tmpPoint.y = vertices.at(FaceIndices.at(i)).y;
						projection.push_back(tmpPoint);
					}
				}
		return true;
	}
	
	return false;
}

bool ModelLoaderClass::CreateMinPolygon(vector<std::uint16_t> *inputPolygon)
{
	vector<std::uint16_t> outputPolygon;
	
	int m = 0;	
	outputPolygon.push_back(inputPolygon->at(0));

	for (int i=1; i< projection.size(); i++)
	{
		if (vertices.at(inputPolygon->at(m)) != vertices.at(inputPolygon->at(i))) //if Pm and Pi point in projection are not equal
		{
			if (m == 0)
			{				
				m++;
				outputPolygon.push_back(inputPolygon->at(i));

			}
			else
			{
				//check Pm-1, Pm and Pi are Collinear or not
				VertexModelLoader  a, b;
				a = projection.at(m) - projection.at(m-1);
				b = projection.at(i) - projection.at(m);
				float det = a.x*b.y - a.y*b.x;

				if (det == 0)
				{
					if (b%a > 0)
						outputPolygon.at(m) = inputPolygon->at(i);
					else
					{
						a = projection.at(i) - projection.at(m - 1);
						b = projection.at(m) - projection.at(m - 1);
						if (a%b < 0)
						{
							outputPolygon.at(m - 1) = outputPolygon.at(m);
							outputPolygon.at(m) = inputPolygon->at(i);
						}
					}
				}
				else
				{			
					m++;
					outputPolygon.push_back(inputPolygon->at(i));
				}						
			}
		}
	}

	inputPolygon->clear();
	for (int i = 0; i < outputPolygon.size(); i++)
		inputPolygon->push_back(outputPolygon.at(i));

	return false;
}

int ModelLoaderClass::CheckConvexPolynom(vector<std::uint16_t> *Polygon)
{
	int n = Polygon->size();
	
	VertexModelLoader a, b;

	a = projection.at(Polygon->at(n - 1)) - projection.at(Polygon->at(1));
	b = projection.at(Polygon->at(n - 1)) - projection.at(Polygon->at(2));

	float c = a.x*b.y-a.y*b.x;

	for (int i = 1; i < n; i++)
	{
		int tmpI = i+1;
		if (tmpI == n)tmpI = 0;
		a = projection.at(Polygon->at(i-i)) - projection.at(Polygon->at(i));
		b = projection.at(Polygon->at(i-1)) - projection.at(Polygon->at(tmpI));

		float z = a.x*b.y - a.y*b.x;

		if (c*z == 0) return -1; //have 180-angle
		else if (c*z < 0) return 0; // not convex
	}

	return 1; //Convex
}

void ModelLoaderClass::LCShift(std::vector<std::uint16_t> *inputPolygon)
{
	int n = inputPolygon->size();
	uint16_t tmpIndex = inputPolygon->at(n - 1);

	for (int i = n - 1; i > 0; i--)
		inputPolygon->at(i) = inputPolygon->at(i - 1);

	inputPolygon->at(0) = tmpIndex;
}