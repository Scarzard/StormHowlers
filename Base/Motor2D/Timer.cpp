// ----------------------------------------------------
// Timer.cpp
// Fast timer with milisecons precision
// ----------------------------------------------------

#include "Timer.h"
#include "SDL\include\SDL_timer.h"

// ---------------------------------------------
Timer::Timer()
{
	Start();
}

// ---------------------------------------------
void Timer::Start()
{
	// (OLD) TODO 1: Fill Start(), Read(), ReadSec() methods
	// they are simple, one line each!
	started_at = SDL_GetTicks();
}

// ---------------------------------------------
uint32 Timer::Read() const
{
	return SDL_GetTicks() - started_at;
}

// ---------------------------------------------
float Timer::ReadSec() const
{
	return float(SDL_GetTicks() - started_at) / 1000.0f;
}