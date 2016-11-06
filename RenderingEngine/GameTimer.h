#pragma once
class GameTimer
{
public:
	GameTimer();
	~GameTimer();
	void Reset();
	void Start();
	void Stop();
	void Tick();
private:
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mStopped;
public:
	float GameTime() const;
	float DeltaTime() const;
};
