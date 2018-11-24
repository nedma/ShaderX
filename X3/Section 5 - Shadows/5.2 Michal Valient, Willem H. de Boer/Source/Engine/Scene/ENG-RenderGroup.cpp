#include "stdafx.h"
#include "eng-rendergroup.h"
#include "material\MAT-NoOutput.h"
#include "Material\MAT-MaterialManager.h"
#include "ENG-Engine.h"

#include "SC-DynamicArray.h"
#include "eng-base3dobject.h"

CRenderGroup::CRenderGroup(char *ObjName,CEngine *OwnerEngine) : CBaseClass(ObjName, OwnerEngine) {
    Objects = new CRootClassArray(FALSE);
    Visible = new CRootClassArray(FALSE);

    ///Create one material with ID D3_GRI_Mat_PixelPhong
    NoOutput = (CBaseMaterial *)OwnerEngine->GetMaterialManager()->Query(D3_GRI_Mat_PixelNoOutput);
    if (NoOutput == NULL) {
        NoOutput = new CMaterialNoOutput("DefaultSimple",this->OwnerEngine);
        NoOutput->SetResourceID(D3_GRI_Mat_PixelNoOutput);
        NoOutput->Load(0, *OwnerEngine->GetRenderSettings());
        OwnerEngine->GetMaterialManager()->Add(NoOutput->GetResourceID(),*NoOutput);
    }
}

CRenderGroup::~CRenderGroup(void) {
    delete Objects;
    delete Visible;
    OwnerEngine->GetMaterialManager()->Delete(D3_GRI_Mat_PixelNoOutput);
}

void CRenderGroup::AddVisible(CBase3DObject &Object) {
    Visible->Add(&Object);
}

void CRenderGroup::AddObject(CBase3DObject &Object) {
    Objects->Add(&Object);
}

void CRenderGroup::ClearVisible() {
    Visible->RemoveAll();
}

DWORD CRenderGroup::GetVisibleCount() {
    return Visible->GetSize();
}

CBase3DObject *CRenderGroup::GetVisible(DWORD Index) {
    return (CBase3DObject *)Visible->GetAt(Index);
}

void CRenderGroup::Render(CRenderSettings &Settings) {
/*
    for (int j=0; j<Visible->GetSize(); j++) {
        CBase3DObject *OC = (CBase3DObject *)Visible->GetAt(j);
        NoOutput->Render(Settings, OC);
    }
    NoOutput->EndRendering(Settings);
*/
//*
    for (int j=0; j<Visible->GetSize(); j++) {
        CBase3DObject *OC = (CBase3DObject *)Visible->GetAt(j);
        OC->Render(Settings);
    }

    for (int j=0; j<Visible->GetSize(); j++) {
        CBase3DObject *OC = (CBase3DObject *)Visible->GetAt(j);
        OC->EndRendering(Settings);
    }
//*/
}
