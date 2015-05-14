#pragma once
#include "ShapeSphere.h"
#include "CompareLineSegmentWithSphere.h"
#include "CompareLineSegmentWithBox.h"

#pragma warning (disable: 4201)
#include "glm-0.9.3.4/glm/glm.hpp"
#pragma warning (default: 4201)

#include <conio.h>

class MovementSphere :
	public ShapeSphere
{
public:
	MovementSphere(void);
	~MovementSphere(void){};

	MovementSphere(Vector3 _begin, Vector3 _end, Vector3 _beginAxis, Vector3 _endAxis, float radius, size_t moveId);



	void draw();
	void freeNode();



	Vector3 getBegin()
	{
		return begin;
	}

	Vector3 getEnd()
	{
		return end;
	}

	size_t getMoveId()
	{
		return m_OriginalId;
	}


	void buildKnnExactMap(KnnMapType & knnMap, const size_t & k, ShapeSphere *startNode);

	void allNNExact(std::vector<ShapeSphere*> &neighborList, size_t numberOfNeighbors, ShapeSphere *startNode);
	

private:

	Vector3 begin;
	Vector3 end;
	Vector3 m_begin_axis;
	Vector3 m_end_axis;
	float m_ToolRadius;
	size_t m_OriginalId;



	void bvh_knn(NeighborListType &neighborList, CheckListType & checkList, 
				const size_t & numberOfNeighbors, CompareLineSegmentWithSphere & lineBoxComparer, 
				CompareLineSegment & lineLineComparer, const size_t & queryingMoveId);



};







class CompareLineSegment
{

	typedef TPoint3d<float> Vector3;

private:

	CompareLineSegmentWithBox m_CompareUtils;
	CompareLineSegmentWithBox::Segment m_currentSegment;

public:

	CompareLineSegment(const CompareLineSegmentWithBox::Segment & currentSegment) //: m_CompareUtils(currentSegment)
	{
		m_currentSegment = currentSegment;
	}


	void idle(){}


	float getDistance(MovementSphere * _aMove)
	{
		CompareLineSegmentWithBox::Segment seg;
		seg.P0 = _aMove->getBegin();
		seg.P1 = _aMove->getEnd();
		return m_CompareUtils.dist3D_Segment_to_Segment(seg, m_currentSegment);
	}


	bool operator()(ShapeSphere* _move1, ShapeSphere* _move2) const
	{
		return _move1->getRanking() < _move2->getRanking();
	}

};



