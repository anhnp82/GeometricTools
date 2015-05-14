#pragma once
#include "Shape.h"
#include "CompareLineSegmentWithBox.h"

#pragma warning (disable: 4201)
#include "glm-0.9.3.4/glm/glm.hpp"
#pragma warning (default: 4201)



class Movement :
	public Shape
{
public:
	Movement(void);
	~Movement(void);

	Movement(Vector3 _begin, Vector3 _end, Vector3 _beginAxis, Vector3 _endAxis, float radius, size_t moveId);

	const BBox & getBbox()
	{
		return box;
	}

	//recursively collect the relative list
	void getRelativeList(std::vector<Shape*> & relativeList, int queryingNodeId);
	//initialized by the querying node
	void initRelativeList(std::vector<Shape*> & relativeList, Shape * _ancestor);

	/*
	* since this is a balanced binary tree, we can get from 6 to 12 elements within 3 layers 
	*/
	Shape* get3LayerAncestor();
	Shape* get4LayerAncestor();
	Shape* get5LayerAncestor();
	Shape* get6LayerAncestor();

	void draw();
	void writeToTexture(GLfloat* bvh_tex, const int & escIdxFromParentToRightChild);
	void textureIndexing(int & index, Shape * _parent, Shape* root);
	void freeNode();

	void buildKNearestNeighborsMap(KnnMapType & knnMap, const size_t & k, const size_t & layers);

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
		return originalId;
	}


	void buildKnnExactMap(KnnMapType & knnMap, const size_t & k);

	void allNNExact(std::vector<Shape*> &neighborList, size_t numberOfNeighbors);
	

private:

	Vector3 begin;
	Vector3 end;
	Vector3 m_begin_axis;
	Vector3 m_end_axis;
	BBox box;
	float radius;
	size_t originalId;



	void bvh_knn(priority_queue<Shape*, std::vector<Shape*>, CompareMoveByLineSegment> &neighborList, 
				priority_queue<Shape*, std::vector<Shape*>, CompareLineSegmentWithBox> & checkList, 
				const size_t & numberOfNeighbors, CompareLineSegmentWithBox & lineBoxComparer, 
				CompareMoveByLineSegment & lineLineComparer, const size_t & queryingMoveId);

	void neighborhoodFilter(const std::vector<Shape*> & inList, std::vector<Shape*> & outList, size_t k);
	void neighborhoodFilterRedundant(const std::vector<Shape*> & inList, std::vector<Shape*> & outList, size_t k);

};



class CompareMove
{

	typedef cadcam::mwTPoint3d<float> Vector3;

private:

	//Movement* currentMove;
	Vector3 currentMean;

public:

	CompareMove(Movement * _currentMovement) 
	{
		currentMean = ( _currentMovement->getBegin() + _currentMovement->getEnd() )/2.0f;
	}

	bool operator()(Shape* _move1, Shape* _move2) const
	{

		Movement * move1 = static_cast<Movement *>(_move1);
		Movement * move2 = static_cast<Movement *>(_move2);

		Vector3 mean1 = ( move1->getBegin() + move1->getEnd() )/2.0f;
		Vector3 mean2 = ( move2->getBegin() + move2->getEnd() )/2.0f;

		float dist1 = (mean1 - currentMean).Length2();
		float dist2 = (mean2 - currentMean).Length2();

		//_move1->setRanking(dist1);
		//_move2->setRanking(dist2);

		return dist1 < dist2;
	}

};



class CompareMoveByLineSegment
{

	typedef cadcam::mwTPoint3d<float> Vector3;

private:

	CompareLineSegmentWithBox m_CompareUtils;
	CompareLineSegmentWithBox::Segment m_currentSegment;

public:

	CompareMoveByLineSegment(const CompareLineSegmentWithBox::Segment & currentSegment) //: m_CompareUtils(currentSegment)
	{
		m_currentSegment = currentSegment;
	}


	void idle(){}


	float getDistance(Movement * _aMove)
	{
		CompareLineSegmentWithBox::Segment seg;
		seg.P0 = _aMove->getBegin();
		seg.P1 = _aMove->getEnd();
		return m_CompareUtils.dist3D_Segment_to_Segment(seg, m_currentSegment);
	}


	bool operator()(Shape* _move1, Shape* _move2) const
	{

		//Movement * move1 = static_cast<Movement *>(_move1);
		//Movement * move2 = static_cast<Movement *>(_move2);

		//CompareLineSegmentWithBox::Segment seg1, seg2;
		//seg1.P0 = move1->getBegin();
		//seg1.P1 = move1->getEnd();
		//seg2.P0 = move2->getBegin();
		//seg2.P1 = move2->getEnd();

		//float dist1 = m_CompareUtils.dist3D_Segment_to_Segment(seg1, m_currentSegment);
		//float dist2 = m_CompareUtils.dist3D_Segment_to_Segment(seg2, m_currentSegment);

		//_move1->setRanking(dist1);
		//_move2->setRanking(dist2);

		//return dist1 < dist2;

		return _move1->getRanking() < _move2->getRanking();
	}

};



class CompareMoveSegment
{

	typedef cadcam::mwTPoint3d<float> Vector3;

private:

	CompareLineSegmentWithBox m_CompareUtils;
	CompareLineSegmentWithBox::Segment m_currentSegment;

public:

	CompareMoveSegment(Movement * _currentMovement) 
	{
		m_currentSegment.P0 = _currentMovement->getBegin();
		m_currentSegment.P1 = _currentMovement->getEnd();
	}

	bool operator()(Shape* _move1, Shape* _move2) const
	{

		Movement * move1 = static_cast<Movement *>(_move1);
		Movement * move2 = static_cast<Movement *>(_move2);

		CompareLineSegmentWithBox::Segment seg1, seg2;
		seg1.P0 = move1->getBegin();
		seg1.P1 = move1->getEnd();
		seg2.P0 = move2->getBegin();
		seg2.P1 = move2->getEnd();

		float dist1 = m_CompareUtils.dist3D_Segment_to_Segment(seg1, m_currentSegment);
		float dist2 = m_CompareUtils.dist3D_Segment_to_Segment(seg2, m_currentSegment);

		//_move1->setRanking(dist1);
		//_move2->setRanking(dist2);

		return dist1 < dist2;
	}

};