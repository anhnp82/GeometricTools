#if _MSC_VER > 1310



#ifndef _KD_TREE_CC_
#define _KD_TREE_CC_

#include <iostream>
#include <assert.h>
#include <float.h>
#include <set>
#include <conio.h>



#include "kdtree.h"



KDTreeNode::KDTreeNode()
{
    for( int i = 0; i < 2; ++i )
	mp_child[ i ] = 0;
}



KDTreeNode::~KDTreeNode()
{
    for( int i = 0; i < 2; ++i )
    {
	if( mp_child[ i ] )
	    delete mp_child[ i ];
    }
}



KDTree::KDTree( int _maxDepth )
{
    m_maxDepth = _maxDepth;
    mp_rootNode = 0;
}



KDTree::~KDTree()
{
    if (mp_rootNode) delete mp_rootNode;
}



void KDTree::setItems( const ItemPtrVec & _items )
{
    if( _items.size() <= 0 )
	return;

    // delete old tree
    if( mp_rootNode )
    {
		delete mp_rootNode;
		mp_rootNode = NULL;
	}

    Vector3 bbMin, bbMax;
    boundingBox( _items, bbMin, bbMax );

    mp_rootNode = generateNewNode( 0, _items, bbMin, bbMax );
}




KDTreeNode * KDTree::generateNewNode( int _level, const ItemPtrVec & _items, const Vector3 & _bbMin, const Vector3 & _bbMax )
{
    if( _level == 0 )
		m_numNodes = 1;
    else
		++m_numNodes;

    KDTreeNode* newNode = new KDTreeNode;

	//_cprintf("item size %d, level %d \n", _items.size(), _level);

    /////////////////////////////////////////
    // abortion criteria

    if((_items.size()<=30) || (_level==m_maxDepth))
    {
		newNode->m_items = _items;
		return newNode;
    }

    /////////////////////////////////////////
    // try to shrink current bounding box

    Vector3 bbMax, bbMin;
    boundingBox( _items, bbMin, bbMax );

    for( int i = 0; i < 3; ++i )
    {
		if(bbMin[i] < _bbMin[i]) bbMin[i] = _bbMin[i];
		if(bbMax[i] > _bbMax[i]) bbMax[i] = _bbMax[i];
    }

	ItemPtrVec::const_iterator iIter, iEnd = _items.end();

//#define USE_MAX_EXTEND_DIM

#ifdef	USE_MAX_EXTEND_DIM

	///*
    /////////////////////////////////////////
    // find axis with largest interval
    
    int maxAxis = -1;
    double interval, maxInterval = -FLT_MAX;
    for( int i = 0; i < 3; ++i )
    {
		interval = bbMax[i] - bbMin[i];
		if(interval > maxInterval)
		{
	    	maxInterval = interval;
	    	maxAxis = i;
		}
    }
    assert( maxAxis != -1 );

    

    /////////////////////////////////////////
    // do the splitting
    
    std::set<float> median_search;
			
	iEnd = _items.end();
	for( iIter = _items.begin(); iIter != iEnd; ++iIter )
	{
		float value=0.0f;
		for (int i=0; i<2; ++i)
		{
			value += (**iIter).getPoint(i)[maxAxis];
		}
		value /= 2.0f;
		median_search.insert( value );
	}
							
	size_t size = median_search.size() / 2;
	std::set<float>::const_iterator it = median_search.begin();
	while(size-- != 0) it++;
	
	newNode->m_splitValue = *it;
    newNode->m_splitAxis = maxAxis;
	//*/

#else

	SplitStatistics splitStatistics = getSplitAxis(_items);
	newNode->m_splitValue = splitStatistics.splitValue;
	newNode->m_splitAxis = splitStatistics.splitAxis;

#endif

	//_cprintf("split axis %d, split value %f \n", newNode->m_splitAxis , newNode->m_splitValue);

    /////////////////////////////////////
    // generate two new sets of edges

    ItemPtrVec itemPtrVec[2];

    for (iIter=_items.begin(); iIter!=iEnd; ++iIter)
    {
		if( (*iIter)->smallerThanValue( newNode->m_splitValue, newNode->m_splitAxis ) )
			itemPtrVec[0].push_back( *iIter );
	
		if( (*iIter)->largerThanValue( newNode->m_splitValue, newNode->m_splitAxis ) )
			itemPtrVec[1].push_back( *iIter );
    }

    if( itemPtrVec[0].size() > 0 )
    {
		Vector3 newBBMax = bbMax;
		newBBMax[newNode->m_splitAxis] = newNode->m_splitValue;

		newNode->mp_child[0] = generateNewNode(_level+1, itemPtrVec[0], bbMin, newBBMax);
    }

    if( itemPtrVec[1].size() > 0 )
    {
		Vector3 newBBMin = bbMin;
		newBBMin[newNode->m_splitAxis] = newNode->m_splitValue;

		newNode->mp_child[1] = generateNewNode(_level+1, itemPtrVec[1], newBBMin, bbMax);
    }

    return newNode;
}




KDTree::SplitStatistics KDTree::computeSplitStatistics(int splitAxis, const ItemPtrVec & _items)
{

	SplitStatistics statistics;
	statistics.splitAxis = splitAxis;

	std::set<float> median_search;
	for( size_t i = 0; i < _items.size(); ++i )
	{
		float value=0.0f;
		for (int i=0; i<2; ++i)
		{
			value += _items[i]->getPoint(i)[splitAxis];
		}
		value /= 2.0f;
		median_search.insert( value );
	}

	size_t size = median_search.size() / 2;
	std::set<float>::const_iterator it = median_search.begin();
	while(size-- != 0) it++;
	statistics.splitValue = *it;

	statistics.noIncidents = 0;
	statistics.noLeft = 0;
	statistics.noRight = 0;
	bool isSmaller, isLarger;
	for (size_t i = 0; i < _items.size(); ++i)
	{
		isSmaller = _items[i]->smallerThanValue(statistics.splitValue, splitAxis);
		isLarger = _items[i]->largerThanValue(statistics.splitValue, splitAxis);
		if (isLarger && isSmaller)
		{
			statistics.noIncidents++;
		}
		if (isLarger)
		{
			statistics.noRight++;
		}
		if (isSmaller)
		{
			statistics.noLeft++;
		}
	}
	return statistics;
}


KDTree::SplitStatistics KDTree::getSplitAxis(const ItemPtrVec & _items)
{
	SplitStatistics statistics[3];
	statistics[0] = computeSplitStatistics(0, _items);
	statistics[1] = computeSplitStatistics(1, _items);
	statistics[2] = computeSplitStatistics(2, _items);

	int maxRank = INT_MIN;
	int maxIndex = 0;
	for (int i = 0; i < 3; i++)
	{
		//int ranking = statistics[i].noLeft + statistics[i].noRight - 2*statistics[i].noIncidents;
		int minSide;
		statistics[i].noLeft < statistics[i].noRight ? minSide = statistics[i].noLeft : minSide = statistics[i].noRight;
		int ranking = minSide - 2*statistics[i].noIncidents;//first time for the duplicate, second for the real ranking
		if (ranking > maxRank)
		{
			maxRank = ranking;
			maxIndex = i;
		}
	}
	return statistics[maxIndex];
}



void KDTree::boundingBox( const ItemPtrVec & _items, Vector3 & _bbMin, Vector3 & _bbMax )
{

	MoveKNN::BBox bbox = _items[0]->getBBox(); 
	for (int i = 1; i < _items.size(); i++) 
		bbox += _items[i]->getBBox(); 

	_bbMax = bbox.GetUpperRightFarCorner();
	_bbMin = bbox.GetBottomLeftNearCorner();

}



void KDTree::closestItems( MoveKNN * _item, ItemPtrVec & _itemPtrVec, const size_t & _noNeighbors )
{
    if( !mp_rootNode ) return;
 
	//the furthest one is at the top
	KnnQueueType neighbors( (CompareMoveSegmentKdTree(_item)) );

    iterateTree( mp_rootNode, _item, neighbors, _noNeighbors );

	_itemPtrVec.resize(_noNeighbors);
	for (size_t i = 0; i < _noNeighbors; ++i)
	{
		//_itemPtrVec.push_back(neighbors.top());
		_itemPtrVec[_noNeighbors - i - 1] = neighbors.top();
		neighbors.pop();
	}
}


void KDTree::buildKdtreeKnnMap(MapType &knnMap, const size_t & k)
{
	buildKdtreeKnnMap(mp_rootNode, knnMap, k);
}


void KDTree::buildKdtreeKnnMap(const KDTreeNode* _node, MapType &knnMap, const size_t & k)
{

	if( _node == 0 )
		return;

	//traverse in pre-order
	if( _node->m_items.size() > 0 )
	{
		for (size_t i = 0; i < _node->m_items.size(); ++i)
		{
			buildKdtreeKnnMap(knnMap, k, _node->m_items[i]);
		}
	}else
	{
		buildKdtreeKnnMap(_node->mp_child[0], knnMap, k);
		buildKdtreeKnnMap(_node->mp_child[1], knnMap, k);
	}
}



void KDTree::buildKdtreeKnnMap(MapType &knnMap, const size_t & k, ItemPtr _item)
{
	std::vector<MoveKNN*> neighbors;

	closestItems(_item, neighbors, k);

	std::vector<size_t> knn(k);

	//_cprintf("neighbors of the move id %d is \n", _item->getId());
	for (size_t i = 0; i < k; i++)
	{
		//_cprintf("%d ", neighbors[i]->getId());
		knn[i] = neighbors[i]->getId();
	}
	//_cprintf("\n");
	knnMap[_item->getId()] = knn;
}



void KDTree::iterateTree( const KDTreeNode * _node, MoveKNN * _item, KnnQueueType & _itemPtrVec, const size_t & _noNeighbors )
{
    float curMinDist = FLT_MAX;

    if( _node == 0 )
	return;

    if( _node->m_items.size() > 0 )
    {
		// this is a leaf node, compare edges with query point
		float dist;
		for( size_t i = 0; i < _node->m_items.size(); ++i )
		{

			if ( _item->getId() == _node->m_items[i]->getId() )
			{
				continue;
			}

			dist = _node->m_items[i]->distToItem( _item );
			_node->m_items[i]->setRanking(dist);

			if (_itemPtrVec.empty())
			{
				_itemPtrVec.push(_node->m_items[i]);
			}
			else
			{
				MoveKNN* kth_neighbor = _itemPtrVec.top();

				if (dist < kth_neighbor->getRanking() || _itemPtrVec.size() < _noNeighbors)
				{
					_itemPtrVec.push(_node->m_items[i]);
				}
			}

			//only keep numberOfNeighbors
			while (_itemPtrVec.size() > _noNeighbors)
			{
				_itemPtrVec.pop();
			}

		}
    }
    else
    {
		// this is not a leaf node, descend to deeper level
		int searchIndex = 0;
		bool searchBoth = false;

		if( _item->getPoint(0)[ _node->m_splitAxis ] <= _node->m_splitValue && 
			_item->getPoint(1)[ _node->m_splitAxis ] <= _node->m_splitValue )
			searchIndex = 0;
		else if ( _item->getPoint(0)[ _node->m_splitAxis ] > _node->m_splitValue && 
			_item->getPoint(1)[ _node->m_splitAxis ] > _node->m_splitValue )
		{
			searchIndex = 1;
		}else
			searchBoth = true;
			

		iterateTree( _node->mp_child[ searchIndex ], _item, _itemPtrVec, _noNeighbors );
		if (_itemPtrVec.size() > 0)
		{
			MoveKNN* kth_neighbor = _itemPtrVec.top();
			curMinDist = kth_neighbor->getRanking();
		}

		// check if we are close to the split-edge
		if( fabs( _node->m_splitValue - _item->getPoint(0)[ _node->m_splitAxis ] ) <= curMinDist ||
			fabs( _node->m_splitValue - _item->getPoint(1)[ _node->m_splitAxis ] ) <= curMinDist ||
			searchBoth)
		{
			// search the other side of the split-edge too
			iterateTree( _node->mp_child[ 1 - searchIndex ], _item, _itemPtrVec, _noNeighbors );				   
		}
    }

    //return curMinDist;
}



#endif // _KD_TREE_CC_
#endif // _MSC_VER >1310