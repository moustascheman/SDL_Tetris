#pragma once
#include "GameEnums.h"
#include <array>

struct BlockInstance {
	int x, y;
	block col;
};

class PieceInstance
{
	public:
		pieceType type;
		std::array<BlockInstance, 4> blocks;
		PieceInstance(pieceType selectedType = I);
		~PieceInstance() {};
		int orientation = 0;
		void SpawnNewPiece(pieceType selectedType = I);
		void RotateClockwise();
	private:
		int pivotIndex = 0;

};

