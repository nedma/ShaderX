#include "stdafx.h"
#include "nommgr.h"
#include "d3dx9.h"
#include "mmgr.h"

#include "material\MAT-DX9ShadowPlain.h"
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

CDX9_MaterialShadowPlain::lightMap CDX9_MaterialShadowPlain::lightSort;
                                                                       
CDX9_MaterialShadowPlain::CDX9_MaterialShadowPlain(char *ObjName,CEngine *OwnerEngine) : CDX9Material(ObjName,OwnerEngine) {
    _LOG(this,D3_DV_GRP2_LEV0,"Creating");
    DefaultTexture = NULL;
    DefaultNormalMap = NULL;
    DefaultSpotLight = NULL;
    DiffuseTexture = NULL;
    NormalTexture = NULL;
    dwNormalID = 0;
    dwDiffuseID = 0;
}

CDX9_MaterialShadowPlain::~CDX9_MaterialShadowPlain(void) {
    _LOG(this,D3_DV_GRP2_LEV0,"Destroying");
    ReleaseBuffers();
}

bool CDX9_MaterialShadowPlain::Clone(CDX9Material **pOut) {
    CDX9Material *pMyOut = NULL;
    DWORD dwID = OwnerEngine->GetUniqueTemporaryNumber();
    CMString szName;
    szName.Format("%s_%u", GetObjName(), dwID);
    pMyOut = new CDX9_MaterialShadowPlain(szName.c_str(), OwnerEngine);
    pMyOut->SetResourceID(dwID);
    OwnerEngine->GetMaterialManager()->Add(pMyOut->GetResourceID(), *pMyOut);
    *pOut = pMyOut;
    return true;
}

void CDX9_MaterialShadowPlain::DoCacheIn(CRenderSettings &RenderSettings) {
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
}

void CDX9_MaterialShadowPlain::DoCacheOut(CRenderSettings &RenderSettings) {
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
}

void CDX9_MaterialShadowPlain::DeviceLost(CRenderSettings &Settings) {
    Effect->OnLostDevice();
    SetLost(true);
    ReleaseBuffers();
}

void CDX9_MaterialShadowPlain::ReleaseBuffers() {
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
    VSDeclaration.Release();
    Effect.Release();
    SetFilled(false);
}

bool CDX9_MaterialShadowPlain::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
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
        _WARN(this,ErrMgr->TNoSupport,"Vertex shaders version 2.0 not supported");
    }
    if (OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.PixelShaderVersion  < D3DPS_VERSION(2,0)) {
        _WARN(this,ErrMgr->TNoSupport,"Pixel shaders version 2.0 not supported");
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

    //Load the effect
    HRESULT hr = LoadFromSource("CDX9_MaterialShadowPlain.fx", RenderSettings, &Effect);
    if (FAILED(hr)) {
        _WARN(this, ErrMgr->TFile, "Failed to load the shader");
    }
    else
    {
        SetFilled(true);
        Effect->FindNextValidTechnique(NULL, &Technique);
        ToClipMatrix = Effect->GetParameterBySemantic(NULL, "D3_MATRIX_TO_CLIP");
        ToWorldMatrix = Effect->GetParameterBySemantic(NULL, "D3_MATRIX_TO_WORLD");
        ToLightMatrix = Effect->GetParameterBySemantic(NULL, "D3_MATRIX_TO_LIGHT");
        LightPosition = Effect->GetParameterBySemantic(NULL, "D3_LIGHT_POS");
        EyePosition = Effect->GetParameterBySemantic(NULL, "D3_EYE_POS");
        LightRanges = Effect->GetParameterBySemantic(NULL, "D3_LIGHT_RANGES");
        SpotlightTexture = Effect->GetParameterBySemantic(NULL, "D3_TEX_SPOT");
        ShadowMapTexture = Effect->GetParameterBySemantic(NULL, "D3_TEX_SHADOW");
    }

    //Load the textures
    DiffuseTexture = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(dwDiffuseID);
    NormalTexture = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(dwNormalID);

    _LOGE(logE,"Loaded");
    return true;
}

bool CDX9_MaterialShadowPlain::Update(CRenderSettings &RenderSettings) {
    return true;
}

bool CDX9_MaterialShadowPlain::Render(CRenderSettings &RenderSettings, CMesh *pMesh, 
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

bool CDX9_MaterialShadowPlain::Render(CRenderSettings &RenderSettings, CBase3DObject *pObject)
{
    return Render(RenderSettings, NULL, 0xFFFFFFFF, pObject);
}

bool CDX9_MaterialShadowPlain::EndRendering(CRenderSettings &RenderSettings)
{
    if (!lightSort.empty())
    {
        RenderSettings.Direct3DDevice->BeginScene();
        RenderSettings.Direct3DDevice->SetVertexDeclaration(VSDeclaration);

        D3DXVECTOR4 tmp(RenderSettings.CameraSource);
        Effect->SetVector(EyePosition, &tmp);

        lightMap::iterator tmpPos;

        Effect->SetTechnique(Technique);

        UINT dwPassCount;
        Effect->Begin(&dwPassCount, 0);

        for (tmpPos=lightSort.begin(); tmpPos!=lightSort.end(); tmpPos++)
        {
            d3lightInfo *LI = &tmpPos->second;

            //light distance ranges
            float rcpZ = 1.0f / (LI->pLight->AttenuationFarEnd - LI->pLight->AttenuationNearStart);
            //float shZAdjust = 1.0f / 100.0f;             //depth adjust
            float shZAdjust = 0.0f;             //depth adjust
            D3DXVECTOR4 Ranges(rcpZ, ((-LI->pLight->AttenuationNearStart) * rcpZ) + shZAdjust, 0, 0);
            Effect->SetVector(LightRanges, &Ranges);
            D3DXVECTOR4 tmpVec(LI->pLight->WorldObjectPos);
            Effect->SetVector(LightPosition, &tmpVec);

            if (LI->pLight->SpotlightTexture!=NULL) 
                Effect->SetTexture(SpotlightTexture, LI->pLight->SpotlightTexture->Texture);
            else 
                Effect->SetTexture(SpotlightTexture, DefaultSpotLight->Texture);

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
        }
        Effect->End();
        RenderSettings.Direct3DDevice->EndScene();
    }
    lightSort.clear();
    return true;
}

bool CDX9_MaterialShadowPlain::LoadXML(CXMLNode *Node, CRenderSettings &Settings)
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

