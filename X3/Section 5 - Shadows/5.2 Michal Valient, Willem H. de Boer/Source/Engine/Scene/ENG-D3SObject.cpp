#include "stdafx.h"
#include "d3scene\eng-d3sobject.h"

#include "ENG-RenderSettings.h"
#include "ENG-BaseTexture.h"
#include "ENG-TextureManager.h"
#include "ENG-MeshManager.h"
#include "ENG-Mesh.h"
#include "ENG-Engine.h"
#include "ENG-MeshPart.h"
#include "Material\MAT-BaseMaterial.h"
#include "Material\MAT-MaterialManager.h"
#include "SC-HashTable2.h"
#include "sc-xmlparser.h"

CD3SObject::CD3SObject(char *ObjName,CEngine *OwnerEngine) : CBase3DObject(ObjName, OwnerEngine) {
    Mesh = NULL;
    SubSetMaterials = new CHashTable2(true,0);
    Visible = true;
}

CD3SObject::~CD3SObject(void) {
    int loge = _LOGB(this,D3_DV_GRP2_LEV0, "Destroying");
    if (Mesh!=NULL) this->OwnerEngine->GetMeshManager()->Delete(Mesh->GetResourceID());
    delete SubSetMaterials;
    _LOGE(loge);
}

void CD3SObject::DoCacheIn(CRenderSettings &RenderSettings) {
    CBaseResource::DoCacheIn(RenderSettings);
    Mesh->CacheIn(RenderSettings);
    if (SubSetMaterials->GetItemCount()) {
        UINT cback = SubSetMaterials->GetFirstItem();
        while (1) {
            CMatSubset *MS = (CMatSubset *)SubSetMaterials->GetValueAtIndex(cback);
            if (MS->Material!=NULL) MS->Material->CacheIn(RenderSettings);
            UINT cback2 = SubSetMaterials->GetNextItem(cback);
            if (cback2 == cback) break;
            else cback = cback2;
        }
    }
    this->Update(RenderSettings);
}

void CD3SObject::DoCacheOut(CRenderSettings &RenderSettings) {
    Mesh->CacheOut(RenderSettings);
    if (SubSetMaterials->GetItemCount()) {
        UINT cback = SubSetMaterials->GetFirstItem();
        while (1) {
            CMatSubset *MS = (CMatSubset *)SubSetMaterials->GetValueAtIndex(cback);
            if (MS->Material!=NULL) MS->Material->CacheOut(RenderSettings);
            UINT cback2 = SubSetMaterials->GetNextItem(cback);
            if (cback2 == cback) break;
            else cback = cback2;
        }
    }
}

//Renders object to screen allowing to use own material.
void *CD3SObject::Render(CRenderSettings &RenderSettings)
{
    for (int i=0; i<Mesh->GetSubsetCount(); i++) {
        CMatSubset *MS = (CMatSubset *)SubSetMaterials->Get(i);
        MS->Material->Render(RenderSettings, Mesh, i, this);
    }
    return NULL;
}

bool CD3SObject::EndRendering(CRenderSettings &RenderSettings)
{
    for (int i=0; i<Mesh->GetSubsetCount(); i++) {
        CMatSubset *MS = (CMatSubset *)SubSetMaterials->Get(i);
        MS->Material->EndRendering(RenderSettings);
    }
    return true;
}

//Renders object to screen without setting explicit material - just render the geometry.
void *CD3SObject::RenderGeometry(CRenderSettings &RenderSettings)
{
    Mesh->Render(RenderSettings,-1);
    return NULL;
}

void CD3SObject::PrepareBoundingGeometry(CRenderSettings &RenderInfo) {
    if (RenderInfo.RenderID != TransformRenderID) {
        this->BSCenter = Mesh->BoundSphereCenter;
        this->BSRadius = Mesh->BoundSphereRadius;
        CBase3DObject::PrepareBoundingGeometry(RenderInfo);
    }
}

void CD3SObject::PrepareGeometry(CRenderSettings &RenderInfo) {
}

void CD3SObject::DeviceLost(CRenderSettings &Settings) {
    Mesh->DeviceLost(Settings);
    if (Mesh->GetLost()) SetLost(TRUE);
    if (SubSetMaterials->GetItemCount()) {
        UINT cback = SubSetMaterials->GetFirstItem();
        while (1) {
            CMatSubset *MS = (CMatSubset *)SubSetMaterials->GetValueAtIndex(cback);
            if (MS->Material!=NULL) {
                MS->Material->DeviceLost(Settings);
                if (MS->Material->GetLost()) SetLost(TRUE);
            }
            UINT cback2 = SubSetMaterials->GetNextItem(cback);
            if (cback2 == cback) break;
            else cback = cback2;
        }
    }
}

bool CD3SObject::AssignMesh(DWORD MeshID) {
    if (this->Mesh!=NULL) {
        this->OwnerEngine->GetMeshManager()->Delete(this->Mesh->GetResourceID());
        this->Mesh = NULL;
    }
    if (MeshID<0xFFFFFFFF) {
        this->Mesh = (CMesh *)OwnerEngine->GetMeshManager()->Query(MeshID);
        if (this->Mesh == NULL) {
            _WARN(this,ErrMgr->TSystem,"Mesh with ID %u cannot be queried",MeshID);
            return false;
        }
    }
    return true;
}

bool CD3SObject::AssignMaterial(DWORD SubsetID, DWORD MaterialID) {
    CMatSubset *MS = (CMatSubset *)SubSetMaterials->Get(SubsetID);
    if (MS==NULL) {
        MS = new CMatSubset;
        SubSetMaterials->Add(SubsetID,MS);
    }
    if (MS!=NULL && MS->Material!=NULL) {
        OwnerEngine->GetMaterialManager()->Delete(MS->Material->GetResourceID());
        MS->Material = NULL;
    }
    if (MaterialID<0xFFFFFFFF) {
        MS->Material = (CBaseMaterial *)OwnerEngine->GetMaterialManager()->Query(MaterialID);
        if (MS->Material == NULL) {
            _WARN(this,ErrMgr->TSystem,"Material with ID %u cannot be queried",MaterialID);
            return false;
        }
    }
    return true;
}
 
bool CD3SObject::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
    if ((Mesh!=NULL) && (!Mesh->GetFilled())) Mesh->Load(Mesh->GetLoadIndex(), RenderSettings);
    if (SubSetMaterials->GetItemCount()) {
        UINT cback = SubSetMaterials->GetFirstItem();
        while (1) {
            CMatSubset *MS = (CMatSubset *)SubSetMaterials->GetValueAtIndex(cback);
            if ((MS->Material!=NULL) && (!MS->Material->GetFilled())) MS->Material->Load(MS->Material->GetLoadIndex(),RenderSettings);
            UINT cback2 = SubSetMaterials->GetNextItem(cback);
            if (cback2 == cback) break;
            else cback = cback2;
        }
    }
    this->SetFilled(TRUE);
    return TRUE;
}

bool CD3SObject::Update(CRenderSettings &RenderSettings) {
    if (Mesh!=NULL) Mesh->Update(RenderSettings);
    if (SubSetMaterials->GetItemCount()) {
        UINT cback = SubSetMaterials->GetFirstItem();
        while (1) {
            CMatSubset *MS = (CMatSubset *)SubSetMaterials->GetValueAtIndex(cback);
            if (MS->Material!=NULL) MS->Material->Update(RenderSettings);
            UINT cback2 = SubSetMaterials->GetNextItem(cback);
            if (cback2 == cback) break;
            else cback = cback2;
        }
    }
    return TRUE;
}

CD3SObject::CMatSubset::~CMatSubset() {
    if (Material!=NULL) {
        Material->OwnerEngine->GetMaterialManager()->Delete(Material->GetResourceID());
    }
}

CD3SObject::CMatSubset::CMatSubset() {
    Material = NULL;
}

bool CD3SObject::LoadXML(CXMLNode *Node, CRenderSettings &Settings) {
    CBase3DObject::LoadXML(Node, Settings);
    MeshID = 0;
    Node->GetAttributeValueI("MESHID", MeshID);
    return true;
}
