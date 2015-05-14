#ifndef _KD_TREE_H_
#define _KD_TREE_H_



#include <vector>
#include "MoveKNN.h"
#include <queue>
#include <map>
//#include <tr1/unordered_map>
#include <climits>


//! KD tree node
class KDTreeNode {

public:

    typedef MoveKNN* ItemPtr;
    typedef std::vector<ItemPtr> ItemPtrVec;

    //! Default constructor
    KDTreeNode();

    //! Destructor
    ~KDTreeNode();

    //! Vector of contained edges
    ItemPtrVec m_items;

    //! Pointers to child nodes
    KDTreeNode * mp_child[ 2 ];


    //! Split axis of this node
    int m_splitAxis;

    //! Split value of this node
    float m_splitValue;
};



//! Binary space partition tree
class KDTree {

public:

    typedef MoveKNN * ItemPtr;
    typedef std::vector< ItemPtr > ItemPtrVec;
	typedef cadcam::mwTPoint3d<float> Vector3;
	typedef std::priority_queue<MoveKNN*, std::vector<MoveKNN*>, CompareMoveSegmentKdTree> KnnQueueType;
	//typedef std::tr1::unordered_map< size_t, std :: vector<size_t> > MapType;
	typedef std::map< size_t, std :: vector<size_t> > MapType;

    //! Default constructor
    KDTree( int _maxDepth = 36 );

    //! Destructor
    ~KDTree();

    //! Set items
    void setItems( const ItemPtrVec & _items );



	//! #nodes in this tree
	int GetNrNodes() { return m_numNodes; }

	void buildKdtreeKnnMap(MapType &knnMap, const size_t & k);

protected:

	struct SplitStatistics 
	{
		float splitValue;
		int noIncidents;
		int noLeft;
		int noRight;
		int splitAxis;
	};
	

    //! Recursively generate a new node of tree
    KDTreeNode * generateNewNode( int _level, const ItemPtrVec & _items, const Vector3 & _bbMin, const Vector3 & _bbMax );

    //! Recursive iteration over the KD tree to find the items closest to given item
    void iterateTree( const KDTreeNode* _node, MoveKNN * _item, KnnQueueType& _itemPtrVec, const size_t & _noNeighbors );

	//! Get vector of pointers to closest items to given item
	void closestItems( MoveKNN * _queryItem, ItemPtrVec & _itemPtr, const size_t & _noNeighbors );

    //! Compute bounding box of given items
    void boundingBox( const ItemPtrVec & _items, Vector3 & _bbMin, Vector3 & _bbMax );

	void buildKdtreeKnnMap(MapType &knnMap, const size_t & k, ItemPtr _item);

	void buildKdtreeKnnMap(const KDTreeNode* _node, MapType &knnMap, const size_t & k);

	SplitStatistics computeSplitStatistics(int splitAxis, const ItemPtrVec & _items);

	SplitStatistics getSplitAxis(const ItemPtrVec & _items);

    //! Root node of tree
    KDTreeNode * mp_rootNode;

    //! Maximal depth of this tree
    int m_maxDepth;    

    //! Number of generated nodes
    int m_numNodes;
};



#endif // _KD_TREE_H_
