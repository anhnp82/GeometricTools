#pragma once


#include "Shape.h"

class CompareLineSegmentWithBox
{
public:

	typedef cadcam::mwTPoint3d<float> Vector3;
	typedef cadcam::mw3dBoundingBox<float> BBox;

	//compute distance between 2 line segments
	struct Segment 
	{
		Vector3 P0;
		Vector3 P1;
	};

	float getDistance(const BBox & box)
	{
		return distToBox(m_CurrentMove, box);
	}

	void idle(){};

	float dist3D_Segment_to_Segment(const Segment & S1, const Segment & S2) const;
	float distToBox(const Segment & move, const BBox & box);

	CompareLineSegmentWithBox(void);

	CompareLineSegmentWithBox(const Segment & _currentMovement) 
	{
		m_CurrentMove = _currentMovement;
	}

	bool operator()(Shape* _bvh1, Shape* _bvh2) const;

	~CompareLineSegmentWithBox(void);

private:

	Segment m_CurrentMove;

	float distToRectangle(const Vector3 & lowerCorner, const Vector3 & upperCorner, size_t normalDimension, const Vector3 & p, 
		bool & isInside);
	float distToRectangle(const Segment & move, const Vector3 & lowerCorner, const Vector3 & upperCorner, size_t normalDimension);
};



