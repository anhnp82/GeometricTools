#ifndef MH_MOVEKNN_H
#define MH_MOVEKNN_H



#include <vector>
#include "mwMachSimVerifier.hpp"


//! Class for a tool move to be stored in a kD tree
class MoveKNN 
{

public:
	MoveKNN() {}

	typedef cadcam::mwTPoint3d<float> Vector3;
	typedef cadcam::mw3dBoundingBox<float> BBox;

	//compute distance between 2 line segments
	struct Segment 
	{
		Vector3 P0;
		Vector3 P1;
	};

	MoveKNN(Vector3 _begin, Vector3 _end, Vector3 begin_axis, Vector3 end_axis, float radius, size_t moveId);

	//! Compute distance of triangle to given point
	float distToItem( MoveKNN * _other );

	//! Does this triangle cover a region smaller than given split value?
	bool smallerThanValue( const float& _splitValue, const int& _splitDimension ) const;
	
	//! Does this triangle cover a region larger than given split value?
	bool largerThanValue( const float& _splitValue, const int& _splitDimension ) const;

	BBox getBBox() const;

	//! Get a point from the current move
	Vector3 getPoint(int i);

	size_t getId(){ return m_Id; }

	Segment getSegment() { return m_segment; }

	void setRanking(float _ranking)
	{
		m_ranking = _ranking;
	}

	float getRanking(){ return m_ranking; }

	float dist3D_Segment_to_Segment(const Segment & S1, const Segment & S2) const;

private:    

	Vector3 m_endPoints[2];
	Vector3 m_tool_axes[2];
	float m_radius;
	BBox m_box;
	size_t m_Id;
	Segment m_segment;
	float m_ranking;

	

};



class CompareMoveSegmentKdTree
{
	typedef cadcam::mwTPoint3d<float> Vector3;

private:

	MoveKNN::Segment m_currentSegment;

public:

	CompareMoveSegmentKdTree(MoveKNN * _currentMovement) 
	{
		m_currentSegment.P0 = _currentMovement->getPoint(0);
		m_currentSegment.P1 = _currentMovement->getPoint(1);
	}

	bool operator()(MoveKNN* move1, MoveKNN* move2) const
	{

		MoveKNN::Segment seg1, seg2;
		seg1.P0 = move1->getPoint(0);
		seg1.P1 = move1->getPoint(1);
		seg2.P0 = move2->getPoint(0);
		seg2.P1 = move2->getPoint(1);

		float dist1 = move1->dist3D_Segment_to_Segment(seg1, m_currentSegment);
		float dist2 = move2->dist3D_Segment_to_Segment(seg2, m_currentSegment);

		return dist1 < dist2;
	}

};


//=============================================================================
#endif // MH_MOVEKNN_H defined
//=============================================================================
