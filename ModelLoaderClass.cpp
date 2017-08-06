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