#include "stdafx.h"
#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"

#include "material\MAT-ShadowBufferPostProcess.h"
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

//--------------------------------------------------------------------------------------
// This is the vertex format used with the quad during post-process.
struct PPVERT
{
    float x, y, z, rhw;
    float tu, tv;       // Texcoord for post-process source

    const static D3DVERTEXELEMENT9 Decl[4];
};

// Vertex declaration for post-processing
const D3DVERTEXELEMENT9 PPVERT::Decl[4] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
    { 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  0 },
    D3DDECL_END()
};

CMaterialShadowBufferPostProcess::CMaterialShadowBufferPostProcess(char *ObjName,CEngine *OwnerEngine) : CDX9Material(ObjName,OwnerEngine) {
    _LOG(this,D3_DV_GRP2_LEV0,"Creating");
    m_dwWidth = 0;
    m_dwHeight = 0;
}

CMaterialShadowBufferPostProcess::~CMaterialShadowBufferPostProcess(void) {
    _LOG(this,D3_DV_GRP2_LEV0,"Destroying");
    DeleteShaders();
}

bool CMaterialShadowBufferPostProcess::Clone(CDX9Material **pOut) {
    CDX9Material *pMyOut = NULL;
    DWORD dwID = OwnerEngine->GetUniqueTemporaryNumber();
    CMString szName;
    szName.Format("%s_%u", GetObjName(), dwID);
    pMyOut = new CMaterialShadowBufferPostProcess(szName.c_str(), OwnerEngine);
    pMyOut->SetResourceID(dwID);
    OwnerEngine->GetMaterialManager()->Add(pMyOut->GetResourceID(), *pMyOut);
    *pOut = pMyOut;
    return true;
}

void CMaterialShadowBufferPostProcess::DeviceLost(CRenderSettings &Settings) {
    Effect->OnLostDevice();
    SetLost(true);
    DeleteShaders();
}

void CMaterialShadowBufferPostProcess::DeleteShaders() {
    VSDeclaration.Release();
    VertexBuf.Release();
    Effect.Release();
    SetFilled(false);
}

bool CMaterialShadowBufferPostProcess::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
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

    RenderSettings.Direct3DDevice->CreateVertexDeclaration(PPVERT::Decl, &VSDeclaration);

    //Load the effect
    HRESULT hr = LoadFromSource("CMaterialShadowBufferPostProcess.fx", RenderSettings, &Effect);
    if (FAILED(hr)) {
        _WARN(this, ErrMgr->TFile, "Failed to load the shader");
    }
    else
    {
        SetFilled(true);
        Effect->FindNextValidTechnique(NULL, &hTechnique);
        hShadowTexture = Effect->GetParameterByName(NULL, "tShadowTex");
    }
    _LOGE(logE,"Loaded");
    return true;
}

bool CMaterialShadowBufferPostProcess::Update(CRenderSettings &RenderSettings) {
    return true;
}

bool CMaterialShadowBufferPostProcess::Render(CRenderSettings &RenderSettings, CMesh *pMesh, 
                                   DWORD dwSubset, CBase3DObject *pObject)
{
    return true;
}

bool CMaterialShadowBufferPostProcess::Render(CRenderSettings &RenderSettings, CBase3DObject *pObject)
{
    return true;
}

bool CMaterialShadowBufferPostProcess::EndRendering(CRenderSettings &RenderSettings)
{
    HRESULT hr = S_OK;
    CComPtr<IDirect3DDevice9> spDevice = RenderSettings.Direct3DDevice;

    if (!VertexBuf)
    {
        m_dwWidth = 0;
        m_dwHeight = 0;

        // Create a vertex buffer out of the quad
        hr = spDevice->CreateVertexBuffer( sizeof(PPVERT) * 4,
            D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
            0,
            D3DPOOL_DEFAULT,
            &VertexBuf,
            NULL );
        if(FAILED(hr))
        {
            _ERROR(this, ErrMgr->TNoSupport, "Cannot create the vertex buffer");
            return false;
        }
    }

    if ((pLight) && (pLight->ShadowTexture))
    {
        DWORD dwHeight = ((CTexture2D *)pLight->ShadowTexture)->Height;
        DWORD dwWidth = ((CTexture2D *)pLight->ShadowTexture)->Width;

        if ((dwWidth != m_dwWidth) || (dwHeight != m_dwHeight))
        {
            m_dwHeight = dwHeight;
            m_dwWidth = dwWidth;

            // Set up our quad
            PPVERT Quad[4] =
            {
                { -0.5f,          -0.5f,           1.0f, 1.0f, 0.0f, 0.0f},
                { m_dwWidth-0.5f, -0.5,            1.0f, 1.0f, 1.0f, 0.0f},
                { -0.5,           m_dwHeight-0.5f, 1.0f, 1.0f, 0.0f, 1.0f},
                { m_dwWidth-0.5f, m_dwHeight-0.5f, 1.0f, 1.0f, 1.0f, 1.0f}
            };

            // Fill in the vertex buffer
            LPVOID pVBData;
            if (SUCCEEDED(VertexBuf->Lock(0, 0, &pVBData, D3DLOCK_DISCARD)))
            {
                CopyMemory(pVBData, Quad, sizeof(Quad));
                hr = VertexBuf->Unlock();
            }
        }
    }

    hr = spDevice->SetVertexDeclaration(VSDeclaration);
    hr = spDevice->SetStreamSource(0, VertexBuf, 0, sizeof(PPVERT));

    CComPtr<IDirect3DSurface9> spOrigRT;
    hr = spDevice->GetRenderTarget(0, &spOrigRT);

    CComQIPtr<IDirect3DTexture9> spShadowTex01 = pLight->ShadowTexture->Texture;
    CComQIPtr<IDirect3DTexture9> spShadowTex02 = pLight->ShadowTextureIntermediate->Texture;
    CComPtr<IDirect3DSurface9> spShadow01;
    CComPtr<IDirect3DSurface9> spShadow02;

    hr = spShadowTex01->GetSurfaceLevel(0, &spShadow01);
    if (FAILED(hr))
    {
        _ERROR(this, ErrMgr->TNoSupport, "spShadowTex01->GetSurfaceLevel(0, &spShadow01);");
        return false;
    }
    hr = spShadowTex02->GetSurfaceLevel(0, &spShadow02);
    if (FAILED(hr))
    {
        _ERROR(this, ErrMgr->TNoSupport, "spShadowTex02->GetSurfaceLevel(0, &spShadow02);");
        return false;
    }

    if (SUCCEEDED(spDevice->BeginScene()))
    {
        hr = Effect->SetTechnique(hTechnique);

        // Draw the quad
        UINT cPasses;
        hr = Effect->Begin(&cPasses, 0);
        for (UINT p=0; p<cPasses; ++p)
        {
            hr = spDevice->SetRenderTarget(0, spShadow02);
            hr = Effect->SetTexture(hShadowTexture, spShadowTex01);

            hr = spDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0L );

            hr = Effect->BeginPass(p);
            hr = Effect->CommitChanges();
            hr = spDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
            hr = Effect->EndPass();

            // flip it
            CComPtr<IDirect3DSurface9> spShadowIT;
            spShadowIT = spShadow01;
            spShadow01 = spShadow02;
            spShadow02 = spShadowIT;
            CComQIPtr<IDirect3DTexture9> spShadowTexIT;
            spShadowTexIT = spShadowTex01;
            spShadowTex01 = spShadowTex02;
            spShadowTex02 = spShadowTexIT;
        }
        hr = Effect->End();
        if (cPasses % 2 == 0)
        {
            //even number of passes - switch the texture buffers
            CBaseTexture *pOrig = pLight->ShadowTexture;
            pLight->ShadowTexture = pLight->ShadowTextureIntermediate;
            pLight->ShadowTextureIntermediate = pOrig;
        }
        hr = spDevice->SetRenderTarget(0, spOrigRT);
        hr = spDevice->EndScene();
    }
    return true;
}

bool CMaterialShadowBufferPostProcess::LoadXML(CXMLNode *Node, CRenderSettings &Settings)
{
    return true;
}
