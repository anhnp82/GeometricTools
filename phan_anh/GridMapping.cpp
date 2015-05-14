//#include "StdAfx.h"
#include "GridMapping.h"
#include <iostream>
#include <fstream>

GridMapping::GridMapping(void)
{
	//tool_path_3d_histogram.resize(DEPTH*HEIGHT*WIDTH);
}

GridMapping::~GridMapping(void)
{

}



void GridMapping::build_histogram_texture()
{
	GLuint *hist = new GLuint[HIST_TEXTURE_SIZE*HIST_TEXTURE_SIZE*4];
	GLuint *cell_paths = new GLuint[MOVE_LIST_TEXTURE_SIZE*MOVE_LIST_TEXTURE_SIZE*4];
	memset (cell_paths, 0, MOVE_LIST_TEXTURE_SIZE*MOVE_LIST_TEXTURE_SIZE*4);
	memset (hist, 0, HIST_TEXTURE_SIZE*HIST_TEXTURE_SIZE*4);

	int count = 0;
	size_t maxNoMoves = 0;

	for (int d = 0; d < DEPTH; d++)
	{
		for (int h = 0; h < HEIGHT; h++)
		{
			for (int w = 0; w < WIDTH; w++)
			{
				int index = d*HEIGHT*WIDTH + h*WIDTH + w;

				std::vector<GLuint> related_moves = tool_path_3d_histogram[index];
				
				//reshape the texture
				int sequential_tex = (count/4);
				int begin_height = (sequential_tex/MOVE_LIST_TEXTURE_SIZE);
				int begin_width = sequential_tex%MOVE_LIST_TEXTURE_SIZE;
				int begin_channel = count%4;
				size_t length = related_moves.size();

				if (maxNoMoves < length) maxNoMoves = length;

				//this should be used in the shader to access hist_texture
				//int hist_tex_height = floor(index/HIST_TEXTURE_SIZE);
				//int hist_tex_width = index%HIST_TEXTURE_SIZE;
				//4 channels per element
				hist[index*4] = begin_width;
				hist[index*4+1] = begin_height;
				hist[index*4+2] = begin_channel;
				hist[index*4+3] = (GLuint) length;

				for (int i = 0; i < related_moves.size(); i++)
				{
					cell_paths[count] = related_moves[i];
					count++;
				}
			}
		}
	}

	GLuint histogram_tex;
	glGenTextures(1, &histogram_tex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, histogram_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI
		, HIST_TEXTURE_SIZE, HIST_TEXTURE_SIZE
		, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, hist);

	GLuint move_tex;
	glGenTextures(1, &move_tex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, move_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI
		, MOVE_LIST_TEXTURE_SIZE, MOVE_LIST_TEXTURE_SIZE
		, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, cell_paths);

	delete [] cell_paths;
	delete [] hist;
	tool_path_3d_histogram.clear();
}








int GridMapping::get_cell_id(float3d corner, int & depth_id, int & height_id, int & width_id)
{
	depth_id = static_cast<int>(floor(DEPTH*(corner[2] - bbox.GetMinZ())/bbox.GetDepth()));
	height_id = static_cast<int>(floor(HEIGHT*(corner[1] - bbox.GetMinY())/bbox.GetHeight()));
	width_id = static_cast<int>(floor(WIDTH*(corner[0] - bbox.GetMinX())/bbox.GetWidth()));

	//tool path could be outside of the stock bounding box
	//truncate the index to the nearest bound

	if (depth_id < 0) depth_id = 0;
	if (height_id < 0) height_id = 0;
	if (width_id < 0) width_id = 0;

	if (depth_id >= DEPTH) depth_id = DEPTH - 1;
	if (height_id >= HEIGHT) height_id = HEIGHT - 1;
	if (width_id >= WIDTH) width_id = WIDTH - 1;

	return depth_id*HEIGHT*WIDTH + height_id*WIDTH + width_id;
}




void GridMapping::histogram_voting(GLfloat* beginList, GLfloat* endList
								   , GLfloat* radius, size_t length)
{
	size_t max = 0;
	for (int i = 0; i < length; i++)
	{
		BoxType path_bbox( float3d(beginList[3*i], beginList[3*i+1], beginList[3*i+2])
						  , float3d(endList[3*i], endList[3*i+1], endList[3*i+2]) );

		path_bbox.Inflate(radius[i]);

		int start_d, start_h, start_w, end_d, end_h, end_w;

		get_cell_id(path_bbox.GetBottomLeftNearCorner(), start_d, start_h, start_w);
		get_cell_id(path_bbox.GetUpperRightFarCorner(), end_d, end_h, end_w);

		for (int z = start_d; z <= end_d; z++)
		{
			for (int y = start_h; y <= end_h; y++)
			{
				for (int x = start_w; x <= end_w; x++)
				{
					tool_path_3d_histogram[z*HEIGHT*WIDTH + y*WIDTH + x].push_back(i);
					if (max < tool_path_3d_histogram[z*HEIGHT*WIDTH + y*WIDTH + x].size())
					{
						max = tool_path_3d_histogram[z*HEIGHT*WIDTH + y*WIDTH + x].size();
					}
				}
			}
		}
	}
}




