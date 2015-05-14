#include "MoveListPruner.h"
#include <conio.h>

MoveListPruner::MoveListPruner(void)
{
}

MoveListPruner::~MoveListPruner(void)
{
}

void MoveListPruner::
build_histogram_texture(const mwMachSimVerifier::MoveSimulationList & simulationMoveList)
{
	GLuint *hist = new GLuint[HIST_TEXTURE_SIZE*HIST_TEXTURE_SIZE*4];
	GLuint *cell_paths = new GLuint[MOVE_LIST_TEXTURE_SIZE*MOVE_LIST_TEXTURE_SIZE*4];
	memset (cell_paths, 0, MOVE_LIST_TEXTURE_SIZE*MOVE_LIST_TEXTURE_SIZE*4);
	memset (hist, 0, HIST_TEXTURE_SIZE*HIST_TEXTURE_SIZE*4);

	int count = 0;

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

				//this should be used in the shader to access hist_texture
				//int hist_tex_height = floor(index/HIST_TEXTURE_SIZE);
				//int hist_tex_width = index%HIST_TEXTURE_SIZE;
				//4 channels per element
				hist[index*4] = begin_width;
				hist[index*4+1] = begin_height;
				hist[index*4+2] = begin_channel;
				hist[index*4+3] = (GLuint)length;

				pruneMovesInCell(cell_paths, count, related_moves, simulationMoveList);

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

bool MoveListPruner::isRapid(const Move & move)
{
	float3d dist = move.end - move.begin;
	float length = dist.Length2();
	//_cprintf("length squared: %f \n", length );
	return length > RAPID_SQUARED_LENGTH;
}


bool MoveListPruner::isRapid(float3d begin, float3d end)
{
	float3d dist = end - begin;
	float length = dist.Length2();
	//_cprintf("length squared: %f \n", length );
	return length > RAPID_SQUARED_LENGTH;
}


void MoveListPruner::pruneMovesInCell(GLuint * cell_paths, int & count, 
									  const std::vector<GLuint> & related_moves, 
									  const mwMachSimVerifier::MoveSimulationList & simulationMoveList)
{
	std::vector<Move> localList;
	for (int i = 0; i < related_moves.size(); i++)
	{
		Move move;
		move.moveId = related_moves[i];
		move.begin = simulationMoveList[move.moveId].beginFrame.m_origin;
		move.end = simulationMoveList[move.moveId].endFrame.m_origin;

		if (!isRapid(move))
		{
			localList.push_back(move);
		}
	}

	if (localList.size() > MAX_NO_MOVES_CELL)
	{
		partial_sort(localList.begin(), localList.begin()+MAX_NO_MOVES_CELL, localList.end(),Compare());
	} 

	size_t k = MAX_NO_MOVES_CELL < localList.size() ? MAX_NO_MOVES_CELL : localList.size();
	for (int i = 0; i < k; i++)
	{
		cell_paths[count] = localList[i].moveId;
		count++;
	}
}