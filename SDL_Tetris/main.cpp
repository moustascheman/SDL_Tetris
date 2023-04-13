#include <iostream>
#include <stdio.h>
#include "game.h"

int main(int argc, char* argv[]) {
	std::cout << "HELLO WORLD";
	Game* tetris = new Game();
	tetris->Initialize();
	tetris->runGame();
	tetris->clean();
	delete tetris;
	return 0;
}