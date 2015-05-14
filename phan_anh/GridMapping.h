#pragma once


#include <GL/glew.h>
using namespace std;



class GridMapping
{
public:
	GridMapping(void);
	~GridMapping(void);

	static const int DEPTH = 100;
	static const int HEIGHT = 300;
	static const int WIDTH = 200;
	//! 2d texture storing 3d grid
	//! HIST_TEXTURE_SIZE = sqrt(DEPTH*HEIGHT*WIDTH)
	static const int HIST_TEXTURE_SIZE = 2500;
	//! store all of local lists containing influencing tool paths
	//! a local list is a cluster of tool paths intersecting a cell
	//! they can overlap within 2 neighboring cells
	static const int MOVE_LIST_TEXTURE_SIZE = 6000;
	//! store global list of tool paths
	static const int GLOBAL_MOVE = 1200;



	typedef 3dBoundingBox<float> BoxType;
	typedef TPoint3d<float> float3d;

	void setBbox(const 3dBoundingBox<float> & aBox)
	{
		bbox = aBox;
	}

	const 3dBoundingBox<float> & getBbox()
	{
		return bbox;
	}

	void histogram_voting(GLfloat* beginList, GLfloat* endList
						  , GLfloat* radius, size_t length);


	void build_histogram_texture();



protected:

	BoxType bbox;

	//! each 3d cell store a list of potential tool paths which can touch this cell
	std::vector< std::vector<GLuint> > tool_path_3d_histogram;

	int GridMapping::get_cell_id(float3d corner, int & xId, int & yId, int & zId);


};

