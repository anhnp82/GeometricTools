#pragma once

#include <algorithm>
#include <vector>
#include "Shape.h"
#include "CompareLineSegmentWithBox.h"
#include "Movement.h"

class BVH : public Shape
{
public:
	BVH(void);
	~BVH(void);



	const BBox & getBbox()
	{
		return bbox;
	}



	BVH(std::vector<Shape*> & moveList); 

	Shape* buildBranch (std::vector<Shape*> & shapes, size_t begin, size_t shape_size);

	BVH(Shape* priml, Shape* prim2); 
	BVH(Shape* priml, Shape* prim2, const BBox & _bbox); 

	void draw();
	void writeToTexture(GLfloat* bvh_tex, const int & escIdxFromParentToRightChild);
	void textureIndexing(int & index, Shape * _parent, Shape* root);

	//recursively collect the relative list
	void getRelativeList(std::vector<Shape*> & relativeList, int queryingNodeId);

	void freeNode();

	void buildKnnExactMap(KnnMapType & knnMap, const size_t & k);

	void buildKNearestNeighborsMap(KnnMapType & knnMap, const size_t & k, const size_t & layers);

	static const int BVH_TEX_SIZE = 800;

private:

	void draw_quad_frame(Vector3 point1, Vector3 point2, Vector3 point3, Vector3 point4);

	void draw_cube();

	void bvh_knn(priority_queue<Shape*, std::vector<Shape*>, CompareMoveByLineSegment> &neighborList, 
				priority_queue<Shape*, std::vector<Shape*>, CompareLineSegmentWithBox> & checkList, 
				const size_t & numberOfNeighbors, CompareLineSegmentWithBox & lineBoxComparer, 
				CompareMoveByLineSegment & lineLineComparer, const size_t & queryingMoveId);

	BBox bbox; 
	Shape* left; 
	Shape* right;

};



class CmpBox
{
private:
	int axis;
public:

	typedef cadcam::mw3dBoundingBox<float> BBox;
	typedef cadcam::mwTPoint3d<float> Vector3;

	CmpBox(int _axis) {axis = _axis;}

	bool operator()(Shape* bvh1, Shape* bvh2) const
	{
		BBox _bbox1 = bvh1->getBbox();
		BBox _bbox2 = bvh2->getBbox();

		Vector3 c1 = _bbox1.GetCenter();
		Vector3 c2 = _bbox2.GetCenter();

		return c1[axis] < c2[axis];
	}

};



