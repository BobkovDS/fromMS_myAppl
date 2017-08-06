#include "ModelLoaderClass.h"


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