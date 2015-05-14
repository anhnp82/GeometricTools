#include "BVH.h"
#include <conio.h>

BVH::BVH(void)
{
}

BVH::~BVH(void)
{
}

void BVH::freeNode()
{
	left->freeNode();
	right->freeNode();
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



inline BVH::BVH(Shape* priml, Shape* prim2, const BBox & _bbox) { 
	bbox = _bbox; 
	left = priml; 
	right = prim2; 
} 

inline BVH::BVH(Shape* priml, Shape* prim2) { 
	left = priml; 
	right = prim2; 
	bbox = priml->getBbox() + prim2->getBbox(); 
}






BVH::BVH(std::vector<Shape*> & shapes) 
{ 
	if (shapes.size() == 1) *this = BVH(shapes[0], shapes[0]); 
	if (shapes.size() == 2) *this = BVH(shapes[0], shapes[1]); 

	if (shapes.size() >= 3)
	{
		bbox = shapes[0]->getBbox(); 
		for (int i = 1; i < shapes.size(); i++) 
			bbox += shapes[i]->getBbox(); 

		//choose the largest axis
		float max = -1.0f;
		int axis = 0;
		if (bbox.GetWidth() > bbox.GetHeight())
		{
			max = bbox.GetWidth();
			axis = 0;
		} 
		else
		{
			max = bbox.GetHeight();
			axis = 1;
		}
		if (bbox.GetDepth() > max) axis = 2;

		//median
		size_t mid_point = shapes.size()/2; 
		std::nth_element(shapes.begin(), shapes.begin() + mid_point, shapes.end(), CmpBox(axis));
		
		// create a new boundingVolume 
		left = buildBranch(shapes, 0, mid_point); 
		right = buildBranch(shapes, mid_point, shapes.size() - mid_point); 
	}

}


Shape* BVH::buildBranch (std::vector<Shape*> & shapes, size_t begin, size_t shape_size) 
{
	//_cprintf("begin: %d \n", begin );
	if (shape_size == 1) return shapes[begin]; 
	if (shape_size == 2) return new BVH(shapes[begin], shapes[begin+1]); 

	BBox box = shapes[begin]->getBbox(); 
	for (int i = 1; i < shape_size; i++) 
		box += shapes[begin + i]->getBbox(); 

	//choose the largest axis
	float max = -1.0f;
	int axis = 0;
	if (box.GetWidth() > box.GetHeight())
	{
		max = box.GetWidth();
		axis = 0;
	} 
	else
	{
		max = box.GetHeight();
		axis = 1;
	}
	if (box.GetDepth() > max) axis = 2;

	//median
	size_t mid_point = shape_size/2; 
	std::nth_element(shapes.begin()+begin, shapes.begin() + begin + mid_point, 
		shapes.begin() + begin + shape_size, CmpBox(axis));

	// create a new boundingVolume 
	Shape* left = buildBranch(shapes, begin, mid_point); 
	Shape* right = buildBranch(shapes, begin + mid_point, shape_size-mid_point); 
	return new BVH(left, right, box); 
}




void BVH::draw_quad_frame(Vector3 point1, Vector3 point2, Vector3 point3, Vector3 point4)
{

	glBegin(GL_LINE_LOOP);

	glColor3f(0.0f, 0.0f, 1.0f);

	glVertex3f(point1.x(), point1.y(), point1.z());
	glVertex3f(point2.x(), point2.y(), point2.z());
	glVertex3f(point3.x(), point3.y(), point3.z());
	glVertex3f(point4.x(), point4.y(), point4.z());

	glEnd();
	
}

void BVH::draw_cube()
{
	Vector3 point1 = bbox.GetBottomLeftNearCorner();
	Vector3 point8 = bbox.GetUpperRightFarCorner();

	Vector3 point2 = Vector3(point8.x(), point1.y(), point1.z());
	Vector3 point3 = Vector3(point8.x(), point1.y(), point8.z());
	Vector3 point4 = Vector3(point1.x(), point1.y(), point8.z());
	Vector3 point5 = Vector3(point1.x(), point8.y(), point8.z());
	Vector3 point6 = Vector3(point1.x(), point8.y(), point1.z());
	Vector3 point7 = Vector3(point8.x(), point8.y(), point1.z());

	draw_quad_frame(point1, point2, point3, point4);
	draw_quad_frame(point5, point6, point7, point8);
	draw_quad_frame(point2, point3, point8, point7);
	draw_quad_frame(point1, point4, point5, point6);
	draw_quad_frame(point1, point2, point7, point6);
	draw_quad_frame(point4, point3, point8, point5);
}

void BVH::draw()
{
	//pre-order
	draw_cube();
	if (parent != NULL)
	{
		_cprintf("bvh id: %d, parent id: %d \n", m_id, parent->getId() );
	} 
	else
	{
		_cprintf("bvh id: %d \n", m_id );
	}

	left->draw();
	right->draw();
}



void BVH::textureIndexing(int & index, Shape * _parent, Shape* root)
{
	/*
	_cprintf("bvh id: %d, (%f, %f, %f), (%f, %f, %f) \n", index, 
		bbox.GetBottomLeftNearCorner().x(), bbox.GetBottomLeftNearCorner().y(), bbox.GetBottomLeftNearCorner().z(), 
		bbox.GetUpperRightFarCorner().x(), bbox.GetUpperRightFarCorner().y(), bbox.GetUpperRightFarCorner().z());
	//*/

	//pre-order visit
	m_id = index;
	index++;
	parent = _parent;
	m_root = root;

	left->textureIndexing(index, this, root);
	right->textureIndexing(index, this, root);
}

void BVH::writeToTexture(GLfloat* bvh_tex, const int & escIdxFromParentToRightChild)
{
	//pre-order visit
	//_cprintf("bvh id: %d, escape index: %d \n", m_id, escIdxFromParentToRightChild );
	///*
	bvh_tex[m_id*8] = bbox.GetBottomLeftNearCorner().x();
	bvh_tex[m_id*8+1] = bbox.GetBottomLeftNearCorner().y();
	bvh_tex[m_id*8+2] = bbox.GetBottomLeftNearCorner().z();
	bvh_tex[m_id*8+3] = 1.0f;//type 1 for bvh

	bvh_tex[m_id*8+4] = bbox.GetUpperRightFarCorner().x();
	bvh_tex[m_id*8+5] = bbox.GetUpperRightFarCorner().y();
	bvh_tex[m_id*8+6] = bbox.GetUpperRightFarCorner().z();
	bvh_tex[m_id*8+7] = (float)escIdxFromParentToRightChild*2;//escape index

	//_cprintf("%f %f %f %f %f %f %f %f ", bvh_tex[m_id*8], bvh_tex[m_id*8+1], bvh_tex[m_id*8+2], bvh_tex[m_id*8+3], 
	//	bvh_tex[m_id*8+4], bvh_tex[m_id*8+5], bvh_tex[m_id*8+6], bvh_tex[m_id*8+7] );
	//*/
	//end of visit

	left->writeToTexture(bvh_tex, right->getId());
	right->writeToTexture(bvh_tex, escIdxFromParentToRightChild);
}





void BVH::getRelativeList(std::vector<Shape*> &relativeList, int queryingNodeId)
{
	left->getRelativeList(relativeList, queryingNodeId);
	right->getRelativeList(relativeList, queryingNodeId);
}



void BVH::buildKNearestNeighborsMap(KnnMapType & knnMap, const size_t & k, const size_t & layers)
{
	left->buildKNearestNeighborsMap(knnMap, k, layers);
	right->buildKNearestNeighborsMap(knnMap, k, layers);
}


void BVH::buildKnnExactMap(KnnMapType & knnMap, const size_t & k)
{
	left->buildKnnExactMap(knnMap, k);
	right->buildKnnExactMap(knnMap, k);
}



void BVH::bvh_knn(priority_queue<Shape*, std::vector<Shape*>, CompareMoveByLineSegment> &neighborList, 
			 priority_queue<Shape*, std::vector<Shape*>, CompareLineSegmentWithBox> & checkList, 
			 const size_t & numberOfNeighbors, CompareLineSegmentWithBox & lineBoxComparer, 
			 CompareMoveByLineSegment & lineLineComparer, const size_t & queryingMoveId)
{

	BBox box1 = left->getBbox();
	BBox box2 = right->getBbox();

	float dist1 = lineBoxComparer.getDistance(box1);
	float dist2 = lineBoxComparer.getDistance(box2);

	left->setRanking(dist1);
	right->setRanking(dist2);

	checkList.push(left);
	checkList.push(right);
	
	//dummy by pass warning
	lineLineComparer.idle();
	size_t dummy = numberOfNeighbors;
	dummy = neighborList.size();
	dummy = queryingMoveId;
	
	//_cprintf("bvh left distance %f right %f checklist size %d neighborList size %d \n", dist1, dist2, checkList.size(), neighborList.size());
}