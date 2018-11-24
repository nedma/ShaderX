#include "stdafx.h"
#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"

#include "material\MAT-NoOutput.h"
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
#include "ENG-BaseTexture.h"
#include "d3scene\eng-d3slight.h"
#include "sc-dynamicarray.h"
#include "eng-enginedefaults.h"
#include "eng-mesh.h"

#include "Shader\SHD-ShaderManager.h"
#include "shader\shd-vertexshader.h"
#include "shader\shd-pixelshader.h"

CMaterialNoOutput::CMaterialNoOutput(char *ObjName,CEngine *OwnerEngine) : CDX9Material(ObjName,OwnerEngine) {
    _LOG(this,D3_DV_GRP2_LEV0,"Creating");
}

CMaterialNoOutput::~CMaterialNoOutput(void) {
    _LOG(this,D3_DV_GRP2_LEV0,"Destroying");
    DeleteShaders();
}

bool CMaterialNoOutput::Clone(CDX9Material **pOut) {
    CDX9Material *pMyOut = NULL;
    DWORD dwID = OwnerEngine->GetUniqueTemporaryNumber();
    CMString szName;
    szName.Format("%s_%u", GetObjName(), dwID);
    pMyOut = new CMaterialNoOutput(szName.c_str(), OwnerEngine);
    pMyOut->SetResourceID(dwID);
    OwnerEngine->GetMaterialManager()->Add(pMyOut->GetResourceID(), *pMyOut);
    *pOut = pMyOut;
    return true;
}

void CMaterialNoOutput::DeviceLost(CRenderSettings &Settings) {
    Effect->OnLostDevice();
    SetLost(true);
    DeleteShaders();
}

void CMaterialNoOutput::DeleteShaders() {
    VSDeclaration.Release();
    Effect.Release();
    SetFilled(false);
}

bool CMaterialNoOutput::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV3,"Loading material");
    DeleteShaders();
    //Vertex shaders support
    if((OwnerEngine->GetGraphicsManager()->StartBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) || 
        (OwnerEngine->GetGraphicsManager()->StartBehaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING)) {
            if( OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.VertexShaderVersion < D3DVS_VERSION(1,1) ) {
                _WARN(this,ErrMgr->TNoSupport,"Vertex shaders version 1.1 not supported");
            }
    }
    //pixel shaders support
    if(OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.PixelShaderVersion  < D3DVS_VERSION(1,1)) {
        _WARN(this,ErrMgr->TNoSupport,"Pixel shaders version 1.1 not supported");
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

    Technique = ToClipMatrix = OutColor = 0;

    //Load the effect
    HRESULT hr = LoadFromSource("CMaterialNoOutput.fx", RenderSettings, &Effect);
    if (FAILED(hr)) {
        _WARN(this, ErrMgr->TFile, "Failed to load the shader");
    }
    else
    {
        SetFilled(true);
        Effect->FindNextValidTechnique(NULL, &Technique);
        ToClipMatrix = Effect->GetParameterBySemantic(NULL, "D3_MATRIX_TO_CLIP");
        OutColor = Effect->GetParameterBySemantic(NULL, "D3_OUTCOLOR");
    }
    _LOGE(logE,"Loaded");
    return true;
}

bool CMaterialNoOutput::Update(CRenderSettings &RenderSettings)
{
    return true;
}

bool CMaterialNoOutput::Render(CRenderSettings &RenderSettings, CMesh *pMesh, DWORD dwSubset, 
                               CBase3DObject *pObject)
{
    d3CacheInfo tmpI;
    tmpI.pMesh = pMesh;
    tmpI.Index = dwSubset;
    D3DXMatrixMultiplyTranspose(&tmpI.ToClip, &pObject->WorldTransform, &RenderSettings.CameraTransformMatrix);
    vMeshes.push_back(tmpI);
    return true;
}

bool CMaterialNoOutput::Render(CRenderSettings &RenderSettings, CBase3DObject *pObject)
{
    d3CacheInfo tmpI;
    tmpI.pMesh = pObject;
    tmpI.Index = 0xFFFFFFFF;
    D3DXMatrixMultiplyTranspose(&tmpI.ToClip, &pObject->WorldTransform, &RenderSettings.CameraTransformMatrix);
    vObjects.push_back(tmpI);
    return true;
}

bool CMaterialNoOutput::EndRendering(CRenderSettings &RenderSettings)
{
    if ((!vObjects.empty()) || (!vMeshes.empty()))
    {
        RenderSettings.Direct3DDevice->SetVertexDeclaration(VSDeclaration);

        UINT dwPassCount;
        Effect->Begin(&dwPassCount, 0);
        for (UINT i=0; i<dwPassCount; i++)
        {
            Effect->BeginPass(i);
            UINT meshcount = vMeshes.size();
            for (UINT j=0; j<meshcount; j++)
            {
                HRESULT hr = Effect->SetMatrixTranspose(ToClipMatrix, &vMeshes[j].ToClip);
                if (SUCCEEDED(hr))
                {
                    Effect->CommitChanges();
                    CMesh *pMesh = (CMesh *)vMeshes[j].pMesh;
                    pMesh->Render(RenderSettings, vMeshes[j].Index);
                }
            }
            UINT objectCount = vObjects.size();
            for (UINT j=0; j<objectCount; j++)
            {
                HRESULT hr = Effect->SetMatrixTranspose(ToClipMatrix, &vObjects[j].ToClip);
                if (SUCCEEDED(hr))
                {
                    Effect->CommitChanges();
                    CBase3DObject *pObject = (CBase3DObject *)vObjects[j].pMesh;
                    pObject->RenderGeometry(RenderSettings);
                }
            }
            Effect->EndPass();
        }
        Effect->End();

        vMeshes.clear();
        vObjects.clear();
    }
    return true;
}

bool CMaterialNoOutput::LoadXML(CXMLNode *Node, CRenderSettings &Settings)
{
    return true;
}
