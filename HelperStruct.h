#pragma once
#include <vector>
#include <array>

struct VertexModelLoader {
	float x, y, z;

	bool operator != (const VertexModelLoader &point) const {
		return (this->x != point.x || this->y != point.y || this->z != point.z);
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

	VertexModelLoader GetVertex(int n) { return allVertices->at(Vertices[n]); }
};

