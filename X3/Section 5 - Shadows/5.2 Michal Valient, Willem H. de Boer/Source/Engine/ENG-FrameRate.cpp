#include "stdafx.h"
#include "eng-framerate.h"

#include "ENG-RenderSettings.h"

CFrameRate::CFrameRate(char *ObjName,CEngine *OwnerEngine) : CBaseClass(ObjName, OwnerEngine) {
    _LOG(this, D3_DV_GRP0_LEV0, "Created");
    LastRenderFrame = 0;
    LastApplicationTime = 0;

    MeasurementCount = 0;
    FPSCumulative = 0;
}

CFrameRate::~CFrameRate(void) {
    GetFinalFPS();
    _LOG(this, D3_DV_GRP0_LEV0, "Destroyed");
}

double CFrameRate::GetFPS(CRenderSettings &Settings) {
    double frames = (double)Settings.RenderID - (double)LastRenderFrame;
    double result = frames / (Settings.RenderTime - LastApplicationTime);
    LastRenderFrame = Settings.RenderID;
    LastApplicationTime = Settings.RenderTime;

    MeasurementCount++;
    FPSCumulative = FPSCumulative + result;

    _LOG(this, D3_DV_GRP0_LEV0, "Framerate is %f FPS", result);
    return result;
}

double CFrameRate::GetFinalFPS() {
    double result = FPSCumulative / (double)MeasurementCount;
    _LOG(this, D3_DV_GRP0_LEV0, "Final (average) framerate from %u measurements is %f FPS", MeasurementCount, result);
    return result;
}