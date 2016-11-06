#pragma once

#include <Windows.h>

class GameTimer
{
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mStopped;
public:
	GameTimer();

	float TotalTime() const;
	float DeltaTime() const;
	
	void Reset();
	void Start();
	void Stop();
	void Tick();
};

