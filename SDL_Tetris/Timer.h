#include <iostream>
#include <stdio.h>
#include <SDL.h>

class Timer
{
	public:
		Timer();
		~Timer() {};
		uint64_t GetTime();
		void Start();
		void Stop();
		bool isRunning = false;
	private:
		bool isPaused = false;
		uint64_t startTime;
		uint64_t stopTime;



};

