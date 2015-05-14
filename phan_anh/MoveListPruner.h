#pragma once

#include "GridMapping.h"

class MoveListPruner : public GridMapping
{
public:

	static const int MAX_NO_MOVES_CELL = 200;
	static const int RAPID_SQUARED_LENGTH = 100;

	MoveListPruner(void);
	~MoveListPruner(void);

	static bool isRapid(float3d begin, float3d end);

	void build_histogram_texture(const mwMachSimVerifier::MoveSimulationList & simulationMoveList);

	void pruneMovesInCell(GLuint * cell_paths, int & count, 
		const std::vector<GLuint> & related_moves, 
		const mwMachSimVerifier::MoveSimulationList & simulationMoveList);

private:

	struct Move 
	{
		int moveId;
		float3d begin;
		float3d end;
	};

	bool isRapid(const Move & move);

	class Compare
	{

	public:

		Compare() {}

		bool operator()(Move move1, Move move2) const
		{
			//float heigh1 = ( move1.begin.z() + move1.end.z() ) / 2.0f;
			//float heigh2 = ( move2.begin.z() + move2.end.z() ) / 2.0f;

			float heigh1 = move1.begin.z() < move1.end.z() ? move1.begin.z() : move1.end.z();
			float heigh2 = move2.begin.z() < move2.end.z() ? move2.begin.z() : move2.end.z();

			return heigh1 < heigh2;
		}

	};

};


