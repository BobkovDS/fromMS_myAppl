
#include <windows.h>
#include "Timer.h"

Timer::Timer()
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*) &countsPerSec);
	mSecondsPerCount = 1.0 / (double) countsPerSec;

	QueryPerformanceCounter((LARGE_INTEGER*)&mCurrTime);
	mPrevTime = mCurrTime;
}

Timer::~Timer()
{
}

bool Timer::Tick()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&mCurrTime);

	mDeltaTime = (mCurrTime - mPrevTime)*mSecondsPerCount;

	if (mDeltaTime >= delta)
	{
		mPrevTime = mCurrTime;
		return true;
	}
	else
		return false;
}