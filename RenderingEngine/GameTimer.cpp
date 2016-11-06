#include "GameTimer.h"



GameTimer::GameTimer()
{
	mSecondsPerCount = 0.0;
	mDeltaTime = 0.0;
}


GameTimer::~GameTimer()
{
}


void GameTimer::Reset()
{
}


void GameTimer::Start()
{
}


void GameTimer::Stop()
{
}


void GameTimer::Tick()
{
}

float GameTimer::GameTime() const
{
	return 0.0f;
}

float GameTimer::DeltaTime() const
{
	return 0.0f;
}
