#pragma once
#include "baseclass.h"

class CRenderSettings;

class CFrameRate : public CBaseClass {
protected:
    UINT LastRenderFrame;
    double LastApplicationTime;

    UINT MeasurementCount;
    double FPSCumulative;
public:
    D3_INLINE double GetLastApplicationTime() {
        return LastApplicationTime;
    }

    CFrameRate(char *ObjName,CEngine *OwnerEngine);
    virtual ~CFrameRate(void);
    double GetFPS(CRenderSettings &Settings);
    double GetFinalFPS();

    MAKE_CLSCREATE(CFrameRate);
    MAKE_CLSNAME("CFrameRate");
    MAKE_DEFAULTGARBAGE()
};
