#include "TestRenderer.h"
#include "Windows.h"


TestRenderer::TestRenderer()
{
	m_bvh = NULL;
	isFindMove = false;
	noMoves = 68803;
}


void TestRenderer::DrawTriangles(const int groupId, const mwRenderedTriangle* triangles, const size_t numOfTriangles)
{
	GLsizei listId;

	std::map<int, GLsizei>::iterator end = displayLists.end();
	if (displayLists.find(groupId) != end)
	{
		// Reset display list 
		listId = displayLists.find(groupId)->second;
		glNewList(listId, GL_COMPILE);
		glEndList();
	}
	else
	{
		// Generate new display list
		listId = glGenLists(1);
		displayLists.insert( std::pair<int, GLsizei>(groupId, listId));

	}

	BVH* triangle_tree = NULL;
	std::vector<TriangleTemp> triListTemp(numOfTriangles);

	if (isFindMove)
	{
		//attach move id to red
		for( size_t i = 0; i < numOfTriangles; ++i )
		{
			TriangleTemp triTemp;
			for( size_t j = 0; j < 3; ++j )
			{
				VertexAttribute vertexAttr;
				vertexAttr.position = triangles[i].Vertices[j];
				vertexAttr.moveId = getMoveId(vertexAttr.position, vertexAttr.orientation);
				triTemp.vertices[j] = vertexAttr;
			}
			triListTemp[i] = triTemp;
		}

		//build bvh
		triangle_tree = build_triangle_bvh(triListTemp, numOfTriangles);

	}

	int noNotFound = 0;
	int noErrors = 0;
	
	// Update display list
	glNewList( listId, GL_COMPILE );
	glBegin(GL_TRIANGLES);
	for( size_t i = 0; i < numOfTriangles; ++i ){

		float adjacencies[3];
		//getMoveId(&triangles[i], adjacencies);//could have collision :(
		if (isFindMove) findAdjacencies(&triangles[i], triangle_tree, adjacencies, noNotFound, noErrors);

		for( size_t j = 0; j < 3; ++j )
		{
			glNormal3f(triangles[i].Normals[j][0], triangles[i].Normals[j][1], triangles[i].Normals[j][2]);
			//glColor4f(triangles[i].Colors[j].Red, triangles[i].Colors[j].Green, triangles[i].Colors[j].Blue, triangles[i].Colors[j].Alpha);

			if (isFindMove) 
			{
				glColor4f(triListTemp[i].vertices[j].moveId, adjacencies[j], 0, 1);
				glMultiTexCoord3f(GL_TEXTURE3, triListTemp[i].vertices[j].orientation.x(), 
					triListTemp[i].vertices[j].orientation.y(), triListTemp[i].vertices[j].orientation.z());

			}
			else 
			{
				Vector3d toolOrientation;
				glColor4f(getMoveId(triangles[i].Vertices[j], toolOrientation), 0, 0, 1);
				glMultiTexCoord3f(GL_TEXTURE3, toolOrientation.x(), toolOrientation.y(), toolOrientation.z());
			}

			glVertex3f(triangles[i].Vertices[j][0], triangles[i].Vertices[j][1], triangles[i].Vertices[j][2]);
		}
	}									
	glEnd();//GL_TRIANGLES
	glEndList();

	_cprintf( "number of opposites not found: %d, #errors: %d \n", noNotFound, noErrors );

};

void TestRenderer::DrawLines(const int groupId, const mwRenderedLine* lines, const size_t numOfLines)
{

	//return;

	GLsizei listId;

	std::map<int, GLsizei>::iterator end = edgeDisplayList.end();
	if (edgeDisplayList.find(groupId) != end)
	{
		// Reset display list
		listId = edgeDisplayList.find(groupId)->second;
		glNewList(listId, GL_COMPILE);
		glEndList();
	}
	else
	{
		// Generate new display list
		listId = glGenLists(1);
		edgeDisplayList.insert( std::pair<int, GLsizei>(groupId, listId));

	}

	// Update display list
	glNewList( listId, GL_COMPILE );
	glBegin(GL_LINES);
	for( size_t i = 0; i < numOfLines; ++i ){

		for( size_t j = 0; j < 2; ++j )
		{
			glColor4f(lines[i].Colors[j].Red, lines[i].Colors[j].Green, lines[i].Colors[j].Blue, lines[i].Colors[j].Alpha);
			glVertex3f(lines[i].Vertices[j][0], lines[i].Vertices[j][1], lines[i].Vertices[j][2]);
		}
	}									
	glEnd();//GL_LINES
	glEndList();
};

void TestRenderer::DeleteGroup(const int groupId)
{  
	int id = groupId; id++;
};

float TestRenderer::getMoveId(Vector3d vertex, Vector3d & orientation)
{
	if (m_bvh != NULL)
	{
		int totalCheck = 0; 
		int containedCheck = 0; 
		int leafCheck = 0;
		Shape::CurrentMove move = m_bvh->getNearestMoveId(vertex, totalCheck, containedCheck, leafCheck);
		//_cprintf("total check %d, contained check %d, leaf check %d \n", totalCheck, containedCheck, leafCheck );
		//Sleep(10000);
		orientation = move.orientation;
		//orientation -= Vector3d(m_minOrientation, m_minOrientation, m_minOrientation);
		//float range = m_maxOrientation - m_minOrientation;
		//orientation /= range;
		orientation.Normalize();
		orientation -= Vector3d(-1, -1, -1);
		orientation /= 2;
		return static_cast<float>(move.moveId)/static_cast<float>(noMoves);
	}
	else return -1;
}


//////////////////////////////////////////////////////////////////////////
//to do: find the nearest vertex in the tangent directions
//////////////////////////////////////////////////////////////////////////
void TestRenderer::getMoveId(const mwRenderedTriangle* triangle, float adjacencies[])
{
	Vector3d barycenter = (triangle->Vertices[0] + triangle->Vertices[1] + triangle->Vertices[2])/3.0f;
	Vector3d face_normal = (triangle->Normals[0] + triangle->Normals[1] + triangle->Normals[2])/3.0f;
	face_normal.Normalize();
	
	Vector3d edge01 = triangle->Vertices[1] - triangle->Vertices[0];
	Vector3d tan01 = edge01%face_normal;
	Vector3d adj01 = barycenter + tan01;

	Vector3d edge12 = triangle->Vertices[2] - triangle->Vertices[1];
	Vector3d tan12 = edge12%face_normal;
	Vector3d adj12 = barycenter + tan12;

	Vector3d edge20 = triangle->Vertices[0] - triangle->Vertices[2];
	Vector3d tan20 = edge20%face_normal;
	Vector3d adj20 = barycenter + tan20;

	Vector3d orient;
	adjacencies[0] = getMoveId( adj01, orient );
	adjacencies[1] = getMoveId( adj12, orient );
	adjacencies[2] = getMoveId( adj20, orient );
}


BVH * TestRenderer::build_triangle_bvh(const std::vector<TriangleTemp> & triangles, size_t length)
{
	std::vector<Shape *> triangleList;
	for (int i = 0; i < length; i++)
	{
		Shape::VertexMove v0;
		v0.vertex = triangles[i].vertices[0].position;
		v0.found = false;
		v0.moveId = triangles[i].vertices[0].moveId;

		Shape::VertexMove v1;
		v1.vertex = triangles[i].vertices[1].position;
		v1.found = false;
		v1.moveId = triangles[i].vertices[1].moveId;

		Shape::VertexMove v2;
		v2.vertex = triangles[i].vertices[2].position;
		v2.found = false;
		v2.moveId = triangles[i].vertices[2].moveId;

		Shape* triangle = new Triangle(v0, v1, v2);

		triangleList.push_back(triangle);
	}
	return new BVH(triangleList);
}

//find all triangles having the bbx containing the two vertices with the current one
//make use of the bvh
void TestRenderer::findAdjacencies(const mwRenderedTriangle* triangle, BVH * bvh, float adjacencies[], 
								   int & noNotFounds, int & noErrors)
{
	for (int i = 0; i < 3; i++)
	{
		Shape::VertexMove adj = bvh->getAdjacentMove(triangle->Vertices[0], triangle->Vertices[1],
			triangle->Vertices[2], i);
		adjacencies[i] = adj.moveId;

		if (!adj.found)
		{
			if (adj.vertex == triangle->Vertices[i])
			{
				noNotFounds++;
			} 
			else
			{
				noErrors++;
			}
		}

	}
}