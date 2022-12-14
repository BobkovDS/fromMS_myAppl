#pragma once
#include "ModelLoaderClass.h"
#include "ConvexHullBuilder.h"
#include "TriangulationDilane.h"
#include "ctime"

using namespace std;
using namespace DirectX;


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
				inVertex.z *= -1;
				VertexCount++;

				vertices.push_back(inVertex);
				//outPutLine = std::to_wstring(inVertex.x) + std::to_wstring(inVertex.y) + std::to_wstring(inVertex.z) + L"\n";
				//OutputDebugString(outPutLine.c_str());
			}
			else if (firstSymbol == "f")
			{

				uint32_t tmpIndex = 0;
				std::string tmpString;
				std::string delimiter = "//";
				FaceIndices.clear();
				std::vector<std::uint32_t> FaceIndicesfromFile;
				while (inStr.good())
				{
					inStr >> tmpString;
					tmpString = tmpString.substr(0, tmpString.find(delimiter));
					tmpIndex = std::stoi(tmpString)-1;
					FaceIndicesfromFile.push_back(tmpIndex);
				}
				
				for (int i = FaceIndicesfromFile.size() - 1; i >= 0; i--)
					FaceIndices.push_back(FaceIndicesfromFile.at(i));

				if (FaceIndices.size() > 3) // at least we have 1 polygon (2 and more triangle), so we need triangolize it
				{
					LocalFaceIndices.clear();
					NewPolyIndices.clear();

					vector<std::uint32_t> outputPolygon;
					if (FindProjectPlane(vertices.at(FaceIndices.at(0)), vertices.at(FaceIndices.at(1)), vertices.at(FaceIndices.at(2)), vertices.at(FaceIndices.at(3)))) // !#? should be modification to use Indices
						TrangulationOfPolygon(&LocalFaceIndices);

					for (int i = 0; i < NewPolyIndices.size(); i++) 
					{
						indices.push_back(FaceIndices.at( NewPolyIndices.at(i))); 
					}
				}
				else if (FaceIndices.size() == 3)
				{
					for (int i = 0; i < 3; i++)
					{
						indices.push_back(FaceIndices.at(i)); 
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

uint32_t ModelLoaderClass::GetNextI()
{
	return indices.at(m_currIndexIndex++);
}

void ModelLoaderClass::TrangulationOfPolygon(vector<std::uint32_t> *PolygonIndex)
{

	vector<std::uint32_t> tmpVector;

	CreateMinPolygon(PolygonIndex); //minimaze of polygon

	int n = PolygonIndex->size();
	if (n == 3)
	{// if this polygon is triangle, save it
		for (int i = 0; i < 3; i++)
			NewPolyIndices.push_back(PolygonIndex->at(i));
	}
	else 
	{
		int d = DirTest(PolygonIndex);

		// if polygon is convex polygon
		if (CheckConvexPolynom(PolygonIndex) == 1)
		{
			DividePolygon(2, PolygonIndex);
		}
		else
		{// if polygon is not convex polygon
			//Searching NonConvex Vertex = dir_test and n2f should have the same sign
			while (d*Nf2(PolygonIndex->at(n-1), PolygonIndex->at(0), PolygonIndex->at(1))<=0)
					LCShift(PolygonIndex);

			// try to find Pk (end of diagonal P1Pk)
			//test P1Pk edge
			bool kLoopExit = false;
			int k=0;
			for (k = 2; (k < n - 1) && (!kLoopExit); k++)
			{
				// a) K -vertex should be located on internal side of line P1P2
				if (d*Nf2(PolygonIndex->at(0), PolygonIndex->at(1), PolygonIndex->at(k)) <= 0)
				{
					// b- edge P1PK should not be cross with other edges (PiPi+1, where k<i<n)
					bool f = false;
					for (int i = k + 1; (i < n-1) && !f; i++) 
					{
						if (CrossSegm(0, k, i, i + 1, PolygonIndex) == 1) f = true;
					}
					kLoopExit = !f;
				}				
			}
			if (kLoopExit) k--;
			DividePolygon(k, PolygonIndex);
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
	projection.clear();
	VertexModelLoader a, b, c;
	a = p2 - p1;
	b = p3 - p2;
	c = p4 - p3;

	float stp = (a*b) % c;

	//if (stp <= 0.00000000001) // vectors are complanar
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
		if ((c.x >= c.y) && (c.x >= c.z)) axis = 1;
		else if ((c.y >= c.x) && (c.y >= c.z)) axis = 2;
		else if ((c.z >= c.y) && (c.z >= c.x)) axis = 3;

		
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

		//Projection we have in local Indices (0..k), so we should biuld new local Indices for that
		for (int i = 0; i < FaceIndices.size(); i++) LocalFaceIndices.push_back(i);
		//for (int i = FaceIndices.size() - 1; i >= 0; i--) LocalFaceIndices.push_back(i);

		return true;
	}
	
	return false;
}

bool ModelLoaderClass::CreateMinPolygon(vector<std::uint32_t> *inputPolygon)
{
	vector<std::uint32_t> outputPolygon;
	
	int m = 0;	
	//outputPolygon.push_back(inputPolygon->at(0));

	for (int i=1; i<inputPolygon->size(); i++)
	{
		if (projection.at(inputPolygon->at(m)) != projection.at(inputPolygon->at(i))) //if Pm and Pi point in projection are not equal
		{
			if (m == 0)
			{				
				m++;
				inputPolygon->at(m) = inputPolygon->at(i);

			}
			else
			{
				//check Pm-1, Pm and Pi are Collinear or not
				VertexModelLoader  a, b;
				a = projection.at(inputPolygon->at(m)) - projection.at(inputPolygon->at(m-1));
				b = projection.at(inputPolygon->at(i)) - projection.at(inputPolygon->at(m));
				float det = a.x*b.y - a.y*b.x;

				if (det == 0)
				{
					if (b%a > 0)
						inputPolygon->at(m) = inputPolygon->at(i);
					else
					{
						a = projection.at(inputPolygon->at(i)) - projection.at(inputPolygon->at(m - 1));
						b = projection.at(inputPolygon->at(m)) - projection.at(inputPolygon->at(m - 1));
						if (a%b < 0)
						{
							inputPolygon->at(m-1) = inputPolygon->at(m);
							inputPolygon->at(m) = inputPolygon->at(i);
						}
					}
				}
				else
				{			
					m++;
					inputPolygon->at(m) = inputPolygon->at(i);
				}						
			}
		}
	}


	if (Nf2(inputPolygon->at(0), inputPolygon->at(m-1), inputPolygon->at(m)) == 0)
	{
		inputPolygon->resize(m);
		m--;
	}

	if (Nf2(inputPolygon->at(m-1), inputPolygon->at(m), inputPolygon->at(0)) == 0)
	{
		for (int i = 0; i < m; i++)
		{
			inputPolygon->at(i)= inputPolygon->at(i+1);
		}
		inputPolygon->resize(m);
	}
	else
	{
		inputPolygon->resize(m + 1);
	}

	return false;
}

int ModelLoaderClass::CheckConvexPolynom(vector<std::uint32_t> *Polygon)
{
	int n = Polygon->size();
		
	float c = Nf2(Polygon->at(n - 1), Polygon->at(0), Polygon->at(1));

	for (int i = 1; i < n; i++)
	{
		int tmpI = i+1;
		if (tmpI == n)tmpI = 0;
		
		float z = Nf2(Polygon->at(i - 1), Polygon->at(i), Polygon->at(tmpI));

		if (c*z == 0) return -1; //have 180-angle
		else if (c*z < 0) return 0; // not convex
	}

	return 1; //Convex
}

void ModelLoaderClass::LCShift(std::vector<std::uint32_t> *inputPolygon)
{
	int n = inputPolygon->size();
	uint32_t tmpIndex = inputPolygon->at(0);

	for (int i =0; i <n-1; i++)
		inputPolygon->at(i) = inputPolygon->at(i +1);

	inputPolygon->at(n-1) = tmpIndex;
}

void ModelLoaderClass::RCShift(std::vector<std::uint32_t> *inputPolygon)
{
	int n = inputPolygon->size();
	uint32_t tmpIndex = inputPolygon->at(n - 1);

	for (int i = n - 1; i > 0; i--)
		inputPolygon->at(i) = inputPolygon->at(i - 1);

	inputPolygon->at(0) = tmpIndex;
}

void ModelLoaderClass::NonConvexVertex(uint32_t pNI, uint32_t pI1, uint32_t pI2)
{
	VertexModelLoader a, b;
	a = projection.at(pNI) - projection.at(pI1);
	b = projection.at(pNI) - projection.at(pI2);
	float z = a.x*b.y - a.y*b.x;
}

int ModelLoaderClass::DirTest(std::vector<std::uint32_t> *inputPolygon)
{
	int n = inputPolygon->size();
	float commonS = 0;
	for (int i = 1; i < n - 1; i++)
	{
		commonS += Nf2(inputPolygon->at(0), inputPolygon->at(i+1), inputPolygon->at(i));
	}

	if (commonS == 0) return commonS;
	else
		return commonS / abs(commonS); //only sign
		
}

float ModelLoaderClass::Nf2(uint32_t p1I, uint32_t p2I, uint32_t pnI)
{
	VertexModelLoader a, b;
	a = projection.at(pnI) - projection.at(p1I);
	b = projection.at(p2I) - projection.at(p1I);
	float z = a.x*b.y - a.y*b.x;

	return z;
}

int ModelLoaderClass::CrossSegm(uint32_t p1I, uint32_t p2I, uint32_t p3I, uint32_t p4I, std::vector<std::uint32_t> *inputPolygon)
{
	/*
		return:
		-1 - P1P2 and P3P4 are parallel or it is the same
		0 -  P1P2 and P3P4 are NOT cross 
		1 -  P1P2 and P3P4 are cross/or Concern 
	*/
	/*
	VertexModelLoader a, b, r;
	float det = 0;
	a = projection.at(inputPolygon->at(p2I)) - projection.at(inputPolygon->at(p1I));
	b = projection.at(inputPolygon->at(p3I)) - projection.at(inputPolygon->at(p4I));

	det = a.x*b.y - a.y*b.x;

	if (det == 0) return -1;

	r = projection.at(inputPolygon->at(p3I)) - projection.at(inputPolygon->at(p1I));
	r.x /= det;
	r.y /= det;*/

	float fa = Nf2(inputPolygon->at(p3I), inputPolygon->at(p4I), inputPolygon->at(p1I));
	float fb = Nf2(inputPolygon->at(p3I), inputPolygon->at(p4I), inputPolygon->at(p2I));
	float fc = Nf2(inputPolygon->at(p1I), inputPolygon->at(p2I), inputPolygon->at(p3I));
	float fd = Nf2(inputPolygon->at(p1I), inputPolygon->at(p2I), inputPolygon->at(p4I));

	bool f1 = (fc == fd);
	if (f1) return -1;

	float t = fa / (fa - fb);
	float k = fc / (fc - fd);

	bool f2 = ((fa*fb <= 0) && (fc*fd <= 0));

	return f2;
	//return ((r.x >= 0) && (r.x <= 1)) && ((r.y >= 0) && (r.y <= 1));
}

void ModelLoaderClass::DividePolygon(uint32_t devidePoint, std::vector<std::uint32_t> *inputPolygon)
{
	vector<std::uint32_t> Polygon1;
	vector<std::uint32_t> Polygon2;

	for (int i = 0; i <= devidePoint; i++)
		Polygon1.push_back(inputPolygon->at(i));

	Polygon2.push_back(inputPolygon->at(0));
	for (int i = devidePoint; i < inputPolygon->size(); i++)
		Polygon2.push_back(inputPolygon->at(i));

	
	TrangulationOfPolygon(&Polygon2);
	TrangulationOfPolygon(&Polygon1);
}

// ------------------------ DELONE --------------------------------

void ModelLoaderClass::GenerateDelone(int iN, int FlipLevel)
{	
	vertices.clear();
	VertexModelLoader tPoint;

	bool readFromFile = false;


	readFromFile = true;
	
	srand(static_cast<unsigned>(time(0)));

	int N = 1000;
	float Lo = -60;
	float Hi = 60;

	iN = N + 1;
	FlipLevel = -1;

	if (readFromFile)
	{
		//LoadModelFromFile(L"TriangulationTest10.obj");
		LoadModelFromFile(L"terrain.obj");
	}
	else
	{
		for (int i = 0; i < N; i++)
		{
			tPoint.x = Lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (Hi - Lo)));
			tPoint.y = Lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (Hi*2 - Lo)));
			tPoint.z = 0;
			//tPoint.x = floor(tPoint.x * 100 + 0.5) / 100;
			//tPoint.y = floor(tPoint.y * 100 + 0.5) / 100;
			vertices.push_back(tPoint);
		}
	}
	
	ConvexHullBuilder convexBuilder(&vertices);
	convexBuilder.Build_ConvexHull();

	TriangulationDilane DeloneCreator(Lo,Lo, int (Hi - Lo), int(Hi*2 - Lo), &vertices);
	DeloneCreator.CreateTriangulation(&convexBuilder._result_convex_hull);
	DeloneCreator.DelonePrepare();
	DeloneCreator.DeloneIt(&indices, iN, FlipLevel);
}

void ModelLoaderClass::LoadHighMap(std::string fileNanme)
{

}

void ModelLoaderClass::CalculateNormal(size_t clcNormalMode)
{
	DirectX::XMVECTOR tmpNormal;

	if (clcNormalMode == 0) //Mode 0 - Vertext Normal is average normale for all Faceplates for this vertext
	{		
		for (UINT32 i = 0; i < indices.size()-2; i++)
		{
			int i1 = i, i2 = i + 1, i3 = i + 2;
			
			VertexModelLoader v1 = vertices.at(indices.at(i1));
			VertexModelLoader v2 = vertices.at(indices.at(i2));
			VertexModelLoader v3 = vertices.at(indices.at(i3));
			
			VertexModelLoader a = v2 - v1;
			VertexModelLoader b = v3 - v1;

			VertexModelLoader n = a*b;
			
			// Add new normal to V1 normal
			tmpNormal = XMLoadFloat3(&v1.normal);
			tmpNormal += XMVectorSet(n.x, n.y, n.z, 0);			
			XMStoreFloat3(&vertices.at(indices.at(i1)).normal, tmpNormal);

			// Add new normal to V2 normal
			tmpNormal = XMLoadFloat3(&v2.normal);
			tmpNormal += XMVectorSet(n.x, n.y, n.z, 0);			
			XMStoreFloat3(&vertices.at(indices.at(i2)).normal, tmpNormal);

			// Add new normal to V3 normal
			tmpNormal = XMLoadFloat3(&v3.normal);
			tmpNormal += XMVectorSet(n.x, n.y, n.z, 0);
			XMStoreFloat3(&vertices.at(indices.at(i3)).normal, tmpNormal);
			i += 2;
		}

		for (UINT32 i = 0; i < vertices.size(); i++)
		{
			tmpNormal = XMLoadFloat3(&vertices.at(i).normal);
			tmpNormal = XMVector3Normalize(tmpNormal);
			XMStoreFloat3(&vertices.at(i).normal, tmpNormal);
		}
	}
}