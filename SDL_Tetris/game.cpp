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


void Game::Gravity() {
	if (gravityTimer.isRunning) {
		uint64_t currentTime = gravityTimer.GetTime();
		if (currentTime >= 1000) {
			gravityTimer.Stop();
			bool isValid = true;
			for (BlockInstance& b : selection.blocks) {
				if (b.y - 1 < 0) {
					isValid = false;
					break;
				}
				else if(board[b.y-1][b.x] != EMPTY) {
					isValid = false;
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
		for (BlockInstance &b : selection.blocks) {
			b.x += amount;
		}
	}

}


void Game::runGame() {
	//std::cout << std::endl << "CURRENT PIECE: " << currentPiece << "\n";
	SDL_Event e;
	bool quit = false;
	
	while (!quit) {
		if (!gravityTimer.isRunning) {
			gravityTimer.Start();
		}
		//Handle Input
		quit = handleInput(e);

		//Game Logic

		Gravity();

		//Rendering
		SDL_SetRenderDrawColor(gRenderer, 0x0, 0x0, 0x0, 0xFF);
		SDL_RenderClear(gRenderer);
		SDL_SetRenderDrawColor(gRenderer, 0x80, 0x80, 0x80, 0xFF);
		SDL_RenderFillRect(gRenderer, &boardBorder);
		SDL_SetRenderDrawColor(gRenderer, 0x0, 0x0, 0x0, 0xFF);
		SDL_RenderFillRect(gRenderer, &boardRect);
		DrawSelection();



		SDL_RenderPresent(gRenderer);
	}

}



void Game::DrawSelection() {
	//SDL_RenderSetViewport(gRenderer, &boardRect);
	for (int i = 0; i < 4; i++) {
		BlockInstance b = selection.blocks[i];
		SDL_Rect srcRect = blockSprites[b.col];
		SDL_Rect blockRect = { boardTopX + 24*(b.x), boardTopY + boardH - 24*(1+b.y), 24, 24};
		SDL_RenderCopy(gRenderer, blockTexture, &srcRect, &blockRect);
	}
}

