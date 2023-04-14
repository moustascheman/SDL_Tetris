#include "PieceInstance.h"


PieceInstance::PieceInstance(pieceType selectedType) {
	type = selectedType;
}

void PieceInstance::SpawnNewPiece(pieceType selectedType) {
	type = selectedType;
	orientation = 0;
	switch (type) {
		case I:
		{
			for (int i = 0; i < 4; i++) {
				blocks[i].col = CYAN;
				blocks[i].y = 20;
				blocks[i].x = 3 + i;
			}
			pivotIndex = 1;
			break;
		}

		case J:
		{
			blocks[0].col = BLUE;
			blocks[0].x = 4;
			blocks[0].y = 20;
			for (int i = 1; i < 4; i++) {
				blocks[i].col = BLUE;
				blocks[i].y = 19;
				blocks[i].x = 3 + i;
			}
			pivotIndex = 2;
			break;
		}

		case L:
		{
			blocks[3].col = ORANGE;
			blocks[3].y = 20;
			blocks[3].x = 6;
			for (int i = 0; i < 3; i++) {
				blocks[i].col = ORANGE;
				blocks[i].y = 19;
				blocks[i].x = 4 + i;
			}
			pivotIndex = 1;
			break;
		}

		case O:
		{
			for (int i = 0; i < 4; i++) {
				blocks[i].col = YELLOW;
			}
			blocks[0].x = 4;
			blocks[0].y = 20;
			blocks[1].x = 5;
			blocks[1].y = 20;
			blocks[2].x = 4;
			blocks[2].y = 19;
			blocks[3].x = 5;
			blocks[3].y = 19;
			pivotIndex = 0;
			break;
		}

		case S:
		{
			for (int i = 0; i < 4; i++) {
				blocks[i].col = GREEN;
			}

			blocks[0].x = 4;
			blocks[0].y = 19;
			blocks[1].x = 5;
			blocks[1].y = 19;
			blocks[2].x = 5;
			blocks[2].y = 20;
			blocks[3].x = 6;
			blocks[3].y = 20;
			pivotIndex = 1;
			break;
		}

		case T:
		{
			for (int i = 0; i < 4; i++) {
				blocks[i].col = PURPLE;
			}
			blocks[0].x = 4;
			blocks[0].y = 19;
			blocks[1].x = 5;
			blocks[1].y = 19;
			blocks[2].x = 5;
			blocks[2].y = 20;
			blocks[3].x = 6;
			blocks[3].y = 19;
			pivotIndex = 1;
			break;
		}

		case Z:
		{
			for (int i = 0; i < 4; i++) {
				blocks[i].col = RED;
			}
			blocks[0].x = 4;
			blocks[0].y = 20;
			blocks[1].x = 5;
			blocks[1].y = 20;
			blocks[2].x = 5;
			blocks[2].y = 19;
			blocks[3].x = 6;
			blocks[3].y = 19;
			pivotIndex = 2;
			break;
		}
	}
}

void PieceInstance::RotateClockwise() {
	int pivotX = blocks[pivotIndex].x;
	int pivotY = blocks[pivotIndex].y;

	

		//I is weird. You need to recalculate the pivot based on its current orientation and position since it doesn't pivot based on a block
		if(type == I)
		{
			switch (orientation) {
				case 0: 
				{
					pivotX = blocks[1].x;
					pivotY = blocks[1].y - 1;
					break;
				}

				case 1: 
				{
					pivotX = blocks[1].x - 1;
					pivotY = blocks[1].y;
					break;
				}

				case 2: 
				{
					pivotX = blocks[1].x;
					pivotY = blocks[1].y + 1;
					break;
				}

				case 3: 
				{
					pivotX = blocks[1].x + 1;
					pivotY = blocks[1].y;
					break;

				}
			}
		}
		// O cannot be rotated
		if (type != O) {
			for (int i = 0; i < 4; i++) {
				if (blocks[i].x != pivotX || blocks[i].y != pivotY) {
					int originX = blocks[i].x - pivotX;
					int originY = blocks[i].y - pivotY;

					int tempX = originY;
					int tempY = -1 * originX;

					blocks[i].x = tempX + pivotX;
					blocks[i].y = tempY + pivotY;
				}
			}
			//increment orientation, reset it to 0 if it's going above 3
			orientation = (orientation + 1 >= 4) ? 0 : orientation + 1;
		}
	


}