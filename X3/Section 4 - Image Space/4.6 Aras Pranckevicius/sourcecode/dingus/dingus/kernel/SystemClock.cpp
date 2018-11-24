// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// --------------------------------------------------------------------------
#include "stdafx.h"

#include "SystemClock.h"

using namespace dingus;


void CSystemClock::setTimes( double time, double lastPerformDuration, int performCount )
{
	mTime = time-mResetTime;
	mLastPerformDuration = lastPerformDuration;
	mPerformCount = performCount;
}

