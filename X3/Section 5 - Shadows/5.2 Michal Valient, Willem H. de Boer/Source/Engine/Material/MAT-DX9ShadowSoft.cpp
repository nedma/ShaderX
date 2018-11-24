#include "stdafx.h"
#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"

#include "material\MAT-DX9ShadowSoft.h"
#include "material\MAT-MaterialManager.h"

#include "ENG-RenderSettings.h"
#include "ENG-Engine.h"
#include "eng-enginedefaults.h"

#include "ENG-Texture2D.h"
#include "ENG-TextureManager.h"
#include "ENG-BaseTexture.h"

#include "ENG-Base3DObject.h"
#include "d3scene\eng-d3slight.h"

#include "eng-mesh.h"

#include "MGR-DirectGraphicsManager.h"
#include "DG-DirectGraphicsDeviceInfo.h"
#include "WND-WindowManager.h"
#include "FIL-FileManager.h"

#include "SC-MString.h"
#include "sc-dynamicarray.h"
#include "sc-xmlparser.h"

#include "Shader\SHD-ShaderManager.h"
#include "shader\shd-vertexshader.h"
#include "shader\shd-pixelshader.h"

//--------------------------------------------------------------------------------------
// This is the vertex format used with the quad during post-process.
struct PPVERT2
{
    float x, y, z, rhw;
    float tu, tv;       // Texcoord for post-process source
    float tu2, tv2;       // Texcoord for post-process source

    const static D3DVERTEXELEMENT9 Decl[4];
};

// Vertex declaration for post-processing
const D3DVERTEXELEMENT9 PPVERT2::Decl[4] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
    { 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  0 },
    { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  1 },
    D3DDECL_END()
};

CDX9_MaterialShadowSoft::lightMap CDX9_MaterialShadowSoft::lightSort;
CComPtr<IDirect3DTexture9> CDX9_MaterialShadowSoft::spRTTex;
CComPtr<ID3DXRenderToSurface> CDX9_MaterialShadowSoft::spRenderToTex;

bool CDX9_MaterialShadowSoft::bUsePureRandom = false;
bool CDX9_MaterialShadowSoft::bUse1D = false;

CDX9_MaterialShadowSoft::CDX9_MaterialShadowSoft(char *ObjName,CEngine *OwnerEngine) : CDX9Material(ObjName,OwnerEngine) {
    _LOG(this,D3_DV_GRP2_LEV0,"Creating");
    DefaultTexture = NULL;
    DefaultNormalMap = NULL;
    DefaultSpotLight = NULL;
    DiffuseTexture = NULL;
    NormalTexture = NULL;
    NoiseTexture = NULL;
    dwNormalID = 0;
    dwDiffuseID = 0;
}

CDX9_MaterialShadowSoft::~CDX9_MaterialShadowSoft(void) {
    _LOG(this,D3_DV_GRP2_LEV0,"Destroying");
    ReleaseBuffers();
}

bool CDX9_MaterialShadowSoft::Clone(CDX9Material **pOut) {
    CDX9Material *pMyOut = NULL;
    DWORD dwID = OwnerEngine->GetUniqueTemporaryNumber();
    CMString szName;
    szName.Format("%s_%u", GetObjName(), dwID);
    pMyOut = new CDX9_MaterialShadowSoft(szName.c_str(), OwnerEngine);
    pMyOut->SetResourceID(dwID);
    OwnerEngine->GetMaterialManager()->Add(pMyOut->GetResourceID(), *pMyOut);
    *pOut = pMyOut;
    return true;
}

void CDX9_MaterialShadowSoft::DoCacheIn(CRenderSettings &RenderSettings) {
    if (DefaultTexture!=NULL) 
        DefaultTexture->CacheIn(RenderSettings);
    if (DefaultSpotLight!=NULL) 
        DefaultSpotLight->CacheIn(RenderSettings);
    if (DefaultNormalMap!=NULL) 
        DefaultNormalMap->CacheIn(RenderSettings);
    if (DiffuseTexture!=NULL) 
        DiffuseTexture->CacheIn(RenderSettings);
    if (NormalTexture!=NULL) 
        NormalTexture->CacheIn(RenderSettings);
    if (NoiseTexture!=NULL)
        NoiseTexture->CacheIn(RenderSettings);
}

void CDX9_MaterialShadowSoft::DoCacheOut(CRenderSettings &RenderSettings) {
    if (DefaultTexture!=NULL) 
        DefaultTexture->CacheOut(RenderSettings);
    if (DefaultSpotLight!=NULL) 
        DefaultSpotLight->CacheOut(RenderSettings);
    if (DefaultNormalMap!=NULL) 
        DefaultNormalMap->CacheOut(RenderSettings);
    if (DiffuseTexture!=NULL) 
        DiffuseTexture->CacheOut(RenderSettings);
    if (NormalTexture!=NULL) 
        NormalTexture->CacheOut(RenderSettings);
    if (NoiseTexture)
        NoiseTexture->CacheOut(RenderSettings);
    spRTTex.Release();
}

void CDX9_MaterialShadowSoft::DeviceLost(CRenderSettings &Settings) {
    Effect->OnLostDevice();
    SetLost(true);
    ReleaseBuffers();
}

void CDX9_MaterialShadowSoft::ReleaseBuffers() {
    if (DefaultTexture!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(DefaultTexture->GetResourceID());
        DefaultTexture = NULL;
    }
    if (DefaultSpotLight!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(DefaultSpotLight->GetResourceID());
        DefaultSpotLight = NULL;
    }
    if (DefaultNormalMap!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(DefaultNormalMap->GetResourceID());
        DefaultNormalMap = NULL;
    }
    if (DiffuseTexture!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(DiffuseTexture->GetResourceID());
        DiffuseTexture = NULL;
    }
    if (NormalTexture!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(NormalTexture->GetResourceID());
        NormalTexture = NULL;
    }
    if (NoiseTexture!=NULL) {
        OwnerEngine->GetTextureManager()->Delete(NoiseTexture->GetResourceID());
        NoiseTexture = NULL;
    }

    VSAccumDeclaration.Release();
    AccumVertexBuf.Release();

    spRTTex.Release();
    spRenderToTex.Release();
    VSDeclaration.Release();
    Effect.Release();
    SetFilled(false);
}

bool CDX9_MaterialShadowSoft::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV3,"Loading material");
    ReleaseBuffers();

    //shader input declaration
    D3DVERTEXELEMENT9  dwDecl[] = {
        { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 24,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 32,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
        D3DDECL_END()
    };
    RenderSettings.Direct3DDevice->CreateVertexDeclaration(dwDecl, &VSDeclaration);

    _LOG(this,D3_DV_GRP2_LEV3,"Loading PS_2_0 version");
    if (OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.VertexShaderVersion < D3DVS_VERSION(2,0)) {
        _ERROR(this,ErrMgr->TNoSupport,"Vertex shaders version 2.0 not supported");
    }
    if (OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.PixelShaderVersion  < D3DPS_VERSION(2,0)) {
        _ERROR(this,ErrMgr->TNoSupport,"Pixel shaders version 2.0 not supported");
    }

    //Load default projector texture
    DefaultSpotLight = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(D3_GRI_Tex_DefaultSpotLight);
    if (DefaultSpotLight==NULL) {
        //CMString LMFile = "D3F_DefaultSpotlight.bmp";
        CMString LMFile = "D3F_DefaultSpotlight2.dds";
        DefaultSpotLight = new CTexture2D(LMFile.c_str(),OwnerEngine);
        DefaultSpotLight->SetResourceID(D3_GRI_Tex_DefaultSpotLight);
        int resindex = OwnerEngine->GetFileManager()->Find(LMFile);
        DefaultSpotLight->Load(resindex,RenderSettings);
        OwnerEngine->GetTextureManager()->Add(DefaultSpotLight->GetResourceID(),*DefaultSpotLight);
    }

    //Load default diffuse texture
    DefaultTexture = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(D3_GRI_Tex_Gray);
    if (DefaultTexture==NULL) {
        CMString LMFile = "D3F_DefaultTexture.bmp";
        DefaultTexture = new CTexture2D(LMFile.c_str(),OwnerEngine);
        DefaultTexture->SetResourceID(D3_GRI_Tex_Gray);
        int resindex = OwnerEngine->GetFileManager()->Find(LMFile);
        DefaultTexture->Load(resindex,RenderSettings);
        OwnerEngine->GetTextureManager()->Add(DefaultTexture->GetResourceID(),*DefaultTexture);
    }

    //Load default normal map texture
    DefaultNormalMap = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(D3_GRI_Tex_DefaultNotmalMap);
    if (DefaultNormalMap==NULL) {
        CMString LMFile = "D3F_DefaultBlack.bmp";
        CTexture2D *TempPointer = new CTexture2D(LMFile.c_str(),OwnerEngine);
        TempPointer->SetResourceID(D3_GRI_Tex_DefaultNotmalMap);
        int resindex = OwnerEngine->GetFileManager()->Find(LMFile);
        TempPointer->Load(resindex,RenderSettings);
        TempPointer->ConvertToNormalMap(RenderSettings);
        DefaultNormalMap = TempPointer;
        OwnerEngine->GetTextureManager()->Add(DefaultNormalMap->GetResourceID(),*DefaultNormalMap);
    }

    //Load noise map texture
    NoiseTexture = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(D3_GRI_Tex_CircularNoise);
    if (NoiseTexture==NULL) {
        CMString LMFile = "D3F_CircularNoiseSigned.dds";
        if (bUsePureRandom)
            LMFile = "D3F_CircularPureNoiseSigned.dds";
        else if (bUse1D)
            LMFile = "D3F_CircularNoiseSigned1D.dds";
        CTexture2D *TempPointer = new CTexture2D(LMFile.c_str(),OwnerEngine);
        TempPointer->SetResourceID(D3_GRI_Tex_CircularNoise);
        int resindex = OwnerEngine->GetFileManager()->Find(LMFile);
        TempPointer->Load(resindex,RenderSettings);
        //TempPointer->ConvertToNormalMap(RenderSettings);
        NoiseTexture = TempPointer;
        OwnerEngine->GetTextureManager()->Add(NoiseTexture->GetResourceID(),*NoiseTexture);
    }

    //Load the effect
    HRESULT hr = LoadFromSource("CDX9_MaterialShadowSoft.fx", RenderSettings, &Effect);
//    HRESULT hr = LoadFromSource("CDX9_MaterialShadowSoft.fx.fxo", RenderSettings, &Effect);
    if (FAILED(hr)) {
        _WARN(this, ErrMgr->TFile, "Failed to load the shader");
    }
    else
    {
        SetFilled(true);
        Effect->FindNextValidTechnique(NULL, &Technique);
        Technique = Effect->GetTechniqueByName("T_CDX9_MaterialShadowPlain20");
        if (RenderSettings.Flags & D3RS_NOSHADOWPOSTPROCESS) {
            Technique = Effect->GetTechniqueByName("T_CDX9_MaterialShadow_ConstantOuter");
        }

        ToClipMatrix = Effect->GetParameterBySemantic(NULL, "D3_MATRIX_TO_CLIP");
        ToWorldMatrix = Effect->GetParameterBySemantic(NULL, "D3_MATRIX_TO_WORLD");
        ToLightMatrix = Effect->GetParameterBySemantic(NULL, "D3_MATRIX_TO_LIGHT");
        LightPosition = Effect->GetParameterBySemantic(NULL, "D3_LIGHT_POS");
        EyePosition = Effect->GetParameterBySemantic(NULL, "D3_EYE_POS");
        LightRanges = Effect->GetParameterBySemantic(NULL, "D3_LIGHT_RANGES");
        SpotlightTexture = Effect->GetParameterBySemantic(NULL, "D3_TEX_SPOT");
        ShadowMapTexture = Effect->GetParameterBySemantic(NULL, "D3_TEX_SHADOW");
        hNoiseTexture = Effect->GetParameterBySemantic(NULL, "D3_TEX_NOISE");
    }

    //Load the textures
    DiffuseTexture = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(dwDiffuseID);
    NormalTexture = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(dwNormalID);

    Effect->SetTexture(hNoiseTexture, NoiseTexture->Texture);

    //Shadow accum code
    hAccumSource = Effect->GetParameterBySemantic(NULL, "D3_TEX_ACCUMSRC");
    hAccumTechnique = Effect->GetTechniqueByName("D3_AccumTechnique");
    if (bUse1D)
        hAccumTechnique = Effect->GetTechniqueByName("D3_AccumTechnique1D");

    //Phong rendering code
    hPhongTechnique = Effect->GetTechniqueByName("D3_PhongTechnique");

    _LOGE(logE,"Loaded");
    return true;
}

bool CDX9_MaterialShadowSoft::Update(CRenderSettings &RenderSettings) {
    return true;
}

bool CDX9_MaterialShadowSoft::Render(CRenderSettings &RenderSettings, CMesh *pMesh, 
                                      DWORD dwSubset, CBase3DObject *pObject)
{
    //Fill the lights to the hash table.
    for (int i=0; i<pObject->Lights->GetSize(); i++)
    {
        CD3SLight *pLight = (CD3SLight *)pObject->Lights->GetAt(i);
        lightMap::iterator tmpPos = lightSort.find(pLight->GetResourceID());
        if (tmpPos == lightSort.end()) {
            //it is a new one
            d3lightInfo myLight;
            myLight.pLight = pLight;
            lightSort[pLight->GetResourceID()] = myLight;
            tmpPos = lightSort.find(pLight->GetResourceID());
        }
        d3lightInfo *LI = &tmpPos->second;
        d3CacheInfo CI;
        CI.Index = dwSubset;
        CI.pMesh = pMesh;
        CI.pObject = pObject;

        D3DXMATRIX tmp;
        // world * view * proj matrix
        D3DXMatrixMultiplyTranspose(&tmp, &pObject->WorldTransform, &RenderSettings.CameraTransformMatrix);
        CI.ToClip = tmp;

        // world matrix
        D3DXMatrixTranspose(&tmp, &pObject->WorldTransform);
        CI.ToWorld = tmp;

        LI->objects.push_back(CI);
    }
    return true;
}

bool CDX9_MaterialShadowSoft::Render(CRenderSettings &RenderSettings, CBase3DObject *pObject)
{
    return Render(RenderSettings, NULL, 0xFFFFFFFF, pObject);
}

bool CDX9_MaterialShadowSoft::EndRendering(CRenderSettings &RenderSettings)
{
    if (!lightSort.empty())
    {
        //Initialize the backbuffer thingy if needed
        {
            if ((!spRTTex) || (!spRenderToTex) || (!VSAccumDeclaration))
            {
                CComPtr<IDirect3DSurface9> spTmp;
                RenderSettings.Direct3DDevice->GetRenderTarget(0, &spTmp);
                D3DSURFACE_DESC Desc;
                spTmp->GetDesc(&Desc);
                DWORD dwWidth = Desc.Width;
                DWORD dwHeight = Desc.Height;
                D3DFORMAT fmtTexture = D3DFMT_A16B16G16R16;
                if (!spRTTex) {
                    HRESULT hRes = D3DXCreateTexture(RenderSettings.Direct3DDevice, dwWidth, dwHeight, 1, D3DUSAGE_RENDERTARGET, fmtTexture, D3DPOOL_DEFAULT, &spRTTex);
                    if (FAILED(hRes))
                        _ERROR(this, ErrMgr->TNoSupport, "Failed to create render target surface");
                }
                if (!spRenderToTex)
                {
                    HRESULT hRes = D3DXCreateRenderToSurface(RenderSettings.Direct3DDevice, dwWidth, dwHeight, fmtTexture, true, D3DFMT_D24S8, &spRenderToTex);
                    if (FAILED(hRes))
                        _ERROR(this, ErrMgr->TNoSupport, "Failed to create render to surface facity");
                }
                if (!VSAccumDeclaration)
                {
                    if (FAILED(RenderSettings.Direct3DDevice->CreateVertexDeclaration(PPVERT2::Decl, &VSAccumDeclaration)))
                        _ERROR(this, ErrMgr->TNoSupport, "Failed to generate vertex declaration");
                }
            }
        }

        RenderSettings.Direct3DDevice->SetVertexDeclaration(VSDeclaration);

        Effect->SetTechnique(Technique);

        D3DXVECTOR4 tmp(RenderSettings.CameraSource);
        Effect->SetVector(EyePosition, &tmp);

        lightMap::iterator tmpPos;

        for (tmpPos=lightSort.begin(); tmpPos!=lightSort.end(); tmpPos++)
        {
            d3lightInfo *LI = &tmpPos->second;

            //############################################
            //First step - render the shadow position info
            //This means that we have to render whole scene 
            //into the texture where we store shadow map 
            //position, depth and shadow result
            //############################################
            
            //We have to remove the old texture
            HRESULT res = S_OK;

            //Setup the rendertarget stuff
            CComPtr<IDirect3DSurface9> shSurface;
            res = spRTTex->GetSurfaceLevel(0, &shSurface);
            ATLASSERT(shSurface);
            ATLASSERT(SUCCEEDED(res));

            CComPtr<IDirect3DSurface9> shOrigRT;
            res = RenderSettings.Direct3DDevice->GetRenderTarget(0, &shOrigRT);
            ATLASSERT(SUCCEEDED(res));

            res = RenderSettings.Direct3DDevice->SetRenderTarget(0, shSurface);
            ATLASSERT(SUCCEEDED(res));

            res = RenderSettings.Direct3DDevice->BeginScene();
            //res = spRenderToTex->BeginScene(shSurface, NULL);
            ATLASSERT(SUCCEEDED(res));
            res = RenderSettings.Direct3DDevice->Clear(0, NULL, 
                                                        D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL | D3DCLEAR_TARGET, 
                                                        0x00000000, 1.0f, 0);
            ATLASSERT(SUCCEEDED(res));
            UINT dwPassCount;
            Effect->Begin(&dwPassCount, 0);

            //light distance ranges
            float rcpZ = 1.0f / (LI->pLight->AttenuationFarEnd - LI->pLight->AttenuationNearStart);
            //float shZAdjust = 1.0f / 100.0f;             //depth adjust
            float shZAdjust = 0.0f;             //depth adjust
            D3DXVECTOR4 Ranges(rcpZ, ((-LI->pLight->AttenuationNearStart) * rcpZ) + shZAdjust, 0, 0);
            Effect->SetVector(LightRanges, &Ranges);
            D3DXVECTOR4 tmpVec(LI->pLight->WorldObjectPos);
            Effect->SetVector(LightPosition, &tmpVec);

            if (LI->pLight->ShadowTexture!=NULL) 
                Effect->SetTexture(ShadowMapTexture, LI->pLight->ShadowTexture->Texture);
            else 
                Effect->SetTexture(ShadowMapTexture, DefaultTexture->Texture);

            for (DWORD i=0; i<dwPassCount; i++)
            {
                Effect->BeginPass(i);
                UINT objectCount = LI->objects.size();
                for (UINT j=0; j<objectCount; j++)
                {
                    Effect->SetMatrixTranspose(ToClipMatrix, &LI->objects[j].ToClip);
                    Effect->SetMatrixTranspose(ToWorldMatrix, &LI->objects[j].ToWorld);

                    // setup texgen, y is inverted
                    D3DXMATRIX TexGenMatrix(0.5f, 0.0f, 0.0f, 0.5f,
                        0.0f,-0.5f, 0.0f, 0.5f,
                        0.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f);

                    D3DXMATRIX ObjectToLight;
                    D3DXMatrixMultiplyTranspose(&ObjectToLight, &LI->objects[j].pObject->WorldTransform, 
                        &LI->pLight->SpotViewProjection);
                    D3DXMatrixMultiply(&ObjectToLight, &TexGenMatrix, &ObjectToLight);

                    Effect->SetMatrixTranspose(ToLightMatrix, &ObjectToLight);

                    Effect->CommitChanges();
                    if (LI->objects[j].pMesh)
                    {
                        LI->objects[j].pMesh->Render(RenderSettings, LI->objects[j].Index);
                    }
                    else
                    {
                        LI->objects[j].pObject->RenderGeometry(RenderSettings);
                    }
                }
                Effect->EndPass();
            }
            Effect->End();
            RenderSettings.Direct3DDevice->EndScene();
            //spRenderToTex->EndScene(D3DX_FILTER_NONE);

            res = RenderSettings.Direct3DDevice->SetRenderTarget(0, shOrigRT);
            ATLASSERT(SUCCEEDED(res));


            /* screenshot *
            //Uncomment this code if you want to save screenshot of frame buffer
            {
                CMString OutFileName;
                HRESULT hResX = S_OK;
                //OutFileName.Format("C:\\Shadow_%s_%06i_A.dds", this->GetObjName(), RenderSettings.RenderID);
                //hResX = D3DXSaveTextureToFile(OutFileName.c_str(), D3DXIFF_DDS, spRTTex, NULL);

                OutFileName.Format("C:\\SoftProcess_%s_%06i_B.bmp", this->GetObjName(), RenderSettings.RenderID);
                hResX = D3DXSaveTextureToFile(OutFileName.c_str(), D3DXIFF_BMP, spRTTex, NULL);
                OutFileName = "";
            }
            /* screenshot */

            RenderSettings.Direct3DDevice->BeginScene();
            //#############################################
            //Second step - run the image space pass
            // - we render the soft shadow values to the screen
            // into the alpha buffer.
            //
            //#############################################
            AccumShadows(RenderSettings, LI->pLight);

            //#############################################
            //Third step - render standard phong lighting 
            //and use the previously computed shadow data
            //
            //#############################################
            RenderPhong(RenderSettings, LI);

            RenderSettings.Direct3DDevice->EndScene();
        }
    }
    lightSort.clear();
    return true;
}

bool CDX9_MaterialShadowSoft::RenderPhong(CRenderSettings &RenderSettings, d3lightInfo *LI)
{
    RenderSettings.Direct3DDevice->SetVertexDeclaration(VSDeclaration);

    Effect->SetTechnique(hPhongTechnique);

    D3DXVECTOR4 tmp(RenderSettings.CameraSource);
    Effect->SetVector(EyePosition, &tmp);

    HRESULT res = S_OK;

    UINT dwPassCount;
    Effect->Begin(&dwPassCount, 0);

    if (LI->pLight->SpotlightTexture!=NULL) 
        Effect->SetTexture(SpotlightTexture, LI->pLight->SpotlightTexture->Texture);
    else 
        Effect->SetTexture(SpotlightTexture, DefaultSpotLight->Texture);

    for (DWORD i=0; i<dwPassCount; i++)
    {
        Effect->BeginPass(i);
        UINT objectCount = LI->objects.size();
        for (UINT j=0; j<objectCount; j++)
        {
            Effect->SetMatrixTranspose(ToClipMatrix, &LI->objects[j].ToClip);
            Effect->SetMatrixTranspose(ToWorldMatrix, &LI->objects[j].ToWorld);

            // setup texgen, y is inverted
            D3DXMATRIX TexGenMatrix(0.5f, 0.0f, 0.0f, 0.5f,
                0.0f,-0.5f, 0.0f, 0.5f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);

            D3DXMATRIX ObjectToLight;
            D3DXMatrixMultiplyTranspose(&ObjectToLight, &LI->objects[j].pObject->WorldTransform, 
                &LI->pLight->SpotViewProjection);
            D3DXMatrixMultiply(&ObjectToLight, &TexGenMatrix, &ObjectToLight);

            Effect->SetMatrixTranspose(ToLightMatrix, &ObjectToLight);

            Effect->CommitChanges();
            if (LI->objects[j].pMesh)
            {
                LI->objects[j].pMesh->Render(RenderSettings, LI->objects[j].Index);
            }
            else
            {
                LI->objects[j].pObject->RenderGeometry(RenderSettings);
            }
        }
        Effect->EndPass();
    }
    Effect->End();
    return true;
}

bool CDX9_MaterialShadowSoft::AccumShadows(CRenderSettings &RenderSettings, 
                                           CD3SLight *pLight)
{
    HRESULT hr = S_OK;
    CComPtr<IDirect3DDevice9> spDevice = RenderSettings.Direct3DDevice;

    if (!AccumVertexBuf)
    {
        // Create a vertex buffer out of the quad
        hr = spDevice->CreateVertexBuffer( sizeof(PPVERT2) * 4,
                                            D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
                                            0,
                                            D3DPOOL_DEFAULT,
                                            &AccumVertexBuf,
                                            NULL );
        if(FAILED(hr))
        {
            _ERROR(this, ErrMgr->TNoSupport, "Cannot create the vertex buffer");
            return false;
        }

        D3DSURFACE_DESC Desc;
        spRTTex->GetLevelDesc(0, &Desc);
        DWORD dwHeight = Desc.Height;
        DWORD dwWidth = Desc.Width;

        float fNoiseX = float(dwWidth) / float(((CTexture2D *)NoiseTexture)->Width);
        float fNoiseY = float(dwHeight) / float(((CTexture2D *)NoiseTexture)->Height);

        // Set up our quad
        PPVERT2 Quad[4] =
        {
            { -0.5f,        -0.5f,           1.0f, 1.0f, 0.0f, 0.0f, 0.0f,      0.0f},
            { dwWidth-0.5f, -0.5,            1.0f, 1.0f, 1.0f, 0.0f, fNoiseX,   0.0f},
            { -0.5,           dwHeight-0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,      fNoiseY},
            { dwWidth-0.5f,   dwHeight-0.5f, 1.0f, 1.0f, 1.0f, 1.0f, fNoiseX,   fNoiseY}
        };

        // Fill in the vertex buffer
        LPVOID pVBData;
        if (SUCCEEDED(AccumVertexBuf->Lock(0, 0, &pVBData, D3DLOCK_DISCARD)))
        {
            CopyMemory(pVBData, Quad, sizeof(Quad));
            hr = AccumVertexBuf->Unlock();
        }
    }

    hr = spDevice->SetVertexDeclaration(VSAccumDeclaration);
    hr = spDevice->SetStreamSource(0, AccumVertexBuf, 0, sizeof(PPVERT2));

    hr = Effect->SetTechnique(hAccumTechnique);

    hr = Effect->SetTexture(hAccumSource, spRTTex);

    // Draw the quad
    UINT cPasses;
    hr = Effect->Begin(&cPasses, 0);
    hr = Effect->CommitChanges();
    for (UINT p=0; p<cPasses; ++p)
    {
        hr = Effect->BeginPass(p);
        hr = spDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
        hr = Effect->EndPass();
    }
    hr = Effect->End();

    hr = Effect->SetTexture(hAccumSource, NULL);
    ATLASSERT(SUCCEEDED(hr));

    return true;
}


bool CDX9_MaterialShadowSoft::LoadXML(CXMLNode *Node, CRenderSettings &Settings)
{
    SpecularColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    CXMLNode *tmpNode = Node->GetSubNode(Node->FindSubNode("SPECULARCOLOR", 0));
    if (tmpNode!=NULL) {
        tmpNode->GetAttributeValueF("X", SpecularColor.r);
        tmpNode->GetAttributeValueF("Y", SpecularColor.g);
        tmpNode->GetAttributeValueF("Z", SpecularColor.b);
    }

    SpecularShininess = 0.3f;
    Node->GetAttributeValueF("SHININESS", SpecularShininess);

    int TexID = -1;
    Node->GetAttributeValueI("BUMPMAPID", TexID);
    NormalTexture = (CBaseTexture *)Settings.Engine->GetTextureManager()->Query(TexID);
    dwNormalID = TexID;

    DiffuseColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    tmpNode = Node->GetSubNode(Node->FindSubNode("DIFFUSECOLOR", 0));
    if (tmpNode!=NULL) {
        tmpNode->GetAttributeValueF("X", DiffuseColor.r);
        tmpNode->GetAttributeValueF("Y", DiffuseColor.g);
        tmpNode->GetAttributeValueF("Z", DiffuseColor.b);
    }

    TexID = -1;
    Node->GetAttributeValueI("DIFFUSEMAPID", TexID);
    DiffuseTexture = (CBaseTexture *)Settings.Engine->GetTextureManager()->Query(TexID);
    dwDiffuseID = TexID;
    return true;
}
