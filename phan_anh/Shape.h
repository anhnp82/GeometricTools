#pragma once


#include <GL/glew.h>
#include <map>
//#include <tr1/unordered_map>
#include "boost/unordered_map.hpp"
#include <queue>

//#define MAX_FLOAT = 3.40282e+38

using namespace std;
//using namespace std::tr1;
class CompareLineSegmentWithBox;
class CompareMoveByLineSegment;

class Shape
{
public:
	Shape(void);
	~Shape(void);

	struct NeighborItem 
	{
		size_t id;
		float ranking;
	};

	typedef cadcam::mw3dBoundingBox<float> BBox;
	typedef cadcam::mwTPoint3d<float> Vector3;
	typedef boost::unordered_map< size_t, std::vector<NeighborItem> > KnnMapType;
	//typedef map< size_t, std::vector<size_t> > KnnMapType;
	//typedef map< size_t, std::vector<NeighborItem> > KnnMapType;

	virtual const BBox & getBbox() = 0;
	virtual void draw() = 0;

	/*
	* we first have to call the textureIndexing method to get the index for every node.
	* Only BVH node needs to store the escape index.
	* Escape index of the left node = index of the right node.
	* Escape index of the right node = escape index of its parent passed through the function call.
	*/
	virtual void writeToTexture(GLfloat* bvh_tex, const int & escIdxFromParentToRightChild) = 0;
	virtual void textureIndexing(int & index, Shape * _parent, Shape* root) = 0;
	virtual void freeNode() = 0;

	//recursively collect the relative list
	virtual void getRelativeList(std::vector<Shape*> & relativeList, int queryingNodeId) = 0;

	/**
	/* The number n of nodes in a perfect binary tree can also be found using this formula: 
	/* n = 2L-1 where L is the number of leaf nodes in the tree.
	**/
	void printNoNodes();

	int getId();

	//virtual void getMoveId();

	static int getNoNodes()
	{
		return Shape::NoNodes;
	}

	float getRanking()
	{
		return ranking;
	}

	void setRanking(float aRank)
	{
		this->ranking = aRank;
	}

	Shape* getParent();

	virtual void bvh_knn(priority_queue<Shape*, std::vector<Shape*>, CompareMoveByLineSegment> &neighborList, 
						priority_queue<Shape*, std::vector<Shape*>, CompareLineSegmentWithBox> & checkList, 
						const size_t & numberOfNeighbors, CompareLineSegmentWithBox & lineBoxComparer, 
						CompareMoveByLineSegment & lineLineComparer, const size_t & queryingMoveId) = 0;

	virtual void buildKNearestNeighborsMap(KnnMapType & knnMap, 
		const size_t & k, const size_t & layers) = 0;

	virtual void buildKnnExactMap(KnnMapType & knnMap, const size_t & k) = 0;

protected:

	int m_id;
	static int NoNodes;
	Shape* parent, *m_root;
	float ranking;

};







