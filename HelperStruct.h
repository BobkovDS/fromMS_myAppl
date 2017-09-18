#pragma once
#include <vector>
#include <array>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#define accuracy  0.000001f

struct VertexModelLoader {
	float x, y, z;	
	DirectX::XMFLOAT3 normal = DirectX::XMFLOAT3(0,0,0);

	bool operator != (const VertexModelLoader &point) const {
		//return (this->x != point.x || this->y != point.y || this->z != point.z);
		float dx = abs(this->x - point.x);
		float dy = abs(this->y - point.y);
		float dz = abs(this->z - point.z);

		return (dx>accuracy || dy>accuracy || dz>accuracy);
	}

	bool operator == (const VertexModelLoader &point) const {
		return (this->x == point.x && this->y == point.y && this->z == point.z);
	}

	VertexModelLoader operator - (const VertexModelLoader &point) const {
		VertexModelLoader c;
		c.x = x - point.x;
		c.y = y - point.y;
		c.z = z - point.z;
		return c;
	}

	float operator % (const VertexModelLoader &point) const { //scalar proizvedenie - Dot product
		float c;
		c = x*point.x + y*point.y + z*point.z;

		return c;
	}

	float operator ^ (const VertexModelLoader &point) const { //determinant
		float c;
		c = x*point.y - y*point.x;

		return c;
	}

	VertexModelLoader operator * (const VertexModelLoader &point) const { //Vectornoe proizvedenie - Cross product
		VertexModelLoader c;
		c.x = y*point.z - z*point.y;
		c.y = -1 * (x*point.z - z*point.x);
		c.z = x*point.y - y*point.x;
		return c;
	}
};

struct Triangle
{
	std::array<unsigned int, 3> Vertices;
	std::array<int, 3> NeighborIDs;
	uint16_t ID;
	static std::vector<VertexModelLoader>* allVertices;

	Triangle() { NeighborIDs = { -1,-1,-1 }; };
	VertexModelLoader GetVertex(int n) { return allVertices->at(Vertices[n]); }
};

