#include "MovementSphere.h"
#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include <queue>

MovementSphere::MovementSphere(void)
{
}



void MovementSphere::freeNode()
{
	//_cprintf("free leaf %d \n", m_OriginalId);
}

MovementSphere::MovementSphere(ShapeSphere::Vector3 _begin, ShapeSphere::Vector3 _end, 
							   Vector3 begin_axis, Vector3 end_axis, float radius, size_t moveId) 
{
	begin = _begin;
	end = _end;
	m_OriginalId = moveId;
	m_ToolRadius = radius;
	m_begin_axis = begin_axis;
	m_end_axis = end_axis;

	m_BoundingSphere.init(begin, end);

}

void MovementSphere::draw()
{
	//glBegin(GL_LINES);
	//
	//glColor3f(1.0f, 0.0f, 0.0f);
	//glVertex3f(begin.x(), begin.y(), begin.z());
	//glVertex3f(end.x(), end.y(), end.z());
	//
	//glEnd();

	//_cprintf("move id: %d \n", m_OriginalId );


}



void MovementSphere::allNNExact(std::vector<ShapeSphere*> &neighborList, size_t numberOfNeighbors, ShapeSphere *startNode)
{
	CompareLineSegmentWithBox::Segment moveSegment;
	moveSegment.P0 = this->getBegin();
	moveSegment.P1 = this->getEnd();

	CompareLineSegmentWithSphere lineBoxComparer(moveSegment);
	CompareLineSegment lineLineComparer(moveSegment);

	//the closest one is at the top
	CheckListType checkList( (lineBoxComparer) );
	float nodePriority = 100000000.0f;
	//the furthest one is at the top
	NeighborListType neighbors( (lineLineComparer) );

	float smallestDistanceFoundSoFar = 100000000.0f;
	ShapeSphere* currentNode = startNode;

	do 
	{
		currentNode->bvh_knn(neighbors, checkList, numberOfNeighbors, lineBoxComparer, lineLineComparer, this->m_OriginalId);
		
		currentNode = checkList.top();
		nodePriority = currentNode->getRanking();

		//_cprintf("check list priority %f \n", nodePriority);
		
		checkList.pop();
		
		if (neighbors.size() == numberOfNeighbors)
		{
			smallestDistanceFoundSoFar = neighbors.top()->getRanking();
		}

	} while (nodePriority <= smallestDistanceFoundSoFar && !checkList.empty());

	//copy the result, the furthest one is at the beginning
	size_t actualSize = neighbors.size();//less than or equal to numberOfNeighbors
	neighborList.resize(actualSize);
	for (size_t i = 0; i < actualSize; ++i)
	{
		//neighborList.push_back(neighbors.top());
		neighborList[actualSize - i - 1] = neighbors.top();
		neighbors.pop();
	}
}



void MovementSphere::bvh_knn(NeighborListType &neighborList, CheckListType & checkList, 
			 const size_t & numberOfNeighbors, CompareLineSegmentWithSphere & lineBoxComparer, 
			 CompareLineSegment & lineLineComparer, const size_t & queryingMoveId)
{

	if ( queryingMoveId == this->m_OriginalId )
	{
		return;
	}

	float distance = lineLineComparer.getDistance(this);
	this->setRanking(distance);

	//_cprintf("distance line - sphere %f \n", distance);

	if (neighborList.empty())
	{
		neighborList.push(this);
		return;
	}
	else
	{
		ShapeSphere* kth_neighbor = neighborList.top();

		if (distance < kth_neighbor->getRanking() || neighborList.size() < numberOfNeighbors)
		{

			//to do
			//remove the co-linear continuous neighbors of the top 4
			//if (neighborList.size() < 5)
			{
				neighborList.push(this);
			}
			
		}
	}
	//only keep numberOfNeighbors
	while (neighborList.size() > numberOfNeighbors)
	{
		//_cprintf("neighborList size %d \n", neighborList.size());
		neighborList.pop();
	}

	//dummy by pass warning
	checkList.size();
	lineBoxComparer.idle();
	
}



void MovementSphere::buildKnnExactMap(KnnMapType & knnMap, const size_t & k, ShapeSphere *startNode)
{
	std::vector<ShapeSphere*> neighbors;

	allNNExact(neighbors, k, startNode);

	std::vector<NeighborItem> knn(neighbors.size());

	//_cprintf("neighbors of the move id %d is \n", m_OriginalId);
	for (size_t i = 0; i < neighbors.size(); ++i)
	{
		MovementSphere * move = static_cast<MovementSphere *>(neighbors[i]);
		//_cprintf("%d ", move->getMoveId());
		NeighborItem item;
		item.id = move->getMoveId();
		item.ranking = move->getRanking();
		knn[i] = item;
	}
	//_cprintf("\n");
	knnMap[m_OriginalId] = knn;
}