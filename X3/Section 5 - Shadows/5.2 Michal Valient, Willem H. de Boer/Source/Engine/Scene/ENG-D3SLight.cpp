#include "stdafx.h"
#include "d3scene\eng-d3slight.h"
#include "eng-frustum.h"
#include "sc-xmlparser.h"
#include "SC-DynamicArray.h"

#include "eng-texture2d.h"
#include "ENG-TextureManager.h"
#include "ENG-Engine.h"
#include "ENG-RenderSettings.h"

#include "Material\MAT-MaterialManager.h"
#include "material\MAT-ShadowBuffer.h"
#include "material\MAT-ShadowBufferPostProcess.h"

CD3SLight::CD3SLight(char *ObjName,CEngine *OwnerEngine) : CBase3DObject(ObjName, OwnerEngine) {
    Target = NULL;

    SpotlightTexture = NULL;
    Type = TTypeOmni;
    Hotspot = 120;
    Intensity = 1;
    Contrast = 1;
    UseAttenuationNear = true;
    UseAttenuationFar = true;
    AttenuationNearStart = 0.0001f;
    AttenuationNearEnd = 0.0001f;
    AttenuationFarStart = 10000.0f;
    AttenuationFarEnd = 10000.0f;
    DecayType = TDecayNone;
    LightColor.r = LightColor.g = LightColor.b = LightColor.a = 1;
    ShadowMapSize = 256;
    Visible = false;

    LitObjects = new CRootClassArray(false);
    ShadowTexture = NULL;
    ShadowTextureIntermediate = NULL;
    RenderToShadow = NULL;
    ShadowBuffer = NULL;
    PostProcess = NULL;

    Frustum = new CFrustum(ObjName,OwnerEngine);
    SetFilled(TRUE);
    SetLost(FALSE);
}

CD3SLight::~CD3SLight(void) {
    if (ShadowTexture!=NULL) 
        OwnerEngine->GetTextureManager()->Delete(ShadowTexture->GetResourceID());
    if (ShadowTextureIntermediate!=NULL) 
        OwnerEngine->GetTextureManager()->Delete(ShadowTextureIntermediate->GetResourceID());
    if (RenderToShadow!=NULL) 
        RenderToShadow->Release();
    if (ShadowBuffer!=NULL) 
        OwnerEngine->GetMaterialManager()->Delete(ShadowBuffer->GetResourceID());
    if (PostProcess!=NULL) 
        OwnerEngine->GetMaterialManager()->Delete(PostProcess->GetResourceID());

    delete Frustum;
    delete LitObjects;
}

bool CD3SLight::RenderShadowBuffer1(CRenderSettings &Settings) {
    //Prepare render target texture
    //DWORD dwWidth = ShadowMapSize;
    DWORD dwWidth = 512;
    //DWORD dwHeight = ShadowMapSize;
    DWORD dwHeight = 512;
    //D3DFORMAT fmtTexture = D3DFMT_A32B32G32R32F;
    D3DFORMAT fmtTexture = D3DFMT_A16B16G16R16;
    //D3DFORMAT fmtTexture = D3DFMT_A8R8G8B8;
    //Prepare render target texture
    if (ShadowTexture==NULL) {
        ShadowTexture = new CTexture2D(this->GetObjName(),this->OwnerEngine);
        CTexture2D *tmpTex = (CTexture2D *)ShadowTexture;
        if (!tmpTex->CreateTexture(Settings, dwWidth, dwHeight, 1, D3DUSAGE_RENDERTARGET, 
            fmtTexture, D3DPOOL_DEFAULT)) {
                _WARN(this, ErrMgr->TNoSupport, "Failed to create shadow surface");
            }
    }
    if (ShadowTextureIntermediate==NULL) {
        ShadowTextureIntermediate = new CTexture2D(this->GetObjName(),this->OwnerEngine);
        CTexture2D *tmpTex = (CTexture2D *)ShadowTextureIntermediate;
        if (!tmpTex->CreateTexture(Settings, dwWidth, dwHeight, 1, D3DUSAGE_RENDERTARGET, 
            fmtTexture, D3DPOOL_DEFAULT)) {
                _WARN(this, ErrMgr->TNoSupport, "Failed to create shadow surface");
            }
    }
    if (ShadowBuffer==NULL) {
        ShadowBuffer = (CBaseMaterial *)OwnerEngine->GetMaterialManager()->Query(D3_GRI_Mat_ShadowBuffer);
        if (ShadowBuffer == NULL) {
            ShadowBuffer = new CMaterialShadowBuffer("ShadowBuffer",this->OwnerEngine);
            ShadowBuffer->SetResourceID(D3_GRI_Mat_ShadowBuffer);
            ShadowBuffer->Load(0, *OwnerEngine->GetRenderSettings());
            OwnerEngine->GetMaterialManager()->Add(ShadowBuffer->GetResourceID(),*ShadowBuffer);
        }
    }
    if (PostProcess==NULL) {
        PostProcess = (CBaseMaterial *)OwnerEngine->GetMaterialManager()->Query(D3_GRI_Mat_PostProcess);
        if (PostProcess == NULL) {
            PostProcess = new CMaterialShadowBufferPostProcess("ShadowBufferPostProcess",this->OwnerEngine);
            PostProcess->SetResourceID(D3_GRI_Mat_PostProcess);
            PostProcess->Load(0, *OwnerEngine->GetRenderSettings());
            OwnerEngine->GetMaterialManager()->Add(ShadowBuffer->GetResourceID(),*ShadowBuffer);
        }
    }
    if (RenderToShadow==NULL) {
        CTexture2D *tmpTex = (CTexture2D *)ShadowTexture;
        HRESULT res = D3DXCreateRenderToSurface(Settings.Direct3DDevice, tmpTex->Width, tmpTex->Height, 
            tmpTex->Format, true, D3DFMT_D24S8, &RenderToShadow);
        if (FAILED(res)) {
            _WARN(this, ErrMgr->TNoSupport, "Failed to create render to surface interface");
        }
    }
    //Do rendering
    IDirect3DTexture9 *shTexture = (IDirect3DTexture9 *)ShadowTexture->Texture;
    IDirect3DSurface9 *shSurface = NULL;
    HRESULT res = shTexture->GetSurfaceLevel(0, &shSurface);
    if(SUCCEEDED(res)) {
        res = RenderToShadow->BeginScene(shSurface, NULL);
        if(SUCCEEDED(res)) {
            CRenderSettings OldRS;
            OldRS.Clone(Settings);

            //gimmi light
            ((CMaterialShadowBuffer *)ShadowBuffer)->pLight = this;

            //Prepare frustum
            D3DXVECTOR3 Up(0.0f,1.0f,0.0f);
            Settings.CameraUpVector = Up;
            Settings.CameraSource = WorldObjectPos;
            Settings.CameraTarget = Target->WorldObjectPos;
            Settings.CameraFieldOfView = D3DXToRadian((float)Hotspot);
            Settings.CameraAspectRatio = 1.0f;
            Settings.CameraZNear = (float)AttenuationNearStart;
            Settings.CameraZFar = (float)AttenuationFarEnd;
            Settings.ComputeCameraMatrix();
//            Settings.Direct3DDevice->SetTransform(D3DTS_VIEW, &Settings.ViewMatrix);
//            Settings.Direct3DDevice->SetTransform(D3DTS_PROJECTION, &Settings.ProjectionMatrix);

            Settings.Direct3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(255, 0, 0, 255), 1.0f, 0 );

            for (int j=0; j<LitObjects->GetSize(); j++) {
                CBase3DObject *OC = (CBase3DObject *)LitObjects->GetAt(j);
                OC->PrepareGeometry(Settings);
                ShadowBuffer->Render(Settings, OC);
            }
            ShadowBuffer->EndRendering(Settings);

            RenderToShadow->EndScene(D3DX_FILTER_NONE);

            if (!(Settings.Flags & D3RS_NOSHADOWPOSTPROCESS)) {
                ((CMaterialShadowBufferPostProcess *)PostProcess)->pLight = this;
                PostProcess->EndRendering(Settings);
            }
            
            Settings.Clone(OldRS);

            /* screenshot *
            //Uncomment this code if you want to save screenshot of each depth buffer
            {
                CComQIPtr<IDirect3DTexture9> spShadowTex01 = ShadowTexture->Texture;
                CComQIPtr<IDirect3DTexture9> spShadowTex02 = ShadowTextureIntermediate->Texture;
                CComPtr<IDirect3DSurface9> spShadow01;
                CComPtr<IDirect3DSurface9> spShadow02;

                HRESULT hr = spShadowTex01->GetSurfaceLevel(0, &spShadow01);
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

                CMString OutFileName;
//                OutFileName.Format("C:\\Shadow_%s_%06i_A.dds", this->GetObjName(), Settings.RenderID);
//                D3DXSaveSurfaceToFile(OutFileName.c_str(), D3DXIFF_DDS, spShadow01, NULL, NULL);

                OutFileName.Format("C:\\Shadow_%s_%06i_B.bmp", this->GetObjName(), Settings.RenderID);
                D3DXSaveSurfaceToFile(OutFileName.c_str(), D3DXIFF_BMP, spShadow02, NULL, NULL);
                OutFileName = "";
            }
            /* screenshot */
        }
        shSurface->Release();
    }
    return true;
}

bool CD3SLight::RenderShadowBuffer(CRenderSettings &Settings) {
    if ((Settings.Flags & D3RS_SHADOWSENABLED)==D3RS_SHADOWSENABLED) 
    {
            return RenderShadowBuffer1(Settings);
    }
    return true;
}

void CD3SLight::PrepareGeometry(CRenderSettings &RenderInfo) {

    D3DXVECTOR3 Up(0.0f,1.0f,0.0f);
    D3DXMatrixLookAtLH(&SpotView, &WorldObjectPos, &Target->WorldObjectPos, &Up);
    D3DXMatrixPerspectiveFovLH(&SpotProjection, D3DXToRadian((float)Hotspot), 1, (float)AttenuationNearStart, (float)AttenuationFarEnd);
    D3DXMatrixMultiply(&SpotViewProjection, &SpotView, &SpotProjection);
    Frustum->CalculateFrustum(SpotView, SpotProjection);
}

bool CD3SLight::TestObjectLit(CBase3DObject *Obj) {
    bool result = Frustum->SphereInFrustum(Obj->WorldBSCenter, Obj->WorldBSRadius);
    if (result) _LOG(this, D3_DV_GRP2_LEV4, "Visible obj %s", Obj->GetObjName());
    else _LOG(this, D3_DV_GRP2_LEV4, "INVISIBLE obj %s", Obj->GetObjName());
    return result;
}

void CD3SLight::AddLitObject(CBase3DObject *Obj) {
    LitObjects->Add(Obj);
}

void *CD3SLight::Render(CRenderSettings &RenderSettings, void *Context, CBaseMaterial *ForcedMaterial, CBaseMaterialContext *ForcedContext) {
    return NULL;
}

void CD3SLight::PrepareBoundingGeometry(CRenderSettings &RenderInfo) {
    if (Target!=NULL) 
        Target->PrepareBoundingGeometry(RenderInfo);
    CBase3DObject::PrepareBoundingGeometry(RenderInfo);
}

void CD3SLight::DoCacheIn(CRenderSettings &RenderSettings) {
}

void CD3SLight::DoCacheOut(CRenderSettings &RenderSettings) {
}

bool CD3SLight::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
    SetFilled(TRUE);
    return TRUE;
}

bool CD3SLight::Update(CRenderSettings &RenderSettings) {
    return TRUE;
}

void CD3SLight::DeviceLost(CRenderSettings &Settings) {
    SetLost(FALSE);
}

bool CD3SLight::LoadXML(CXMLNode *Node, CRenderSettings &Settings) {
    CBase3DObject::LoadXML(Node, Settings);

    CMString tmpType = "TARGETED_SPOTLIGHT";
    Node->GetAttributeValue("LIGHTTYPE", tmpType);
    if (strcmp(tmpType.c_str(), "OMNI") == 0) {
        Type = TTypeOmni;
    } else if (strcmp(tmpType.c_str(), "TARGETED_SPOTLIGHT") == 0) {
        Type = TTypeSpot;
    } else if (strcmp(tmpType.c_str(), "DIRECTIONAL") == 0) {
        Type = TTypeDirect;
    } else if (strcmp(tmpType.c_str(), "FREE_SPOTLIGHT") == 0) {
        Type = TTypeSpot;
    } else {
        Type = TTypeDirect;
    }

    Node->GetAttributeValueF("HOTSPOT", Hotspot);
    Node->GetAttributeValueF("INTENSITY", Intensity);
    Node->GetAttributeValueF("CONTRAST", Contrast);
    Node->GetAttributeValueB("USEATTENNEAR", UseAttenuationNear);
    Node->GetAttributeValueB("USEATTENFAR", UseAttenuationFar);
    Node->GetAttributeValueF("NEARATTENSTART", AttenuationNearStart);
    Node->GetAttributeValueF("NEARATTENEND", AttenuationNearEnd);
    Node->GetAttributeValueF("FARATTENSTART", AttenuationFarStart);
    Node->GetAttributeValueF("FARATTENEND", AttenuationFarEnd);
    CMString tmpDecayType = "NONE";
    Node->GetAttributeValue("DECAYTYPE", tmpDecayType);
    if (strcmp(tmpDecayType.c_str(), "NONE") == 0) {
        DecayType = TDecayNone;
    } else if (strcmp(tmpDecayType.c_str(), "INVERSE") == 0) {
        DecayType = TDecayInverse;
    } else {
        DecayType = TDecayInverseSquare;
    }

    Node->GetAttributeValueI("MAPSIZE", ShadowMapSize);

    Node->GetAttributeValueI("TARGETID", TargetID);
    CXMLNode *tmpColor = Node->GetSubNode(Node->FindSubNode("COLOR",0));
    if (tmpColor) {
        LightColor.r = LightColor.g = LightColor.b = LightColor.a = 1.0f;
        tmpColor->GetAttributeValueF("X", LightColor.r);
        tmpColor->GetAttributeValueF("Y", LightColor.g);
        tmpColor->GetAttributeValueF("Z", LightColor.b);
    } else {
        _WARN(this, ErrMgr->TNoSupport, "No 'COLOR' node found in light.");
    }
    return true;
}
