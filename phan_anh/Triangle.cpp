#include "Triangle.h"
#include <conio.h>

Triangle::Triangle(VertexMove _v0, VertexMove _v1, VertexMove _v2)
{
	m_v0 = _v0;
	m_v1 = _v1;
	m_v2 = _v2;
	bbox = BBox( m_v0.vertex, m_v1.vertex );
	bbox.EnlargeForPoint(m_v2.vertex);
}

Triangle::~Triangle(void)
{
}


const Shape::BBox & Triangle::getBbox()
{
	return bbox;
}

void Triangle::draw()
{
	glBegin(GL_TRIANGLES);

	glColor3f((float)rand()/(float)RAND_MAX, (float)rand()/(float)RAND_MAX, (float)rand()/(float)RAND_MAX);
	
	glVertex3f(m_v0.vertex.x(), m_v0.vertex.y(), m_v0.vertex.z());
	glVertex3f(m_v1.vertex.x(), m_v1.vertex.y(), m_v1.vertex.z());
	glVertex3f(m_v2.vertex.x(), m_v2.vertex.y(), m_v2.vertex.z());
	glEnd();
}

void Triangle::writeToTexture(GLfloat* bvh_tex, const int & escIdxFromParentToRightChild)
{
	//return;
	int dummy = escIdxFromParentToRightChild;
	GLfloat aBvh = bvh_tex;
}

void Triangle::textureIndexing(int & index){}
void Triangle::freeNode(){}
Shape::CurrentMove Triangle::getNearestMoveId(const Vector3 & vertex, int & totalCheck, 
							 int & contained_check, int & leaf_check)
{
	CurrentMove dummy;
	return dummy;
}



Shape::VertexMove Triangle::getAdjacentMove(const Vector3 & v0, const Vector3 & v1, 
											const Vector3 & v2, const int & excludedVertex)
{
	switch (excludedVertex)
	{
	case 0:

		if (bbox.Contains(v1) && bbox.Contains(v2))
		{
			
			if (v0 == m_v0.vertex || v0 == m_v1.vertex || v0 == m_v2.vertex)//the same triangle
			{
				if (v0 == m_v0.vertex)
				{
					m_v0.found = false;
					return m_v0;
				}
				if (v0 == m_v1.vertex)
				{
					m_v1.found = false;
					return m_v1;
				}
				if (v0 == m_v2.vertex)
				{
					m_v2.found = false;
					return m_v2;
				}
			}else//the opposite triangle, found
			{

				//_cprintf( "opposite!!! \n" );

				if (m_v0.vertex != v1 && m_v0.vertex != v2)
				{
					//_cprintf( "opposite!!! \n" );
					m_v0.found = true;
					return m_v0;
				}
				if (m_v1.vertex != v1 && m_v1.vertex != v2)
				{
					
					m_v1.found = true;
					return m_v1;
				}
				if (m_v2.vertex != v1 && m_v2.vertex != v2)
				{
					m_v2.found = true;
					return m_v2;
				}
			}

		} 
		else
		{
			VertexMove dummy;
			dummy.moveId = -1;
			return dummy;
		}

		break;

	case 1:

		if (bbox.Contains(v0) && bbox.Contains(v2))
		{

			if (v1 == m_v0.vertex || v1 == m_v1.vertex || v1 == m_v2.vertex)//the same triangle
			{
				if (v1 == m_v0.vertex)
				{
					m_v0.found = false;
					return m_v0;
				}
				if (v1 == m_v1.vertex)
				{
					m_v1.found = false;
					return m_v1;
				}
				if (v1 == m_v2.vertex)
				{
					m_v2.found = false;
					return m_v2;
				}
			}else//the opposite triangle, found
			{
				
				//_cprintf( "opposite!!! \n" );
				
				if (m_v0.vertex != v0 && m_v0.vertex != v2)
				{
					m_v0.found = true;
					return m_v0;
				}
				if (m_v1.vertex != v0 && m_v1.vertex != v2)
				{
					m_v1.found = true;
					return m_v1;
				}
				if (m_v2.vertex != v0 && m_v2.vertex != v2)
				{
					m_v2.found = true;
					return m_v2;
				}
			}

		} 
		else
		{
			VertexMove dummy;
			dummy.moveId = -1;
			return dummy;
		}

		break;


	case 2:

		if (bbox.Contains(v1) && bbox.Contains(v0))
		{

			if (v2 == m_v0.vertex || v2 == m_v1.vertex || v2 == m_v2.vertex)//the same triangle
			{
				if (v2 == m_v0.vertex)
				{
					m_v0.found = false;
					return m_v0;
				}
				if (v2 == m_v1.vertex)
				{
					m_v1.found = false;
					return m_v1;
				}
				if (v2 == m_v2.vertex)
				{
					m_v2.found = false;
					return m_v2;
				}
			}else//the opposite triangle, found
			{
				
				//_cprintf( "opposite!!! \n" );
				
				if (m_v0.vertex != v1 && m_v0.vertex != v0)
				{
					m_v0.found = true;
					return m_v0;
				}
				if (m_v1.vertex != v1 && m_v1.vertex != v0)
				{
					m_v1.found = true;
					return m_v1;
				}
				if (m_v2.vertex != v1 && m_v2.vertex != v0)
				{
					m_v2.found = true;
					return m_v2;
				}
			}

		} 
		else
		{
			VertexMove dummy;
			dummy.moveId = -1;
			return dummy;
		}

		break;

	}
}