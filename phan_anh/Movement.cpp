#include "Movement.h"
#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <conio.h>
#include <queue>

Movement::Movement(void)
{
}

Movement::~Movement(void)
{
}

void Movement::freeNode()
{}

Movement::Movement(Shape::Vector3 _begin, Shape::Vector3 _end, 
				   Vector3 begin_axis, Vector3 end_axis, float radius, size_t moveId)
{
	box = BBox( _begin, _end );
	box.Inflate(radius);
	begin = _begin;
	end = _end;
	originalId = moveId;
	this->radius = radius;
	m_begin_axis = begin_axis;
	m_end_axis = end_axis;
}

void Movement::draw()
{
	glBegin(GL_LINES);
	
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(begin.x(), begin.y(), begin.z());
	glVertex3f(end.x(), end.y(), end.z());
	
	glEnd();

	//_cprintf("move id: %d, original id: %d, parent id: %d \n", m_id, originalId, parent->getId() );
	_cprintf("move id: %d, original id: %d, parent id: %d, ancestor id: %d \n", 
		m_id, originalId, parent->getId(), get3LayerAncestor()->getId() );

	/*
	if (m_id == 5)
	{
		std::vector<Shape*> neighbors;
		initRelativeList(neighbors, get3LayerAncestor());
		std::vector<Shape*> filteredNeighbors;
		neighborhoodFilter(neighbors, filteredNeighbors, 4);

		_cprintf("neighbors of the node id %d is \n", m_id);
		for (int i = 0; i < filteredNeighbors.size(); i++)
		{
			_cprintf("%d ", filteredNeighbors[i]->getId());
		}
		_cprintf("\n");
	}
	//*/
}

void Movement::writeToTexture(GLfloat *bvh_tex, const int & escIdxFromParentToRightChild)
{
	//visit
	//_cprintf("move id: %d, escape index: %d \n", m_id, m_id + 1 );

	Vector3 beginMove = begin;
	Vector3 endMove = end;

	// Move toolpath to center of tool
	beginMove += m_begin_axis * radius;
	endMove += m_end_axis * radius;

	// Calculate the direction vector
	Vector3 moveVec = endMove - beginMove;
	float length = ~moveVec;
	if (beginMove != endMove) moveVec.Normalize();

	bvh_tex[m_id*8] = beginMove.x();
	bvh_tex[m_id*8+1] = beginMove.y();
	bvh_tex[m_id*8+2] = beginMove.z();
	bvh_tex[m_id*8+3] = 0.0f;//type 1 for bvh

	bvh_tex[m_id*8+4] = moveVec.x();
	bvh_tex[m_id*8+5] = moveVec.y();
	bvh_tex[m_id*8+6] = moveVec.z();
	bvh_tex[m_id*8+7] = length;

	std::cout << escIdxFromParentToRightChild;

	//end visit. Since this is a leaf node, nothing left to do.
}

void Movement::textureIndexing(int & index, Shape * _parent, Shape* root)
{
	/*
	_cprintf("move id: %d, (%f, %f, %f), (%f, %f, %f) \n", 
	index, begin.x(), begin.y(), begin.z(), end.x(), end.y(), end.z());
	//*/

	m_id = index;
	index++;

	parent = _parent;
	m_root = root;
}




Shape * Movement::get3LayerAncestor()
{
	//parent is guaranteed to be not null
	Shape * grantParent = parent->getParent();
	if (grantParent != NULL)
	{
		Shape * grantGrantParent = grantParent->getParent();
		if (grantGrantParent != NULL)
		{
			return grantGrantParent;
		} 
		else
		{
			return grantParent;
		}
	} 
	else
	{
		return parent;
	}
}



Shape * Movement::get4LayerAncestor()
{
	//parent is guaranteed to be not null
	Shape * grantParent = parent->getParent();
	if (grantParent != NULL)
	{
		Shape * grantGrantParent = grantParent->getParent();
		if (grantGrantParent != NULL)
		{
			Shape * ancestor = grantGrantParent->getParent();
			if (ancestor != NULL)
			{
				return ancestor;
			} 
			else
			{
				return grantGrantParent;
			}
		} 
		else
		{
			return grantParent;
		}
	} 
	else
	{
		return parent;
	}
}




Shape * Movement::get5LayerAncestor()
{
	//parent is guaranteed to be not null
	Shape * grantParent = parent->getParent();
	if (grantParent != NULL)
	{
		Shape * grantGrantParent = grantParent->getParent();
		if (grantGrantParent != NULL)
		{
			Shape * ancestor = grantGrantParent->getParent();
			if (ancestor != NULL)
			{
				Shape * ancestorParent = ancestor->getParent();
				if (ancestorParent != NULL)
				{
					return ancestorParent;
				} 
				else
				{
					return ancestor;
				}
			} 
			else
			{
				return grantGrantParent;
			}
		} 
		else
		{
			return grantParent;
		}
	} 
	else
	{
		return parent;
	}
}




Shape * Movement::get6LayerAncestor()
{
	//parent is guaranteed to be not null
	Shape * grantParent = parent->getParent();
	if (grantParent != NULL)
	{
		Shape * grantGrantParent = grantParent->getParent();
		if (grantGrantParent != NULL)
		{
			Shape * ancestor = grantGrantParent->getParent();
			if (ancestor != NULL)
			{
				Shape * ancestorParent = ancestor->getParent();
				if (ancestorParent != NULL)
				{
					Shape * ancestorGrantParent = ancestorParent->getParent();
					if (ancestorGrantParent != NULL)
					{
						return ancestorGrantParent;
					} 
					else
					{
						return ancestorParent;
					}
				} 
				else
				{
					return ancestor;
				}
			} 
			else
			{
				return grantGrantParent;
			}
		} 
		else
		{
			return grantParent;
		}
	} 
	else
	{
		return parent;
	}
}



void Movement::initRelativeList(std::vector<Shape*> &relativeList, Shape *_ancestor)
{
	_ancestor->getRelativeList(relativeList, m_id);
}



void Movement::getRelativeList(std::vector<Shape*> &relativeList, int queryingNodeId)
{
	if (queryingNodeId != m_id)
	{
		relativeList.push_back(this);
	}
}




void Movement::neighborhoodFilter(const std::vector<Shape*> &inList, std::vector<Shape*> &outList, size_t k)
{
	for (size_t i = 0; i < inList.size(); i++)
	{
		Movement* neighbor = static_cast<Movement*>(inList[i]);
		if ( neighbor->originalId != (this->originalId - 1) && neighbor->originalId != (this->originalId + 1)
			&& neighbor->originalId != (this->originalId - 2) && neighbor->originalId != (this->originalId + 2) )
		{
			outList.push_back(inList[i]);
		}
	}

	if (outList.size() <= k) return;

	//sort the out list according to squared distance using partial sort for k-nearest neighbors
	partial_sort(outList.begin(), outList.begin()+k, outList.end(), CompareMove(this));

}



void Movement::neighborhoodFilterRedundant(const std::vector<Shape*> &inList, std::vector<Shape*> &outList, size_t k)
{
	std::vector<Shape*> redundantList;
	//size_t topN = 4;

	for (size_t i = 0; i < inList.size(); i++)
	{
		Movement* neighbor = static_cast<Movement*>(inList[i]);
		if ( neighbor->originalId != (this->originalId - 1) && neighbor->originalId != (this->originalId + 1)
			&& neighbor->originalId != (this->originalId - 2) && neighbor->originalId != (this->originalId + 2) )
		{
			redundantList.push_back(inList[i]);
		}
	}



	
	//no remove topN
	for (size_t i = 0; i < redundantList.size(); i++)
	{
		outList.push_back(redundantList[i]);
	}
	if (outList.size() <= k) return;
	partial_sort(outList.begin(), outList.begin()+k, outList.end(), CompareMove(this));
	return;
	//end of no remove topN

	//warning: unreachable code
	/*

	if (redundantList.size() <= topN)
	{
		for (size_t i = 0; i < redundantList.size(); i++)
		{
			outList.push_back(redundantList[i]);
		}
		return;
	} 
	else
	{
		//get rank 3-4
		partial_sort(redundantList.begin(), redundantList.begin()+topN, redundantList.end(), CompareMove(this));
		//sorting by computing distance between line segments gives worse result
		//partial_sort(redundantList.begin(), redundantList.begin()+topN, redundantList.end(), CompareMoveByLineSegment(this));
		Movement* top1 = static_cast<Movement*>(redundantList[0]);
		Movement* top2 = static_cast<Movement*>(redundantList[1]);
		Movement* top3 = static_cast<Movement*>(redundantList[2]);
		Movement* top4 = static_cast<Movement*>(redundantList[3]);

		for (size_t i = 0; i < redundantList.size(); i++)
		{
			Movement* neighbor = static_cast<Movement*>(redundantList[i]);
			if ( neighbor->originalId != (top1->originalId - 1) && neighbor->originalId != (top1->originalId + 1)
				&& neighbor->originalId != (top2->originalId - 1) && neighbor->originalId != (top2->originalId + 1)
				&& neighbor->originalId != (top3->originalId - 1) && neighbor->originalId != (top3->originalId + 1)
				&& neighbor->originalId != (top4->originalId - 1) && neighbor->originalId != (top4->originalId + 1) )
			{
				outList.push_back(redundantList[i]);
			}
		}

		if (outList.size() <= k) return;
		//sort the out list according to squared distance using partial sort for k-nearest neighbors
		partial_sort(outList.begin(), outList.begin()+k, outList.end(), CompareMove(this));
		//partial_sort(outList.begin(), outList.begin()+k, outList.end(), CompareMoveByLineSegment(this));
	}

	//*/

}



void Movement::buildKNearestNeighborsMap(KnnMapType & knnMap, const size_t & k, const size_t & layers)
{
	std::vector<Shape*> filteredNeighbors;
	Shape* ancestor;
	//is4Layers ? ancestor = get4LayerAncestor() : ancestor = get3LayerAncestor();
	
	switch (layers)
	{
	case 3:
		ancestor = get3LayerAncestor();
		break;
	case 4:
		ancestor = get4LayerAncestor();
		break;
	case 5:
		ancestor = get5LayerAncestor();
		break;
	case 6:
		ancestor = get6LayerAncestor();
		break;
	default:
		ancestor = get6LayerAncestor();
	}
	
	initRelativeList(filteredNeighbors, ancestor);
	if (filteredNeighbors.size() > k) 
		partial_sort(filteredNeighbors.begin(), filteredNeighbors.begin()+k, filteredNeighbors.end(), CompareMoveSegment(this));

	//std::vector<Shape*> filteredNeighbors;
	//neighborhoodFilterRedundant(neighbors, filteredNeighbors, k);

	std::vector<NeighborItem> knn(filteredNeighbors.size());

	//_cprintf("neighbors of the move id %d is \n", originalId);
	for (size_t i = 0; i < filteredNeighbors.size(); i++)
	{
		Movement * move = static_cast<Movement *>(filteredNeighbors[i]);
		//_cprintf("%d ", move->getMoveId());
		NeighborItem item;
		item.id = move->getMoveId();
		item.ranking = move->getRanking();
		knn[i] = item;
	}
	//_cprintf("\n");
	knnMap[originalId] = knn;
}







void Movement::allNNExact(std::vector<Shape*> &neighborList, size_t numberOfNeighbors)
{
	CompareLineSegmentWithBox::Segment moveSegment;
	moveSegment.P0 = this->getBegin();
	moveSegment.P1 = this->getEnd();

	CompareLineSegmentWithBox lineBoxComparer(moveSegment);
	CompareMoveByLineSegment lineLineComparer(moveSegment);

	//the closest one is at the top
	priority_queue<Shape*, std::vector<Shape*>, CompareLineSegmentWithBox> checkList( (lineBoxComparer) );
	float nodePriority = 1000000.0f;
	//the furthest one is at the top
	priority_queue<Shape*, std::vector<Shape*>, CompareMoveByLineSegment> neighbors( (lineLineComparer) );

	float smallestDistanceFoundSoFar = 1000000.0f;
	Shape* currentNode = m_root;

	do 
	{
		currentNode->bvh_knn(neighbors, checkList, numberOfNeighbors, lineBoxComparer, lineLineComparer, this->originalId);
		
		currentNode = checkList.top();
		nodePriority = currentNode->getRanking();
		
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





void Movement::bvh_knn(priority_queue<Shape*, std::vector<Shape*>, CompareMoveByLineSegment> &neighborList, 
			 priority_queue<Shape*, std::vector<Shape*>, CompareLineSegmentWithBox> & checkList, 
			 const size_t & numberOfNeighbors, CompareLineSegmentWithBox & lineBoxComparer, 
			 CompareMoveByLineSegment & lineLineComparer, const size_t & queryingMoveId)
{

	//if ( queryingMoveId != this->originalId && 
	//	(queryingMoveId + 1) != this->originalId && (queryingMoveId - 1) != this->originalId &&
	//	(queryingMoveId + 2) != this->originalId && (queryingMoveId - 2) != this->originalId )
	//{
	//	
	//}

	//if ( queryingMoveId == this->originalId || 
	//	(queryingMoveId + 1) == this->originalId || (queryingMoveId - 1) == this->originalId ||
	//	(queryingMoveId + 2) == this->originalId || (queryingMoveId - 2) == this->originalId )
	//{
	//	return;
	//}



	if ( queryingMoveId == this->originalId )
	{
		return;
	}



	float distance = lineLineComparer.getDistance(this);
	this->setRanking(distance);

	if (neighborList.empty())
	{
		neighborList.push(this);
		return;
	}
	else
	{
		Shape* kth_neighbor = neighborList.top();

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

	//_cprintf("move distance %f, kth distance %f neighborList size %d \n", distance, neighborList.top(), neighborList.size());
}




void Movement::buildKnnExactMap(KnnMapType & knnMap, const size_t & k)
{
	std::vector<Shape*> neighbors;

	allNNExact(neighbors, k);

	std::vector<NeighborItem> knn(neighbors.size());

	//_cprintf("neighbors of the move id %d is \n", originalId);
	for (size_t i = 0; i < neighbors.size(); ++i)
	{
		Movement * move = static_cast<Movement *>(neighbors[i]);
		//_cprintf("%d ", move->getMoveId());
		NeighborItem item;
		item.id = move->getMoveId();
		item.ranking = move->getRanking();
		knn[i] = item;
	}
	//_cprintf("\n");
	knnMap[originalId] = knn;
}