#include "CompareLineSegmentWithBox.h"

CompareLineSegmentWithBox::CompareLineSegmentWithBox(void)
{
}

CompareLineSegmentWithBox::~CompareLineSegmentWithBox(void)
{
}


bool CompareLineSegmentWithBox::operator()(Shape* _bvh1, Shape* _bvh2) const
{

	//BBox box1 = _bvh1->getBbox();
	//BBox box2 = _bvh2->getBbox();

	//float dist1 = distToBox(m_CurrentMove, box1);
	//float dist2 = distToBox(m_CurrentMove, box2);

	//_bvh1->setRanking(dist1);
	//_bvh2->setRanking(dist2);

	//return dist1 > dist2;

	return _bvh1->getRanking() > _bvh2->getRanking();
}


float CompareLineSegmentWithBox::distToBox(const Segment & move, const Shape::BBox & box)
{
	float score = 1000000.0f;
	//check if one or both ends are inside
	bool isBeginIn = box.Contains(move.P0);
	bool isEndIn = box.Contains(move.P1);

	if (isBeginIn && isEndIn)
	{
		score = -100;
		return score;
	}else if (isBeginIn || isEndIn)
	{
		score = -10;
		return score;
	}

	//this time, the move is outside the box
	//compare distance from the movement to each side of the box
	Vector3 p1, p2;
	float dists[6];

	p1 = box.GetBottomLeftNearCorner();
	p2 = Vector3(box.GetBottomLeftNearCorner().x(), box.GetUpperRightFarCorner().y(), box.GetUpperRightFarCorner().z());
	dists[0] = distToRectangle(move, p1, p2, 0);

	p1 = Vector3(box.GetUpperRightFarCorner().x(), box.GetBottomLeftNearCorner().y(), box.GetBottomLeftNearCorner().z());
	p2 = box.GetUpperRightFarCorner();
	dists[1] = distToRectangle(move, p1, p2, 0);

	p1 = box.GetBottomLeftNearCorner();
	p2 = Vector3(box.GetUpperRightFarCorner().x(), box.GetBottomLeftNearCorner().y(), box.GetUpperRightFarCorner().z());
	dists[2] = distToRectangle(move, p1, p2, 1);

	p1 = Vector3(box.GetBottomLeftNearCorner().x(), box.GetUpperRightFarCorner().y(), box.GetBottomLeftNearCorner().z());
	p2 = box.GetUpperRightFarCorner();
	dists[3] = distToRectangle(move, p1, p2, 1);

	p1 = box.GetBottomLeftNearCorner();
	p2 = Vector3(box.GetUpperRightFarCorner().x(), box.GetUpperRightFarCorner().y(), box.GetBottomLeftNearCorner().z());
	dists[4] = distToRectangle(move, p1, p2, 2);

	p1 = Vector3(box.GetBottomLeftNearCorner().x(), box.GetBottomLeftNearCorner().y(), box.GetUpperRightFarCorner().z());
	p2 = box.GetUpperRightFarCorner();
	dists[5] = distToRectangle(move, p1, p2, 2);

	for (size_t i = 0; i < 6; i++)
	{
		if (dists[i] < score)
		{
			score = dists[i];
		}
	}
	return score;
}







float CompareLineSegmentWithBox::distToRectangle(const Vector3 & lowerCorner, const Vector3 & upperCorner, size_t normalDimension, const Vector3 & p, 
								bool & isInside)
{
	Vector3 normal;

	switch (normalDimension)
	{
	case 0:
		isInside = (p.y() <= upperCorner.y() && p.y() >= lowerCorner.y() && p.z() <= upperCorner.z() && p.z() >= lowerCorner.z() );
		normal = Vector3(1, 0, 0);
		break;
	case 1:
		isInside = (p.x() <= upperCorner.x() && p.x() >= lowerCorner.x() && p.z() <= upperCorner.z() && p.z() >= lowerCorner.z() );
		normal = Vector3(0, 1, 0);
		break;
	case 2:
		isInside = (p.y() <= upperCorner.y() && p.y() >= lowerCorner.y() && p.x() <= upperCorner.x() && p.x() >= lowerCorner.x() );
		normal = Vector3(0, 0, 1);
		break;
	}

	float projection = normal*(p - lowerCorner);
	return fabs(projection);
}



float CompareLineSegmentWithBox::distToRectangle(const Segment & moveSegment, const Vector3 & lowerCorner, const Vector3 & upperCorner, size_t normalDimension)
{
	bool isBeginInside = false;
	bool isEndInside = false;

	float distBegin = distToRectangle(lowerCorner, upperCorner, normalDimension, moveSegment.P0, isBeginInside);
	float distEnd = distToRectangle(lowerCorner, upperCorner, normalDimension, moveSegment.P1, isEndInside);

	if (isBeginInside && isEndInside) return distBegin < distEnd ? distBegin : distEnd;

	//at this state we need to check with 4 lines
	float distToLines = 1000000.0f;
	Segment seg1, seg2, seg3, seg4;

	Vector3 topLeft, bottomRight;

	switch (normalDimension)
	{
	case 0:

		topLeft = Vector3(lowerCorner.x(), lowerCorner.y(), upperCorner.z());
		bottomRight = Vector3(lowerCorner.x(), upperCorner.y(), lowerCorner.z());

		break;
	case 1:

		topLeft = Vector3(lowerCorner.x(), lowerCorner.y(), upperCorner.z());
		bottomRight = Vector3(upperCorner.x(), upperCorner.y(), lowerCorner.z());

		break;
	case 2:

		topLeft = Vector3(lowerCorner.x(), upperCorner.y(), upperCorner.z());
		bottomRight = Vector3(upperCorner.x(), lowerCorner.y(), lowerCorner.z());

		break;
	}

	seg1.P0 = lowerCorner;
	seg1.P1 = topLeft;
	seg2.P0 = topLeft;
	seg2.P1 = upperCorner;
	seg3.P0 = upperCorner;
	seg3.P1 = bottomRight;
	seg4.P0 = bottomRight;
	seg4.P1 = lowerCorner;

	float distToLine1 = dist3D_Segment_to_Segment(moveSegment, seg1);
	float distToLine2 = dist3D_Segment_to_Segment(moveSegment, seg2);
	float distToLine3 = dist3D_Segment_to_Segment(moveSegment, seg3);
	float distToLine4 = dist3D_Segment_to_Segment(moveSegment, seg4);
	if (distToLine1 < distToLines) distToLines = distToLine1;
	if (distToLine2 < distToLines) distToLines = distToLine2;
	if (distToLine3 < distToLines) distToLines = distToLine3;
	if (distToLine4 < distToLines) distToLines = distToLine4;

	if (isBeginInside)
	{
		return distBegin < distToLines ? distBegin : distToLines;
	}

	if (isEndInside)
	{
		return distEnd < distToLines ? distEnd : distToLines;
	}

	return distToLines;

}



//http://geomalgorithms.com/a07-_distance.html
// dist3D_Segment_to_Segment(): get the 3D minimum distance between 2 segments
//    Input:  two 3D line segments S1 and S2
//    Return: the shortest distance between S1 and S2
float CompareLineSegmentWithBox::dist3D_Segment_to_Segment(const Segment & S1, const Segment & S2) const
{
	float SMALL_NUM = 0.00000001f;

	Vector3   u = S1.P1 - S1.P0;
	Vector3   v = S2.P1 - S2.P0;
	Vector3   w = S1.P0 - S2.P0;
	float    a = (u*u);         // always >= 0
	float    b = (u*v);
	float    c = (v*v);         // always >= 0
	float    d = (u*w);
	float    e = (v*w);
	float    D = a*c - b*b;        // always >= 0
	float    sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
	float    tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

	// compute the line parameters of the two closest points
	if (D < SMALL_NUM) { // the lines are almost parallel
		sN = 0.0;         // force using point P0 on segment S1
		sD = 1.0;         // to prevent possible division by 0.0 later
		tN = e;
		tD = c;
	}
	else {                 // get the closest points on the infinite lines
		sN = (b*e - c*d);
		tN = (a*e - b*d);
		if (sN < 0.0) {        // sc < 0 => the s=0 edge is visible
			sN = 0.0;
			tN = e;
			tD = c;
		}
		else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0) {            // tc < 0 => the t=0 edge is visible
		tN = 0.0;
		// recompute sc for this edge
		if (-d < 0.0)
			sN = 0.0;
		else if (-d > a)
			sN = sD;
		else {
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
		tN = tD;
		// recompute sc for this edge
		if ((-d + b) < 0.0)
			sN = 0;
		else if ((-d + b) > a)
			sN = sD;
		else {
			sN = (-d +  b);
			sD = a;
		}
	}
	// finally do the division to get sc and tc
	sc = (abs(sN) < SMALL_NUM ? 0.0f : sN / sD);
	tc = (abs(tN) < SMALL_NUM ? 0.0f : tN / tD);

	// get the difference of the two closest points
	Vector3   dP = w + (sc * u) - (tc * v);  // =  S1(sc) - S2(tc)

	return ~(dP);   // return the closest distance
}
//===================================================================