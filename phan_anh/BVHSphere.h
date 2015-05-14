#pragma once

#include <algorithm>
#include <vector>
#include "ShapeSphere.h"
#include "CompareLineSegmentWithSphere.h"
#include "MovementSphere.h"

#include <conio.h>

class BVHSphere : public ShapeSphere
{
public:
	BVHSphere(void);
	~BVHSphere(void)
	{
		//_cprintf("bvh destroy \n");
	};

	BVHSphere(std::vector<ShapeSphere*> & moveList); 

	ShapeSphere* buildBranch(std::vector<ShapeSphere*> & shapes, size_t begin, size_t shape_size);

	BVHSphere(ShapeSphere* priml, ShapeSphere* prim2); 
	BVHSphere(ShapeSphere* priml, ShapeSphere* prim2, const BoundingSphere & _bbox); 

	void draw();

	void freeNode();

	void buildKnnExactMap(KnnMapType & knnMap, const size_t & k, ShapeSphere *startNode);

	static const int BVH_TEX_SIZE = 800;

private:

	void bvh_knn(NeighborListType &neighborList, CheckListType & checkList, 
				const size_t & numberOfNeighbors, CompareLineSegmentWithSphere & lineBoxComparer, 
				CompareLineSegment & lineLineComparer, const size_t & queryingMoveId);

	ShapeSphere* left; 
	ShapeSphere* right;

};



class CmpSphere
{
private:
	int axis;
public:

	typedef TPoint3d<float> Vector3;

	CmpSphere(int _axis) {axis = _axis;}

	bool operator()(ShapeSphere* bvh1, ShapeSphere* bvh2) const
	{
		BoundingSphere _bbox1 = bvh1->getBoundingSphere();
		BoundingSphere _bbox2 = bvh2->getBoundingSphere();

		Vector3 c1 = _bbox1.getCenter();
		Vector3 c2 = _bbox2.getCenter();

		return c1[axis] < c2[axis];
	}

};



