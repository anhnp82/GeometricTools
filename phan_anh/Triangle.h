#pragma once
#include "shape.h"

class Triangle :
	public Shape
{
public:
	Triangle(VertexMove _v0, VertexMove _v1, VertexMove _v2);
	~Triangle(void);

	const BBox & getBbox();
	void draw();
	void writeToTexture(GLfloat* bvh_tex, const int & escIdxFromParentToRightChild);
	void textureIndexing(int & index);
	void freeNode();
	CurrentMove getNearestMoveId(const Vector3 & vertex, int & totalCheck, 
		int & contained_check, int & leaf_check);

	VertexMove getAdjacentMove(const Vector3 & v0, const Vector3 & v1, 
		const Vector3 & v2, const int & excludedVertex);

private:

	BBox bbox;
	VertexMove m_v0;
	VertexMove m_v1;
	VertexMove m_v2;
};
