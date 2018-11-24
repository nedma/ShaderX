#include "stdafx.h"
#include "d3scene\eng-d3starget.h"

CD3STarget::CD3STarget(char *ObjName,CEngine *OwnerEngine) : CBase3DObject(ObjName, OwnerEngine) {
    Visible = false;
    SetFilled(TRUE);
    SetLost(FALSE);
}

CD3STarget::~CD3STarget(void) {
}

void CD3STarget::PrepareGeometry(CRenderSettings &RenderInfo) {
}

void *CD3STarget::Render(CRenderSettings &RenderSettings, void *Context, CBaseMaterial *ForcedMaterial, CBaseMaterialContext *ForcedContext) {
    return NULL;
}

void CD3STarget::DoCacheIn(CRenderSettings &RenderSettings) {
}

void CD3STarget::DoCacheOut(CRenderSettings &RenderSettings) {
}

bool CD3STarget::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
    SetFilled(TRUE);
    return TRUE;
}

bool CD3STarget::Update(CRenderSettings &RenderSettings) {
    return TRUE;
}

void CD3STarget::DeviceLost(CRenderSettings &Settings) {
    SetLost(FALSE);
}
