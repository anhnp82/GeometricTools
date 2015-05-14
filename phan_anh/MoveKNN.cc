#if _MSC_VER > 1310
//== INCLUDES =================================================================

#include <iostream>

#include "MoveKNN.h"
#include <float.h>



//== IMPLEMENTATION ==========================================================




MoveKNN::Vector3 MoveKNN::getPoint(int i)
{
	return m_endPoints[i];
}



MoveKNN::MoveKNN(MoveKNN::Vector3 _begin, MoveKNN::Vector3 _end, 
				   Vector3 begin_axis, Vector3 end_axis, float radius, size_t moveId)
{
	m_box = BBox( _begin, _end );
	m_endPoints[0] = _begin;
	m_endPoints[1] = _end;
	m_segment.P0 = _begin;
	m_segment.P1 = _end;
	m_Id = moveId;
	m_radius = radius;
	m_tool_axes[0] = begin_axis;
	m_tool_axes[1] = end_axis;

	m_ranking = FLT_MAX;
}


MoveKNN::BBox MoveKNN::getBBox() const
{
	return m_box;
}



float MoveKNN::distToItem( MoveKNN * _other ) 
{
	Segment segment = _other->getSegment();
	float dist = dist3D_Segment_to_Segment(m_segment, segment);
	return dist;
}


bool MoveKNN::smallerThanValue( const float & _splitValue,
				 const int & _splitDimension ) const
{

	for( size_t i = 0; i < 2; ++i )
    {
		if( m_endPoints[i][ _splitDimension ] <= _splitValue )
			return true;
    }

    return false;
}


bool MoveKNN::largerThanValue( const float & _splitValue,
				const int & _splitDimension ) const
{

    for( size_t i = 0; i < 2; ++i )
    {
		if( m_endPoints[i][ _splitDimension ] >= _splitValue )
			return true;
    }

    return false;
}



//http://geomalgorithms.com/a07-_distance.html
// dist3D_Segment_to_Segment(): get the 3D minimum distance between 2 segments
//    Input:  two 3D line segments S1 and S2
//    Return: the shortest distance between S1 and S2
float MoveKNN::dist3D_Segment_to_Segment(const Segment & S1, const Segment & S2) const
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











#endif // _MSC_VER >1310