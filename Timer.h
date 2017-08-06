#pragma once
class Timer
{
public:
	Timer();
	~Timer();
	bool Tick();

private:
	double mSecondsPerCount;
	double mDeltaTime;
	const double delta = 0.07;

	__int64 mPrevTime;
	__int64 mCurrTime;
};

