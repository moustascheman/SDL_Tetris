#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <queue>
#include <random>
#include <chrono>
#include <array>
#include "PieceInstance.h"



/*
struct SelectedPiece {
	pieceType type;
	std::array<placeBlock, 4> blocks;
};*/

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int blockDimension = 24 * (SCREEN_HEIGHT / 600);

const int boardTopX = 280;
const int boardTopY = 48;

const int boardW = 240;
const int boardH = 504;

const int boardOutlinePx = 5;

class Game
{
public:
	Game() {}
	~Game() {}
	bool Initialize();
	const int VANISH_ZONE_LINE = 21;
	void runGame();
	void clean();
	

private:
	SDL_Rect blockSprites[7];
	SDL_Window* gWindow;
	SDL_Texture* blockTexture;
	SDL_Renderer* gRenderer;
	std::vector<std::vector<block>> board;
	std::queue<pieceType> nextQueue;
	SDL_Rect boardRect = { boardTopX, boardTopY, boardW, boardH };
	SDL_Rect boardBorder = { boardTopX - 5, boardTopY - 5, boardW + 10, boardH + 10 };
	void FillPieceQueue();
	void Gravity();
	int currentPiece = NULL;
	PieceInstance selection = PieceInstance();
	void DrawSelection();
	void shift(int amount);
	bool SetupBlockSprites();
	bool handleInput(SDL_Event &e);
};




