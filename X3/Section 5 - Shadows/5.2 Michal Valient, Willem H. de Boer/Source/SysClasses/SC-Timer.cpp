// SC-Timer.cpp: implementation of the CTimer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SC-Timer.h"
#include "SC-MString.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimer::CTimer(char *pObjName,CEngine *pOwnerEngine) : CBaseClass(pObjName,pOwnerEngine) {
    int logE = _LOGB(this,D3_DV_GRP1_LEV0,"Starting timer");
    ZeroTimer(true);
    _LOGE(logE,"Created");
}

CTimer::~CTimer() {
    //TODO: print application statistic
    int logE = _LOGB(this,D3_DV_GRP1_LEV0,"Destroying timer");
    GetTimer();
    _LOG(this,D3_DV_GRP1_LEV0,"Start tick count was %I64i", HW_StartTickCount);
    _LOG(this,D3_DV_GRP1_LEV0,"Current tick count is %I64i", HW_LastGetTime);
    _LOG(this,D3_DV_GRP1_LEV0,"Hardware tick counter overlaeapped %I64i times", HW_OverleapCount);
    _LOG(this,D3_DV_GRP1_LEV0,"Tick count from start of timer is %I64i (with %I64i overleaps)", App_TickCount, App_OverleapCount);
    _LOG(this,D3_DV_GRP1_LEV0,"Tick count from start of timer in seconds is %f", ToSeconds(App_TickCount));
    _LOGE(logE,"Destroyed");
}
                                                           
void CTimer::ZeroTimer(bool Paused) {
    LARGE_INTEGER Freq;
    HW_CountsPerSecond = 0;
    if (QueryPerformanceFrequency(&Freq)) { //check for presence of HiRes timer
        if (Freq.QuadPart>0) HW_CountsPerSecond = Freq.QuadPart;   //Get ticks per second
    }
    if (HW_CountsPerSecond>0) {    //we have hires timer
        HighResTimer = true;
        LARGE_INTEGER CurrentCount;
        QueryPerformanceCounter(&CurrentCount);
        HW_StartTickCount = CurrentCount.QuadPart;
        __int64 over = 9223372036854775807 / (HW_CountsPerSecond * 60 * 60 * 24);
        CMString TypeOver = "days";
        if (over>365) {
            over = over / 365;
            TypeOver = "years";
        }
        _LOG(this,D3_DV_GRP1_LEV0,"HiRes timer with %I64i counts per second (overleap period: %I64i %s)", HW_CountsPerSecond, over, TypeOver.c_str());
        _LOG(this,D3_DV_GRP1_LEV0,"Current tick count is %I64i", HW_StartTickCount);
        __int64 nextOver = (9223372036854775807-HW_StartTickCount) / (HW_CountsPerSecond * 60 * 60 * 24);
        CMString TypeNextOver = "days";
        if (nextOver>365) {
            nextOver = nextOver / 365;
            TypeNextOver = "years";
        }
        _LOG(this,D3_DV_GRP1_LEV0,"Next overleap will be in %I64i %s", nextOver, TypeNextOver.c_str());
    } else {
        HighResTimer = false;
        HW_CountsPerSecond = 1000;
        DWORD SysTime = GetTickCount();
        HW_StartTickCount = SysTime;
        __int64 over = 0xFFFFFFFF / (HW_CountsPerSecond * 60 * 60 * 24);
        CMString TypeOver = "days";
        if (over>365) {
            over = over / 365;
            TypeOver = "years";
        }
        _LOG(this,D3_DV_GRP1_LEV0,"Normal timer with %I64i counts per second (overleap period: %I64u %s)", HW_CountsPerSecond, over, TypeOver.c_str());
        _LOG(this,D3_DV_GRP1_LEV0,"Current tick count is %I64i", HW_StartTickCount);
        __int64 nextOver = (0xFFFFFFFF-HW_StartTickCount) / (HW_CountsPerSecond * 60 * 60 * 24);
        CMString TypeNextOver = "days";
        if (nextOver>365) {
            nextOver = nextOver / 365;
            TypeNextOver = "years";
        }
        _LOG(this,D3_DV_GRP1_LEV0,"Next overleap will be in %I64i %s", nextOver, TypeNextOver.c_str());
    };
    App_TickCount = 0;
    App_OverleapCount = 0;
    HW_OverleapCount = 0;
    HW_LastGetTime = HW_StartTickCount;
    TickDelta = 0;
    this->Paused = Paused;
}

void CTimer::GetTimerHi() {
    LARGE_INTEGER CurrentCount;
    QueryPerformanceCounter(&CurrentCount);
    __int64 SysTime = CurrentCount.QuadPart;
    if (SysTime<HW_LastGetTime) { //time overleapped
        TickDelta = (9223372036854775807 - HW_LastGetTime) + SysTime;
        HW_OverleapCount++;
    } else {
        TickDelta = SysTime - HW_LastGetTime;
    }
    HW_LastGetTime = SysTime;
}

void CTimer::GetTimerLo() {
    __int64 SysTime = GetTickCount();
    if ((DWORD)SysTime<(DWORD)HW_LastGetTime) {
        TickDelta = (0xFFFFFFFF - HW_LastGetTime) + SysTime;
        HW_OverleapCount++;
    } else {
        TickDelta = SysTime - HW_LastGetTime;
    }
    HW_LastGetTime = SysTime;
}



__int64 CTimer::GetTimer() {
    if (!Paused) {
        //Get current ticks
        if (HighResTimer) GetTimerHi();
        else GetTimerLo();
        //Resolve application counts
        if ((App_TickCount > 0) && (App_TickCount+TickDelta<=0)) {    //Overleaped application tick counter
            App_OverleapCount++;
        }
        //_LOG(this,D3_DV_GRP0_LEV0,"App Timer %I64i, Delta %I64i, Paused Tick %I64i", App_TickCount, TickDelta, Paused_TickCount);
        App_TickCount += TickDelta;
        return App_TickCount;
    } else {    //paused, return last App_TickCount
        return App_TickCount;
    }
}

double CTimer::ToSeconds(__int64 TickCount) {
    __int64 gtime = TickCount;
    __int64 FullPart = gtime / HW_CountsPerSecond;
    __int64 FracPart = gtime % HW_CountsPerSecond;
    return ((double)FullPart) + ((double)FracPart / (double)HW_CountsPerSecond);
}

double CTimer::GetTimerSecond() {
    return ToSeconds(GetTimer());
}

void CTimer::PauseTimer() {
    if (Paused) return;
    Paused_TickCount = App_TickCount;
    GetTimer();
    Paused = true;
    //_LOG(this,D3_DV_GRP0_LEV0,"PAUSE - App Timer %I64i, Delta %I64i, Paused Tick %I64i", App_TickCount, TickDelta, Paused_TickCount);
    _LOG(this,D3_DV_GRP1_LEV0,"Pausing in %.4f second (and %I64i overleaps)", ToSeconds(Paused_TickCount), App_OverleapCount);
}

void CTimer::ResumeTimer() {
    if (!Paused) return;
    Paused = false;
    GetTimer();
    App_TickCount = Paused_TickCount;
    //_LOG(this,D3_DV_GRP0_LEV0,"RESUME - App Timer %I64i, Delta %I64i, Paused Tick %I64i", App_TickCount, TickDelta, Paused_TickCount);
    _LOG(this,D3_DV_GRP1_LEV0,"Continuing after %.4f seconds (and %I64i overleaps)", ToSeconds(TickDelta), App_OverleapCount);
}
