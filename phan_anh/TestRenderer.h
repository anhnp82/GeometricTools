
#include <map>
#include <gl/glew.h>
#include "BVH.h"
#include <conio.h>
#include "Triangle.h"

class TestRenderer 
{
public:

	struct VertexAttribute 
	{
		Vector3d position;
		float moveId;
		Vector3d orientation;
	};

	struct TriangleTemp 
	{
		VertexAttribute vertices[3];
	};

	std::map<int, GLsizei> displayLists;
	std::map<int, GLsizei> edgeDisplayList;

	void DrawTriangles(const int groupId, const mwRenderedTriangle* triangles, const size_t numOfTriangles);
	void DrawLines(const int groupId, const mwRenderedLine* lines, const size_t numOfLines);
	void DeleteGroup(const int groupId);

	TestRenderer();

	void setBvh(BVH * bvh)
	{
		m_bvh = bvh;
	}

	void setNoMoves(int size)
	{
		noMoves = size;
		_cprintf("size %d \n", size );
	}

	void setIsFindMove(bool findMove)
	{
		isFindMove = findMove;
	}

	void setMaxOrientation(float maxOrientation)
	{
		m_maxOrientation = maxOrientation;
		_cprintf("max orientation %f \n", m_maxOrientation );
	}

	void setMinOrientation(float minOrientation)
	{
		m_minOrientation = minOrientation;
		_cprintf("min orientation %f \n", m_minOrientation );
	}

	BVH * build_triangle_bvh(const std::vector<TriangleTemp> & triangles, size_t length);

private:
	
	BVH * m_bvh;
	float getMoveId(Vector3d vertex, Vector3d & orientation);
	void getMoveId(const mwRenderedTriangle* triangle, float adjacencies[]);
	void findAdjacencies(const mwRenderedTriangle* triangle, BVH * bvh, float adjacencies[], 
		int & noNotFounds, int & noErrors);

	
	float m_minOrientation;
	float m_maxOrientation;
	int noMoves;
	bool isFindMove;
};