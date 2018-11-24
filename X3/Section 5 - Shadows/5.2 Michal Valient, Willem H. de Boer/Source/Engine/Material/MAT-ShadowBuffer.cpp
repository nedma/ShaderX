#include "stdafx.h"
#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"

#include "material\MAT-ShadowBuffer.h"
#include "material\MAT-MaterialManager.h"

#include "ENG-RenderSettings.h"
#include "ENG-Engine.h"
#include "ENG-Texture2D.h"
#include "ENG-TextureManager.h"
#include "ENG-Base3DObject.h"
#include "MGR-DirectGraphicsManager.h"
#include "WND-WindowManager.h"
#include "FIL-FileManager.h"
#include "SC-MString.h"
#include "DG-DirectGraphicsDeviceInfo.h"
#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"
#include "ENG-BaseTexture.h"
#include "d3scene\eng-d3slight.h"
#include "sc-dynamicarray.h"
#include "eng-enginedefaults.h"
#include "eng-mesh.h"

#include "Shader\SHD-ShaderManager.h"
#include "shader\shd-vertexshader.h"
#include "shader\shd-pixelshader.h"

CMaterialShadowBuffer::CMaterialShadowBuffer(char *ObjName,CEngine *OwnerEngine) : CDX9Material(ObjName,OwnerEngine) {
    _LOG(this,D3_DV_GRP2_LEV0,"Creating");
}

CMaterialShadowBuffer::~CMaterialShadowBuffer(void) {
    _LOG(this,D3_DV_GRP2_LEV0,"Destroying");
    DeleteShaders();
}

bool CMaterialShadowBuffer::Clone(CDX9Material **pOut) {
    CDX9Material *pMyOut = NULL;
    DWORD dwID = OwnerEngine->GetUniqueTemporaryNumber();
    CMString szName;
    szName.Format("%s_%u", GetObjName(), dwID);
    pMyOut = new CMaterialShadowBuffer(szName.c_str(), OwnerEngine);
    pMyOut->SetResourceID(dwID);
    OwnerEngine->GetMaterialManager()->Add(pMyOut->GetResourceID(), *pMyOut);
    *pOut = pMyOut;
    return true;
}

void CMaterialShadowBuffer::DeviceLost(CRenderSettings &Settings) {
    Effect->OnLostDevice();
    SetLost(true);
    DeleteShaders();
}

void CMaterialShadowBuffer::DeleteShaders() {
    VSDeclaration.Release();
    Effect.Release();
    SetFilled(false);
}

bool CMaterialShadowBuffer::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV3,"Loading material");
    DeleteShaders();
    //Vertex shaders support
    if((OwnerEngine->GetGraphicsManager()->StartBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) || 
        (OwnerEngine->GetGraphicsManager()->StartBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)) {
            if( OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.VertexShaderVersion < D3DVS_VERSION(2,0) ) {
                _WARN(this,ErrMgr->TNoSupport,"Vertex shaders version 2.0 not supported");
            }
    }
    //pixel shaders support
    if(OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.PixelShaderVersion  < D3DVS_VERSION(2,0)) {
        _WARN(this,ErrMgr->TNoSupport,"Pixel shaders version 2.0 not supported");
    }

    //shader input declaration
    D3DVERTEXELEMENT9  dwDecl[] = {
        { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 24,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 32,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
        D3DDECL_END()
    };
    RenderSettings.Direct3DDevice->CreateVertexDeclaration(dwDecl, &VSDeclaration);

    Technique = 0;
    ToClipMatrix = 0;
    ToWorld = 0;
    LightPos = 0;
    LightRanges = 0;

    //Load the effect
    HRESULT hr = LoadFromSource("CMaterialShadowBuffer.fx", RenderSettings, &Effect);
    if (FAILED(hr)) {
        _WARN(this, ErrMgr->TFile, "Failed to load the shader");
    }
    else
    {
        SetFilled(true);
        Effect->FindNextValidTechnique(NULL, &Technique);
        ToClipMatrix = Effect->GetParameterBySemantic(NULL, "D3_MATRIX_TO_CLIP");
        ToWorld = Effect->GetParameterBySemantic(NULL, "D3_MATRIX_TO_WORLD");
        LightPos = Effect->GetParameterBySemantic(NULL, "D3_LIGHT_POS");
        LightRanges = Effect->GetParameterBySemantic(NULL, "D3_LIGHT_RANGES");
    }
    _LOGE(logE,"Loaded");
    return true;
}

bool CMaterialShadowBuffer::Update(CRenderSettings &RenderSettings) {
    return true;
}

bool CMaterialShadowBuffer::Render(CRenderSettings &RenderSettings, CMesh *pMesh, 
                                   DWORD dwSubset, CBase3DObject *pObject)
{
    d3CacheInfo tmpI;
    tmpI.pMesh = pMesh;
    tmpI.Index = dwSubset;
    D3DXMatrixTranspose(&tmpI.ToWorld, &pObject->WorldTransform);
    D3DXMatrixMultiplyTranspose(&tmpI.ToClip, &pObject->WorldTransform, &RenderSettings.CameraTransformMatrix);
    vMeshes.push_back(tmpI);
    return true;
}

bool CMaterialShadowBuffer::Render(CRenderSettings &RenderSettings, CBase3DObject *pObject)
{
    d3CacheInfo tmpI;
    tmpI.pMesh = pObject;
    tmpI.Index = 0xFFFFFFFF;
    D3DXMatrixTranspose(&tmpI.ToWorld, &pObject->WorldTransform);
    D3DXMatrixMultiplyTranspose(&tmpI.ToClip, &pObject->WorldTransform, &RenderSettings.CameraTransformMatrix);
    vObjects.push_back(tmpI);
    return true;
}

bool CMaterialShadowBuffer::EndRendering(CRenderSettings &RenderSettings)
{
    RenderSettings.Direct3DDevice->SetVertexDeclaration(VSDeclaration);

    
    float rcpZ = 1.0f / (pLight->AttenuationFarEnd - pLight->AttenuationNearStart);
    //float shZAdjust = 1.0f / 100.0f;             //depth adjust
    float shZAdjust = 0.0f;             //depth adjust
    D3DXVECTOR4 Ranges(rcpZ, ((-pLight->AttenuationNearStart) * rcpZ) + shZAdjust, 0, 0);
    Effect->SetVector(LightRanges, &Ranges);
    D3DXVECTOR4 tmpVec(pLight->WorldObjectPos);
    Effect->SetVector(LightPos, &tmpVec);

    UINT dwPassCount;
    Effect->Begin(&dwPassCount, 0);
    for (UINT i=0; i<dwPassCount; i++)
    {
        Effect->BeginPass(i);
        UINT meshcount = vMeshes.size();
        for (UINT j=0; j<meshcount; j++)
        {
            Effect->SetMatrixTranspose(ToClipMatrix, &vMeshes[j].ToClip);
            Effect->SetMatrixTranspose(ToWorld, &vMeshes[j].ToWorld);
            Effect->CommitChanges();
            CMesh *pMesh = (CMesh *)vMeshes[j].pMesh;
            pMesh->Render(RenderSettings, vMeshes[j].Index);
        }
        UINT objectCount = vObjects.size();
        for (UINT j=0; j<objectCount; j++)
        {
            Effect->SetMatrixTranspose(ToClipMatrix, &vObjects[j].ToClip);
            Effect->SetMatrixTranspose(ToWorld, &vObjects[j].ToWorld);
            Effect->CommitChanges();
            CBase3DObject *pObject = (CBase3DObject *)vObjects[j].pMesh;
            pObject->RenderGeometry(RenderSettings);
        }
        Effect->EndPass();
    }
    Effect->End();

    vMeshes.clear();
    vObjects.clear();
    return true;
}

bool CMaterialShadowBuffer::LoadXML(CXMLNode *Node, CRenderSettings &Settings)
{
    return true;
}
