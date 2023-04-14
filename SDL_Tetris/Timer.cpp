#include "Timer.h"


Timer::Timer() {
	startTime = 0;
	stopTime = 0;
}

void Timer::Start() {
	if (!isRunning) {
		startTime = SDL_GetTicks64();
		stopTime = 0;
		isRunning = true;
	}
	else {
		std::cerr << "Timer is already running, you must stop first" << std::endl;
	}
}

void Timer::Stop() {
	if (isRunning) {
		stopTime = SDL_GetTicks64();
		isRunning = false;
	}
	else {
		//std::cerr << "Timer is is not running" << std::endl;
	}
}


uint64_t Timer::GetTime() {
	if (isRunning) {
		return SDL_GetTicks64() - startTime;
	}
	else if (!isRunning && startTime > 0 && stopTime > 0) {
		return stopTime - startTime;
	}
	else {
		return 0;
	}
}




