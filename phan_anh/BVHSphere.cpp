#include "BVHSphere.h"


BVHSphere::BVHSphere(void)
{
}



void BVHSphere::freeNode()
{
	 left->freeNode();
	 right->freeNode();
	
	//post-order visit
	//post-order visit
	if (left == right)
	{
		delete left;
	} 
	else
	{
		delete left;
		delete right;
	}
}



inline BVHSphere::BVHSphere(ShapeSphere* priml, ShapeSphere* prim2, const BoundingSphere & _bbox) { 
	m_BoundingSphere = _bbox; 
	left = priml; 
	right = prim2; 
} 

inline BVHSphere::BVHSphere(ShapeSphere* priml, ShapeSphere* prim2) { 
	left = priml; 
	right = prim2; 
	m_BoundingSphere = priml->getBoundingSphere();
	m_BoundingSphere += prim2->getBoundingSphere(); 
}



BVHSphere::BVHSphere(std::vector<ShapeSphere*> & shapes) 
{ 

	///*

	if (shapes.size() == 1) *this = BVHSphere(shapes[0], shapes[0]); 
	if (shapes.size() == 2) *this = BVHSphere(shapes[0], shapes[1]); 

	if (shapes.size() >= 3)
	{
		m_BoundingSphere = shapes[0]->getBoundingSphere(); 
		for (int i = 1; i < shapes.size(); i++) 
			m_BoundingSphere += shapes[i]->getBoundingSphere(); 

		//choose the largest axis
		int axis = m_BoundingSphere.GetElongatedDim();

		//median
		size_t mid_point = shapes.size()/2; 
		std::nth_element(shapes.begin(), shapes.begin() + mid_point, shapes.end(), CmpSphere(axis));
		
		// create a new boundingVolume 
		left = buildBranch(shapes, 0, mid_point); 
		right = buildBranch(shapes, mid_point, shapes.size() - mid_point); 
	}

	//*/

}



ShapeSphere* BVHSphere::buildBranch (std::vector<ShapeSphere*> & shapes, size_t begin, size_t shape_size) 
{
	//_cprintf("begin: %d \n", begin );
	if (shape_size == 1) return shapes[begin]; 
	if (shape_size == 2) return new BVHSphere(shapes[begin], shapes[begin+1]); 

	BoundingSphere sphere = shapes[begin]->getBoundingSphere(); 
	for (int i = 1; i < shape_size; i++) 
		sphere += shapes[begin + i]->getBoundingSphere(); 

	//choose the largest axis
	int axis = sphere.GetElongatedDim();

	//median
	size_t mid_point = shape_size/2; 
	std::nth_element(shapes.begin()+begin, shapes.begin() + begin + mid_point, 
		shapes.begin() + begin + shape_size, CmpSphere(axis));

	// create a new boundingVolume 
	ShapeSphere* left = buildBranch(shapes, begin, mid_point); 
	ShapeSphere* right = buildBranch(shapes, begin + mid_point, shape_size-mid_point); 
	return new BVHSphere(left, right, sphere); 
}



void BVHSphere::draw()
{
	//pre-order

	//_cprintf("bvh \n" );

	left->draw();
	right->draw();
}



void BVHSphere::buildKnnExactMap(KnnMapType & knnMap, const size_t & k, ShapeSphere *startNode)
{
	left->buildKnnExactMap(knnMap, k, startNode);
	right->buildKnnExactMap(knnMap, k, startNode);
}



void BVHSphere::bvh_knn(NeighborListType &neighborList, CheckListType & checkList, 
			 const size_t & numberOfNeighbors, CompareLineSegmentWithSphere & lineBoxComparer, 
			 CompareLineSegment & lineLineComparer, const size_t & queryingMoveId)
{

	///*

	BoundingSphere sphere1 = left->getBoundingSphere();
	BoundingSphere sphere2 = right->getBoundingSphere();

	float dist1 = lineBoxComparer.getDistance(sphere1);
	float dist2 = lineBoxComparer.getDistance(sphere2);

	left->setRanking(dist1);
	right->setRanking(dist2);

	checkList.push(left);
	checkList.push(right);
	
	//dummy by pass warning
	lineLineComparer.idle();
	size_t dummy = numberOfNeighbors;
	dummy = neighborList.size();
	dummy = queryingMoveId;
	
	//*/

}




