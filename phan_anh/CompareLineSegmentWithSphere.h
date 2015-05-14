#pragma once


#include "ShapeSphere.h"
#include "CompareLineSegmentWithBox.h"

class CompareLineSegmentWithSphere
{

	typedef TPoint3d<float> Vector3;

private:

	CompareLineSegmentWithBox::Segment m_currentSegment;
	Vector3 m_direction;
	float m_length;

public:

	CompareLineSegmentWithSphere(const CompareLineSegmentWithBox::Segment & currentSegment) 
	{
		m_currentSegment = currentSegment;
		m_direction = m_currentSegment.P1 - m_currentSegment.P0;
		m_length = ~m_direction;
		m_direction /= m_length;
	}


	void idle(){}


	float getDistance(const BoundingSphere & sphere)
	{


		//check if one or both ends are inside
		bool isBeginIn = sphere.Contains(m_currentSegment.P0);
		bool isEndIn = sphere.Contains(m_currentSegment.P1);

		if (isBeginIn && isEndIn)
		{
			//score = -100;
			return -100;
		}else if (isBeginIn || isEndIn)
		{
			//score = -10;
			return -10;
		}


		Vector3 center = sphere.getCenter();
		Vector3 toCenter = center - m_currentSegment.P0;
		float projection = toCenter*m_direction;

		if (projection <= 0)
		{
			return ~(center - m_currentSegment.P0);
		}
		if (projection >= m_length)
		{
			return ~(center - m_currentSegment.P1);
		}

		Vector3 projectedPoint = (m_currentSegment.P0 + projection*m_direction);
		return ~(center - projectedPoint);

	}


	bool operator()(ShapeSphere* _move1, ShapeSphere* _move2) const
	{
		return _move1->getRanking() > _move2->getRanking();
	}

};