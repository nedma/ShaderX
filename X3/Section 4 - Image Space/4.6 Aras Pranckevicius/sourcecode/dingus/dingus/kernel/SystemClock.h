// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// --------------------------------------------------------------------------

#ifndef __SYSTEM_CLOCK_H
#define __SYSTEM_CLOCK_H

#include "../utils/Singleton.h"

namespace dingus {

class CSystemClock : public CSingleton<CSystemClock> {
public:
	void reset( double time ) { mResetTime = time; }
	void setTimes( double time, double lastPerformDuration, int performCount );

	double getTime() const { return mTime; }
	double getLastPerformDuration() const { return mLastPerformDuration; }
	int getPerformCount() const { return mPerformCount; }

private:
	CSystemClock() : mResetTime(0), mTime(0), mLastPerformDuration(0), mPerformCount(0) { }
	IMPLEMENT_SIMPLE_SINGLETON(CSystemClock);
	
private:
	double	mResetTime;
	double	mTime;
	double	mLastPerformDuration;
	int		mPerformCount;
};

}; // namespace

#endif
