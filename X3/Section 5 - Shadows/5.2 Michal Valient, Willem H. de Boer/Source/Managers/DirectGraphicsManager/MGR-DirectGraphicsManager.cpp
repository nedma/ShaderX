// MGR-DirectGraphicsManager.cpp: implementation of the DirectGraphicsManager class.
//
//////////////////////////////////////////////////////////////////////

//TODO: presunut guard buffers

#include "stdafx.h"
#include "MGR-DirectGraphicsManager.h"
#include "ENG-Engine.h"
#include "SC-MString.h"
//#include "ENG-TextureManager.h"
#include "DG-DirectGraphicsDeviceInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirectGraphicsManager::CDirectGraphicsManager(char *ObjName,CEngine *OwnerEngine) : CBaseManager(ObjName,OwnerEngine) {
    _LOG(this,D3_DV_GRP0_LEV0,"Creating");
    Direct3DDevice=NULL;
    Direct3D=NULL;
    SelectedDeviceInfo=NULL;
}

CDirectGraphicsManager::~CDirectGraphicsManager() {
    int logE = _LOGB(this,D3_DV_GRP0_LEV0,"Destroying");
    Release();
    _LOGE(logE,"Destroyed");
}

void CDirectGraphicsManager::Release() {
    int LogE = _LOGB(this,D3_DV_GRP0_LEV0,"Releasing Contents");
    if (Direct3DDevice!=NULL) Direct3DDevice->Release();
    if (Direct3D!=NULL) Direct3D->Release();
    if (SelectedDeviceInfo!=NULL) delete SelectedDeviceInfo;
    SelectedDeviceInfo=NULL;
    _LOGE(LogE,"Released");
}

bool CDirectGraphicsManager::CreateDirect3D() {
    _LOG(this,D3_DV_GRP0_LEV0,"Creating Direct3D Object");
    if (Direct3D!=NULL) {
        _WARN(this,ErrMgr->TSystem,"Cannot create Direct3D object before releasing previous");
        return FALSE;
    } else {
        Direct3D = Direct3DCreate9(D3D_SDK_VERSION);
        if (Direct3D==NULL) {
            _FATAL(this,ErrMgr->TSystem,"Cannot create Direct3D object");
            return FALSE;
        } else {
            _LOG(this,D3_DV_GRP0_LEV0,"Direct3D Object created, Built with DirectX SDK Version %d",D3D_SDK_VERSION);
            return TRUE;
        }
    }
}

CDirectGraphicsDeviceInfo *CDirectGraphicsManager::GetDeviceInfo(UINT Adapter, D3DDEVTYPE DevType) {
    if (Direct3D==NULL) {
        _WARN(this,ErrMgr->TSystem,"Direct3D object not yet created");
        return NULL;
    } else {
        _LOG(this,D3_DV_GRP0_LEV2,"Getting device info for adapter %d , device %d",Adapter,DevType);
		CMString tmp = this->ObjectName;
		tmp+="_DevInfo";
        CDirectGraphicsDeviceInfo *ret = new CDirectGraphicsDeviceInfo(tmp.c_str(),OwnerEngine);
        if (!ret->GetInfo(*Direct3D,Adapter,DevType)) {
            delete ret;
            ret=NULL;
        }
        return ret;
    }
}

bool CDirectGraphicsManager::CreateDirect3DDevice(UINT Adapter, D3DDEVTYPE DevType, HWND  FocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* PresentationParameters) {
    if (Direct3D==NULL) {
        _WARN(this,ErrMgr->TSystem,"Direct3D object not yet created");
        return NULL;
    } else {
        _LOG(this,D3_DV_GRP0_LEV0,"Creating Direct3D device for adapter %d , device %d",Adapter,DevType);
        if (SelectedDeviceInfo!=NULL) {
            delete SelectedDeviceInfo;
            SelectedDeviceInfo=NULL;
        }
        UINT Count = PresentationParameters->BackBufferCount;
        HRESULT ret = Direct3D->CreateDevice(Adapter,DevType,FocusWindow,BehaviorFlags,PresentationParameters,&Direct3DDevice);
        if (ret!=D3D_OK) {
            if (PresentationParameters->BackBufferCount!=Count) {
                _WARN(this,ErrMgr->TNoSupport,"You specified %d of backbuffers, device supports only %d. Recreating",Count,PresentationParameters->BackBufferCount);
                ret = Direct3D->CreateDevice(Adapter,DevType,FocusWindow,BehaviorFlags,PresentationParameters,&Direct3DDevice);
            }
            if (ret!=D3D_OK) {
                _ERROR(this,ErrMgr->TSystem,"Cannot create Direct3DDevice");
                return FALSE;
            }
        }
        SelectedDeviceInfo = GetDeviceInfo(Adapter, DevType);
        if (SelectedDeviceInfo==NULL) {
            _WARN(this,ErrMgr->TNoSupport,"Cannot get device info");
        }
        this->Adapter = Adapter;
        this->DevType = DevType;
        if (PresentationParameters->hDeviceWindow!=NULL) Window = PresentationParameters->hDeviceWindow;
        else Window = FocusWindow;
        StartBehaviorFlags = BehaviorFlags;
        memcpy(&StartPresentationParameters,PresentationParameters,sizeof(D3DPRESENT_PARAMETERS));

        SetDefaultRenderStates();
        return TRUE;
    }
}

void CDirectGraphicsManager::SetDefaultRenderStates() {
    //Set D3Device state defaults
    Direct3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE );
    Direct3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    Direct3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    Direct3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    Direct3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);   //Other alpha test states are not set to any default due to this disable
    Direct3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    Direct3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
    Direct3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW );
    Direct3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    Direct3DDevice->SetRenderState(D3DRS_DITHERENABLE, FALSE);
    Direct3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    Direct3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);     //Other fog states are disabled due to this
    Direct3DDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
    Direct3DDevice->SetRenderState(D3DRS_DEPTHBIAS, 0);
    Direct3DDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);    //Other stencil states are disabled due to this
    Direct3DDevice->SetRenderState(D3DRS_WRAP0, 0);
    Direct3DDevice->SetRenderState(D3DRS_WRAP1, 0);
    Direct3DDevice->SetRenderState(D3DRS_WRAP2, 0);
    Direct3DDevice->SetRenderState(D3DRS_WRAP3, 0);
    Direct3DDevice->SetRenderState(D3DRS_WRAP4, 0);
    Direct3DDevice->SetRenderState(D3DRS_WRAP5, 0);
    Direct3DDevice->SetRenderState(D3DRS_WRAP6, 0);
    Direct3DDevice->SetRenderState(D3DRS_WRAP7, 0);
    Direct3DDevice->SetRenderState(D3DRS_WRAP8, 0);
    Direct3DDevice->SetRenderState(D3DRS_WRAP9, 0);
    Direct3DDevice->SetRenderState(D3DRS_WRAP10, 0);
    Direct3DDevice->SetRenderState(D3DRS_WRAP11, 0);
    Direct3DDevice->SetRenderState(D3DRS_WRAP12, 0);
    Direct3DDevice->SetRenderState(D3DRS_WRAP13, 0);
    Direct3DDevice->SetRenderState(D3DRS_WRAP14, 0);
    Direct3DDevice->SetRenderState(D3DRS_WRAP15, 0);
    Direct3DDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
    Direct3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE );

    //Texture samplers
    for (UINT i=0; i<16; i++) {
	    Direct3DDevice->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	    Direct3DDevice->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	    Direct3DDevice->SetSamplerState( i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
        Direct3DDevice->SetSamplerState( i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
        Direct3DDevice->SetSamplerState( i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
        Direct3DDevice->SetSamplerState( i, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP );
    }
}

bool CDirectGraphicsManager::IsDepthFormatValid(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT DepthFormat) {
    if (Direct3D==NULL) {
        _WARN(this,ErrMgr->TNoSupport,"Direct3D object not yet created");
        return FALSE;
    } else {
        HRESULT hr = Direct3D->CheckDeviceFormat( Adapter,
                                              DevType,
                                              AdapterFormat,
                                              D3DUSAGE_DEPTHSTENCIL,
                                              D3DRTYPE_SURFACE,
                                              DepthFormat);

        return SUCCEEDED( hr );
    }
}



D3DFORMAT CDirectGraphicsManager::GetBestDepthStencilBuffer(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterMode, int &DesiredZDepth, int &DesiredStencil) {
    if (Direct3D==NULL) {
        _WARN(this,ErrMgr->TNoSupport,"Direct3D object not yet created");
        return D3DFMT_UNKNOWN;
    } else {
        _LOG(this,D3_DV_GRP0_LEV4,"Finding Depth buffer format for %d , device %d, Mode %d",Adapter,DevType,AdapterMode);
        bool ExD32S0    = IsDepthFormatValid(Adapter,DevType,AdapterMode, D3DFMT_D32);
        bool ExD24S8    = IsDepthFormatValid(Adapter,DevType,AdapterMode, D3DFMT_D24S8);
        bool ExD24X8    = IsDepthFormatValid(Adapter,DevType,AdapterMode, D3DFMT_D24X8);
        bool ExD24X4S4  = IsDepthFormatValid(Adapter,DevType,AdapterMode, D3DFMT_D24X4S4);
        bool ExD16      = IsDepthFormatValid(Adapter,DevType,AdapterMode, D3DFMT_D16);
        bool ExD16LOCK  = IsDepthFormatValid(Adapter,DevType,AdapterMode, D3DFMT_D16_LOCKABLE);
        bool ExD15S1    = IsDepthFormatValid(Adapter,DevType,AdapterMode, D3DFMT_D15S1);
        if (DesiredZDepth>=32) {
            if (ExD32S0) {
                DesiredZDepth = 32;
                DesiredStencil = 0;
                return D3DFMT_D32;
            }
            DesiredStencil = -1;
        }
        //go down to 24 bits
        if (DesiredZDepth>=24) {
            if (DesiredStencil>=8 || DesiredStencil<0) {
                if (ExD24S8) {
                    DesiredZDepth = 24;
                    DesiredStencil = 8;
                    return D3DFMT_D24S8;
                }
            }
            if (DesiredStencil>=4 || DesiredStencil<0) {
                if (ExD24X4S4) {
                    DesiredZDepth = 24;
                    DesiredStencil = 4;
                    return D3DFMT_D24X4S4;
                }
            }
            if (DesiredStencil<=0) {
                if (ExD24X8) {
                    DesiredZDepth = 24;
                    DesiredStencil = 0;
                    return D3DFMT_D24X8;
                }
            }
        }
        //go down to 16 bits
        if (DesiredZDepth>=16) {
            if (DesiredStencil>=1) {
                if (ExD15S1) {
                    DesiredZDepth = 15;
                    DesiredStencil = 1;
                    return D3DFMT_D15S1;
                }
            }
            if (DesiredStencil<=0) {
                if (ExD16) {
                    DesiredZDepth = 16;
                    DesiredStencil = 0;
                    return D3DFMT_D16;
                }
                if (ExD16LOCK) {
                    DesiredZDepth = 16;
                    DesiredStencil = 0;
                    return D3DFMT_D16_LOCKABLE;
                }
            }
        }
        //fail safe... try everything in (my opinion) optimal sort (by stencil)
        if (ExD24S8) {
            DesiredZDepth = 24;
            DesiredStencil = 8;
            return D3DFMT_D24S8;
        }
        if (ExD24X4S4) {
            DesiredZDepth = 24;
            DesiredStencil = 4;
            return D3DFMT_D24X4S4;
        }
        if (ExD15S1) {
            DesiredZDepth = 15;
            DesiredStencil = 1;
            return D3DFMT_D15S1;
        }
        if (ExD32S0) {
            DesiredZDepth = 32;
            DesiredStencil = 0;
            return D3DFMT_D32;
        }
        if (ExD24X8) {
            DesiredZDepth = 24;
            DesiredStencil = 0;
            return D3DFMT_D24X8;
        }
        if (ExD16) {
            DesiredZDepth = 16;
            DesiredStencil = 0;
            return D3DFMT_D16;
        }
        if (ExD16LOCK) {
            DesiredZDepth = 16;
            DesiredStencil = 0;
            return D3DFMT_D16_LOCKABLE;
        }
        return D3DFMT_UNKNOWN;
    }
}

