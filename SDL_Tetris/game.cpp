#include "game.h"
#include <iostream>




bool Game::Initialize() {

	bool success = true;
	//SDL Setup

	

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	else {
		gWindow = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			return false;
		}
		gRenderer = gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (gRenderer == NULL) {
			printf("Failed to init Renderer: SDL_Error: %s\n", SDL_GetError());
			return false;
		}
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		int imgFlags = IMG_INIT_PNG;
		if (!(IMG_Init(imgFlags) & imgFlags)) {
			printf("Error Initializing SDL image");
			return false;
		}
	}
	//sprite setup
	if (!SetupBlockSprites()) {
		return false;
	}

	LoadSRSTests();

	//Board setup
	std::vector<std::vector<block>> gBoard(40, std::vector<block>(10, EMPTY));
	board = gBoard;

	//setup piece
	FillPieceQueue();
	currentPiece = nextQueue.front();
	nextQueue.pop();

	
	//spawn top piece
	selection.SpawnNewPiece(static_cast<pieceType>(currentPiece));
	
	//initialize timers
	gravityTimer = Timer();
	lockTimer = Timer();
	
	return true;


}

void Game::LoadSRSTests() {
	std::vector<std::vector<Translation>> normalTests(4, std::vector<Translation>(4));
	Tests = normalTests;
	std::vector<std::vector<Translation>> otherTests(4, std::vector<Translation>(4));
	ITests = otherTests;

	//J, L, T, S, Z Test Data
	// Test going from Orientation 0 -> 1
	Translation test0_1 = { -1, 0 };
	Translation test0_2 = { -1,1 };
	Translation test0_3 = { 0, -2 };
	Translation test0_4 = { -1, -2 };

	Tests[0][0] = test0_1;
	Tests[0][1] = test0_2;
	Tests[0][2] = test0_3;
	Tests[0][3] = test0_4;

	// Test going from Orientation 1 -> 2
	Translation test1_1 = { 1, 0 };
	Translation test1_2 = { 1,-1 };
	Translation test1_3 = { 0, 2 };
	Translation test1_4 = { 1, 2 };

	Tests[1][0] = test1_1;
	Tests[1][1] = test1_2;
	Tests[1][2] = test1_3;
	Tests[1][3] = test1_4;

	// Test going from Orientation 2 -> 3
	Translation test2_1 = { 1, 0 };
	Translation test2_2 = { 1,1 };
	Translation test2_3 = { 0, -2 };
	Translation test2_4 = { 1, -2 };

	Tests[2][0] = test2_1;
	Tests[2][1] = test2_2;
	Tests[2][2] = test2_3;
	Tests[2][3] = test2_4;

	// Test going from Orientation 3 -> 0
	Translation test3_1 = { -1, 0 };
	Translation test3_2 = { -1,-1 };
	Translation test3_3 = { 0, 2 };
	Translation test3_4 = { -1, 2 };

	Tests[3][0] = test3_1;
	Tests[3][1] = test3_2;
	Tests[3][2] = test3_3;
	Tests[3][3] = test3_4;

	// I Test Data
	// Orientation 0 -> 1
	Translation itest0_1 = { -2, 0 };
	Translation itest0_2 = { 1, 0 };
	Translation itest0_3 = { -2, -1 };
	Translation itest0_4 = { 1, 2 };

	ITests[0][0] = itest0_1;
	ITests[0][1] = itest0_2;
	ITests[0][2] = itest0_3;
	ITests[0][3] = itest0_4;

	//Orientation 1 -> 2
	Translation itest1_1 = { -1, 0 };
	Translation itest1_2 = { 2, 0 };
	Translation itest1_3 = { -1, 2 };
	Translation itest1_4 = { 2, -1 };

	ITests[1][0] = itest1_1;
	ITests[1][1] = itest1_2;
	ITests[1][2] = itest1_3;
	ITests[1][3] = itest1_4;

	//Orientation 2 -> 3
	Translation itest2_1 = { 2, 0 };
	Translation itest2_2 = { -1, 0 };
	Translation itest2_3 = { 2, 1 };
	Translation itest2_4 = { -1, -2 };

	ITests[2][0] = itest2_1;
	ITests[2][1] = itest2_2;
	ITests[2][2] = itest2_3;
	ITests[2][3] = itest2_4;

	//Orientation 3 -> 0;
	Translation itest3_1 = { -2, 0 };
	Translation itest3_2 = { 1, 0 };
	Translation itest3_3 = { -2, -1 };
	Translation itest3_4 = { 1, 2 };

	ITests[3][0] = itest3_1;
	ITests[3][1] = itest3_2;
	ITests[3][2] = itest3_3;
	ITests[3][3] = itest3_4;

}


void Game::Gravity() {
	if (gravityTimer.isRunning) {
		uint64_t currentTime = gravityTimer.GetTime();
		if (currentTime >= 1000) {
			gravityTimer.Stop();
			bool isValid = true;
			for (BlockInstance& b : selection.blocks) {
				if (b.y == 0) {
					isValid = false;
					if (!lockTimer.isRunning) {
						lockTimer.Start();
					}
					break;
				}
				else if(board[b.y-1][b.x] != EMPTY) {
					isValid = false;
					if (!lockTimer.isRunning) {
						lockTimer.Start();
					}
					break;
				}
			}
			if (isValid) {
				for (BlockInstance& b : selection.blocks) {
					b.y -= 1;
				}
			}
		}
	}
}

void Game::CheckLock() {
	if (lockTimer.isRunning) {
		uint64_t currentTime = lockTimer.GetTime();
		if (currentTime >= 500) {
			lockTimer.Stop();
			bool validPosition = false;
			for (BlockInstance& b : selection.blocks) {
				if (b.y == 0 || board[b.y - 1][b.x] != EMPTY) {
					validPosition = true;
					break;
				}
			}
			if (validPosition) {
				Lock();
			}
		}
	}
}

void Game::Lock() {
	for (BlockInstance& b : selection.blocks) {
		board[b.y][b.x] = b.col;
	}
	mustSpawn = true;
}


void Game::SpawnPiece() {
	if (nextQueue.size() < 3) {
		FillPieceQueue();
	}
	currentPiece = nextQueue.front();
	nextQueue.pop();
	
	selection.SpawnNewPiece(static_cast<pieceType>(currentPiece));
	mustSpawn = false;
}

void Game::FillPieceQueue() {
	std::vector<pieceType> bag = {I, J, L, O, S, T, Z};
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(bag.begin(), bag.end(), std::default_random_engine(seed));
	for (pieceType& p : bag) {
		nextQueue.push(p);
	}
}

bool Game::SetupBlockSprites() {
	std::string path = "blocks.png";
	blockTexture = IMG_LoadTexture(gRenderer, path.c_str());
	if (blockTexture == NULL) {
		return false;
	}

	//cyan
	blockSprites[0].x = 5;
	blockSprites[0].y = 11;
	blockSprites[0].h = 16;
	blockSprites[0].w = 16;

	//blue
	blockSprites[1].x = 25;
	blockSprites[1].y = 11;
	blockSprites[1].h = 16;
	blockSprites[1].w = 16;

	//orange
	blockSprites[2].x = 45;
	blockSprites[2].y = 11;
	blockSprites[2].h = 16;
	blockSprites[2].w = 16;

	//yellow
	blockSprites[3].x = 65;
	blockSprites[3].y = 11;
	blockSprites[3].h = 16;
	blockSprites[3].w = 16;

	//green
	blockSprites[4].x = 5;
	blockSprites[4].y = 31;
	blockSprites[4].h = 16;
	blockSprites[4].w = 16;

	//purple
	blockSprites[5].x = 25;
	blockSprites[5].y = 31;
	blockSprites[5].h = 16;
	blockSprites[5].w = 16;

	//red
	blockSprites[6].x = 45;
	blockSprites[6].y = 31;
	blockSprites[6].h = 16;
	blockSprites[6].w = 16;
	return true;

}


void Game::clean() {
	SDL_DestroyTexture(blockTexture);
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	blockTexture = NULL;
	gRenderer = NULL;
	gWindow = NULL;
	IMG_Quit();
	SDL_Quit();
}


bool Game::handleInput(SDL_Event &e) {
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) return true;
		
		if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
				case SDLK_LEFT: 
				{
					shift(-1);
					break;
				}
				case SDLK_RIGHT:
				{
					shift(1);
					break;
				}
				case SDLK_UP: {
					if (selection.type != O) {
						RotateClockwise();
					}
				}
			}
		}

	}

	return false;
}

void Game::shift(int amount) {
	bool isValid = true;
	for (BlockInstance &b : selection.blocks) {
		if (b.x + amount >= 10 || b.x + amount < 0) {
			isValid = false;
		}
		else {
			int tempX = b.x + amount;
			if (board[b.y][tempX] != EMPTY) {
				isValid = false;
			}
		}
	}
	if (isValid) {
		gravityTimer.Stop();
		lockTimer.Stop();
		for (BlockInstance &b : selection.blocks) {
			b.x += amount;
		}
	}

}

void Game::RotateClockwise() {
	//Save the last (valid) X and Y coordinates for the tetromino and its Orientation
	int Orientation = selection.orientation;
	std::vector<Translation> lastPositions(4);
	for (int i = 0; i < 4; i++) {
		lastPositions[i] = { selection.blocks[i].x, selection.blocks[i].y };
	}
	selection.RotateClockwise();
	if (!TestRotation(Orientation)) {
		//change the coordinates back to their original values
		for (int i = 0; i < 4; i++) {
			selection.blocks[i].x = lastPositions[i].x;
			selection.blocks[i].y = lastPositions[i].y;
		}
		selection.orientation = Orientation;
	}

}

bool Game::TestRotation(int originalOrientation) {
	bool conflict = false;

	for (BlockInstance& b : selection.blocks) {
		if (ValidPosition(b.x, b.y)) {
			conflict = true;
			break;
		}
	}

	if (!conflict) {
		return true;
	}
	
	pieceType p = selection.type;
	if (p == I) {
		for (int i = 0; i < 4; i++) {
			int tX = ITests[originalOrientation][i].x;
			int tY = ITests[originalOrientation][i].y;
			bool valid = true;
			for (BlockInstance& b : selection.blocks) {
				if (!ValidPosition(b.x + tX, b.y + tY)) {
					valid = false;
					break;
				}
			}
			if (valid) {
				for (BlockInstance& b : selection.blocks) {
					b.x += tX;
					b.y += tY;
				}
				gravityTimer.Stop();
				lockTimer.Stop();
				return true;
			}
		}
	}
	else 
	{
		for (int i = 0; i < 4; i++) {
			int tX = Tests[originalOrientation][i].x;
			int tY = Tests[originalOrientation][i].y;
			bool valid = true;
			for (BlockInstance& b : selection.blocks) {
				if (!ValidPosition(b.x + tX, b.y + tY)) {
					valid = false;
					break;
				}
			}
			if (valid) {
				for (BlockInstance& b : selection.blocks) {
					b.x += tX;
					b.y += tY;
				}
				gravityTimer.Stop();
				lockTimer.Stop();
				return true;
			}
		}
	}
	return false;
}

bool Game::ValidPosition(int x, int y) {
	if (x < 0 || x > 9 || y < 0) {
		return false;
	}
	return (board[y][x] == EMPTY);
}

void Game::runGame() {
	//std::cout << std::endl << "CURRENT PIECE: " << currentPiece << "\n";
	SDL_Event e;
	bool quit = false;
	
	while (!quit) {
		if (mustSpawn) {
			SpawnPiece();
		}


		if (!gravityTimer.isRunning) {
			gravityTimer.Start();
		}
		//Handle Input
		quit = handleInput(e);

		//Game Logic

		Gravity();
		CheckLock();

		//Rendering
		SDL_SetRenderDrawColor(gRenderer, 0x0, 0x0, 0x0, 0xFF);
		SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0x80, 0x80, 0x80, 0xFF);
		SDL_RenderFillRect(gRenderer, &boardBorder);
		SDL_SetRenderDrawColor(gRenderer, 0x0, 0x0, 0x0, 0xFF);
		SDL_RenderFillRect(gRenderer, &boardRect);
		DrawSelection();
		DrawBoard();


		SDL_RenderPresent(gRenderer);
	}

}



void Game::DrawSelection() {
	//SDL_RenderSetViewport(gRenderer, &boardRect);
	if (!mustSpawn) {
		for (int i = 0; i < 4; i++) {
			BlockInstance b = selection.blocks[i];
			SDL_Rect srcRect = blockSprites[b.col];
			SDL_Rect blockRect = { boardTopX + 24 * (b.x), boardTopY + boardH - 24 * (1 + b.y), 24, 24 };
			SDL_RenderCopy(gRenderer, blockTexture, &srcRect, &blockRect);
		}
	}
}

void Game::DrawBoard() {

	for (int i = 0; i < 21; i++) {
		for (int j = 0; j < 10; j++) {
			if (board[i][j] != EMPTY) {
				SDL_Rect srcRect = blockSprites[board[i][j]];
				SDL_Rect blockRect = { boardTopX + 24 * (j), boardTopY + boardH - 24 * (1 + i), 24, 24 };
				SDL_RenderCopy(gRenderer, blockTexture, &srcRect, &blockRect);
			}
		}
	}
}

