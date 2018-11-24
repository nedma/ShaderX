// DG-DirectGraphicsDeviceInfo.cpp: implementation of the CDirectGraphicsDeviceInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DG-DirectGraphicsDeviceInfo.h"
#include "SU-StrUtils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define _GET(X,Y) ((X & Y)!=0) ? TRUE : FALSE

bool CDirectGraphicsDeviceInfo::GetDeviceCaps(IDirect3D9 &Direct3D, UINT Adapter, D3DDEVTYPE DevType) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV2,"Getting Device Caps for adapter %d, DevType %d",Adapter,DevType);
    HRESULT ret = Direct3D.GetDeviceCaps(Adapter,DevType,&DevCaps);
    if (ret!=D3D_OK) {
        _FATAL(this, ErrMgr->TSystem,"Cannot get device caps (Adapter %d, DevType %d)",Adapter,DevType);
    }
    _LOGE(logE);
    return (ret==D3D_OK);
}

void CDirectGraphicsDeviceInfo::GetMonitor(IDirect3D9 &Direct3D, UINT Adapter) {
    int logE = _LOGB(this,D3_DV_GRP0_LEV2,"Getting Monitor info for adapter %d",Adapter);
    AdapterMonitor = Direct3D.GetAdapterMonitor(Adapter);
    _LOG(this,D3_DV_GRP0_LEV2,"Monitor: %p",AdapterMonitor);
    _LOGE(logE);
}

bool CDirectGraphicsDeviceInfo::GetDriverInfo(IDirect3D9 &Direct3D, UINT Adapter) {
    D3DADAPTER_IDENTIFIER9 tmpai;
    bool res = TRUE;
    int logE = _LOGB(this,D3_DV_GRP2_LEV2,"Getting Driver info for adapter %d",Adapter);
    if (Direct3D.GetAdapterIdentifier(Adapter, 0, &tmpai)==D3D_OK) {
        Driver = suStrReallocCopy(Driver,tmpai.Driver);
        Description = suStrReallocCopy(Description,tmpai.Description);
        DriverVersion_Product = HIWORD(tmpai.DriverVersion.HighPart);
        DriverVersion_Version = LOWORD(tmpai.DriverVersion.HighPart);
        DriverVersion_SubVersion = HIWORD(tmpai.DriverVersion.LowPart);
        DriverVersion_Build = LOWORD(tmpai.DriverVersion.LowPart);
        VendorId = tmpai.VendorId;
        DeviceId = tmpai.DeviceId;
        SubSysId = tmpai.SubSysId;
        Revision = tmpai.Revision;
        DeviceIdentifier = tmpai.DeviceIdentifier;
        _LOG(this,D3_DV_GRP2_LEV2,"Driver: %s",Driver);
        _LOG(this,D3_DV_GRP2_LEV2,"Description: %s",Description);
        _LOG(this,D3_DV_GRP2_LEV2,"DriverVersion: %d.%d.%d.%d",DriverVersion_Product,DriverVersion_Version,DriverVersion_SubVersion,DriverVersion_Build);
        _LOG(this,D3_DV_GRP2_LEV2,"VendorId: %d",VendorId);
        _LOG(this,D3_DV_GRP2_LEV2,"DeviceId: %d",DeviceId);
        _LOG(this,D3_DV_GRP2_LEV2,"SubSysId: %d",SubSysId);
        _LOG(this,D3_DV_GRP2_LEV2,"Revision: %d",Revision);
        _LOG(this,D3_DV_GRP2_LEV2,"GUID: %u-%u-%u-%u",DeviceIdentifier.Data1,DeviceIdentifier.Data2,DeviceIdentifier.Data3,DeviceIdentifier.Data4);
    } else {
        _FATAL(this,ErrMgr->TSystem,"Cannot obtain driver info for adapter %d",Adapter);
        res = FALSE;
    }
    _LOGE(logE);
    return res;
}

bool CDirectGraphicsDeviceInfo::GetModeInfo(IDirect3D9 &Direct3D, UINT Adapter) {
    AdapterModeCount = 0;
    bool res = TRUE;
    int logE = _LOGB(this,D3_DV_GRP2_LEV2,"Getting mode info for adapter %d",Adapter);
    {
        char tmpFormatASCII[] = "A2R10G10B10";
        D3DFORMAT tmpFormatBIN = D3DFMT_A2R10G10B10;
        UINT tmpAdapterModeCount = Direct3D.GetAdapterModeCount(Adapter, tmpFormatBIN);
        int logE2 = _LOGB(this,D3_DV_GRP2_LEV2,"Getting modes for format %s (%d modes found)",tmpFormatASCII,AdapterModeCount);
        if (tmpAdapterModeCount) {
            D3DDISPLAYMODE *tmpModes = (D3DDISPLAYMODE *)MemMgr->AllocMem(sizeof(D3DDISPLAYMODE)*tmpAdapterModeCount,0);
            for (UINT i=0;i<tmpAdapterModeCount;i++) {
                if (Direct3D.EnumAdapterModes(Adapter, tmpFormatBIN, i,&tmpModes[i])!=D3D_OK) {
                    _FATAL(this,ErrMgr->TSystem,"Error retrieving mode %d for adapter %d",i,Adapter);
                    res = FALSE;
                }
            }
            Modes = (D3DDISPLAYMODE *)MemMgr->ReAllocMem(Modes, sizeof(D3DDISPLAYMODE)*(AdapterModeCount+tmpAdapterModeCount),0);
            memcpy(&Modes[AdapterModeCount], tmpModes, sizeof(D3DDISPLAYMODE) * tmpAdapterModeCount);
            AdapterModeCount += tmpAdapterModeCount;
            MemMgr->FreeMem(tmpModes);
        }
        _LOGE(logE2);
    }
    {
        char tmpFormatASCII[] = "A8R8G8B8";
        D3DFORMAT tmpFormatBIN = D3DFMT_A8R8G8B8;
        UINT tmpAdapterModeCount = Direct3D.GetAdapterModeCount(Adapter, tmpFormatBIN);
        int logE2 = _LOGB(this,D3_DV_GRP2_LEV2,"Getting modes for format %s (%d modes found)",tmpFormatASCII,AdapterModeCount);
        if (tmpAdapterModeCount) {
            D3DDISPLAYMODE *tmpModes = (D3DDISPLAYMODE *)MemMgr->AllocMem(sizeof(D3DDISPLAYMODE)*tmpAdapterModeCount,0);
            for (UINT i=0;i<tmpAdapterModeCount;i++) {
                if (Direct3D.EnumAdapterModes(Adapter, tmpFormatBIN, i,&tmpModes[i])!=D3D_OK) {
                    _FATAL(this,ErrMgr->TSystem,"Error retrieving mode %d for adapter %d",i,Adapter);
                    res = FALSE;
                }
            }
            Modes = (D3DDISPLAYMODE *)MemMgr->ReAllocMem(Modes, sizeof(D3DDISPLAYMODE)*(AdapterModeCount+tmpAdapterModeCount),0);
            memcpy(&Modes[AdapterModeCount], tmpModes, sizeof(D3DDISPLAYMODE) * tmpAdapterModeCount);
            AdapterModeCount += tmpAdapterModeCount;
            MemMgr->FreeMem(tmpModes);
        }
        _LOGE(logE2);
    }
    {
        char tmpFormatASCII[] = "X8R8G8B8";
        D3DFORMAT tmpFormatBIN = D3DFMT_X8R8G8B8;
        UINT tmpAdapterModeCount = Direct3D.GetAdapterModeCount(Adapter, tmpFormatBIN);
        int logE2 = _LOGB(this,D3_DV_GRP2_LEV2,"Getting modes for format %s (%d modes found)",tmpFormatASCII,AdapterModeCount);
        if (tmpAdapterModeCount) {
            D3DDISPLAYMODE *tmpModes = (D3DDISPLAYMODE *)MemMgr->AllocMem(sizeof(D3DDISPLAYMODE)*tmpAdapterModeCount,0);
            for (UINT i=0;i<tmpAdapterModeCount;i++) {
                if (Direct3D.EnumAdapterModes(Adapter, tmpFormatBIN, i,&tmpModes[i])!=D3D_OK) {
                    _FATAL(this,ErrMgr->TSystem,"Error retrieving mode %d for adapter %d",i,Adapter);
                    res = FALSE;
                }
            }
            Modes = (D3DDISPLAYMODE *)MemMgr->ReAllocMem(Modes, sizeof(D3DDISPLAYMODE)*(AdapterModeCount+tmpAdapterModeCount),0);
            memcpy(&Modes[AdapterModeCount], tmpModes, sizeof(D3DDISPLAYMODE) * tmpAdapterModeCount);
            AdapterModeCount += tmpAdapterModeCount;
            MemMgr->FreeMem(tmpModes);
        }
        _LOGE(logE2);
    }
    {
        char tmpFormatASCII[] = "A1R5G5B5";
        D3DFORMAT tmpFormatBIN = D3DFMT_A1R5G5B5;
        UINT tmpAdapterModeCount = Direct3D.GetAdapterModeCount(Adapter, tmpFormatBIN);
        int logE2 = _LOGB(this,D3_DV_GRP2_LEV2,"Getting modes for format %s (%d modes found)",tmpFormatASCII,AdapterModeCount);
        if (tmpAdapterModeCount) {
            D3DDISPLAYMODE *tmpModes = (D3DDISPLAYMODE *)MemMgr->AllocMem(sizeof(D3DDISPLAYMODE)*tmpAdapterModeCount,0);
            for (UINT i=0;i<tmpAdapterModeCount;i++) {
                if (Direct3D.EnumAdapterModes(Adapter, tmpFormatBIN, i,&tmpModes[i])!=D3D_OK) {
                    _FATAL(this,ErrMgr->TSystem,"Error retrieving mode %d for adapter %d",i,Adapter);
                    res = FALSE;
                }
            }
            Modes = (D3DDISPLAYMODE *)MemMgr->ReAllocMem(Modes, sizeof(D3DDISPLAYMODE)*(AdapterModeCount+tmpAdapterModeCount),0);
            memcpy(&Modes[AdapterModeCount], tmpModes, sizeof(D3DDISPLAYMODE) * tmpAdapterModeCount);
            AdapterModeCount += tmpAdapterModeCount;
            MemMgr->FreeMem(tmpModes);
        }
        _LOGE(logE2);
    }
    {
        char tmpFormatASCII[] = "X1R5G5B5";
        D3DFORMAT tmpFormatBIN = D3DFMT_X1R5G5B5;
        UINT tmpAdapterModeCount = Direct3D.GetAdapterModeCount(Adapter, tmpFormatBIN);
        int logE2 = _LOGB(this,D3_DV_GRP2_LEV2,"Getting modes for format %s (%d modes found)",tmpFormatASCII,AdapterModeCount);
        if (tmpAdapterModeCount) {
            D3DDISPLAYMODE *tmpModes = (D3DDISPLAYMODE *)MemMgr->AllocMem(sizeof(D3DDISPLAYMODE)*tmpAdapterModeCount,0);
            for (UINT i=0;i<tmpAdapterModeCount;i++) {
                if (Direct3D.EnumAdapterModes(Adapter, tmpFormatBIN, i,&tmpModes[i])!=D3D_OK) {
                    _FATAL(this,ErrMgr->TSystem,"Error retrieving mode %d for adapter %d",i,Adapter);
                    res = FALSE;
                }
            }
            Modes = (D3DDISPLAYMODE *)MemMgr->ReAllocMem(Modes, sizeof(D3DDISPLAYMODE)*(AdapterModeCount+tmpAdapterModeCount),0);
            memcpy(&Modes[AdapterModeCount], tmpModes, sizeof(D3DDISPLAYMODE) * tmpAdapterModeCount);
            AdapterModeCount += tmpAdapterModeCount;
            MemMgr->FreeMem(tmpModes);
        }
        _LOGE(logE2);
    }
    {
        char tmpFormatASCII[] = "R5G6B5";
        D3DFORMAT tmpFormatBIN = D3DFMT_R5G6B5;
        UINT tmpAdapterModeCount = Direct3D.GetAdapterModeCount(Adapter, tmpFormatBIN);
        int logE2 = _LOGB(this,D3_DV_GRP2_LEV2,"Getting modes for format %s (%d modes found)",tmpFormatASCII,AdapterModeCount);
        if (tmpAdapterModeCount) {
            D3DDISPLAYMODE *tmpModes = (D3DDISPLAYMODE *)MemMgr->AllocMem(sizeof(D3DDISPLAYMODE)*tmpAdapterModeCount,0);
            for (UINT i=0;i<tmpAdapterModeCount;i++) {
                if (Direct3D.EnumAdapterModes(Adapter, tmpFormatBIN, i,&tmpModes[i])!=D3D_OK) {
                    _FATAL(this,ErrMgr->TSystem,"Error retrieving mode %d for adapter %d",i,Adapter);
                    res = FALSE;
                }
            }
            Modes = (D3DDISPLAYMODE *)MemMgr->ReAllocMem(Modes, sizeof(D3DDISPLAYMODE)*(AdapterModeCount+tmpAdapterModeCount),0);
            memcpy(&Modes[AdapterModeCount], tmpModes, sizeof(D3DDISPLAYMODE) * tmpAdapterModeCount);
            AdapterModeCount += tmpAdapterModeCount;
            MemMgr->FreeMem(tmpModes);
        }
        _LOGE(logE2);
    }
    if (AdapterModeCount==0) {
        _FATAL(this,ErrMgr->TSystem,0,"No modes supported for adapter %d",Adapter);
        res = FALSE;
    }
    _LOGE(logE);
    return res;
}

CDirectGraphicsDeviceInfo::CDirectGraphicsDeviceInfo(char *ObjName,CEngine *OwnerEngine) : CBaseClass(ObjName,OwnerEngine) {
	int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Creating");
    Adapter = D3DADAPTER_DEFAULT;
    DeviceType = D3DDEVTYPE_HAL;
    Driver = NULL;
    Description = NULL;
    Modes = NULL;
    AdapterModeCount = 0;
    _LOGE(logE);
}

CDirectGraphicsDeviceInfo::~CDirectGraphicsDeviceInfo() {
	int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Destroying");
    if (Driver!=NULL) MemMgr->FreeMem(Driver);
    if (Description!=NULL) MemMgr->FreeMem(Description);
    if (Modes!=NULL) MemMgr->FreeMem(Modes);
	_LOGE(logE,"Destroyed");
}

bool CDirectGraphicsDeviceInfo::GetAdapterInfo(IDirect3D9 &Direct3D, UINT Adapter) {
	int logE = _LOGB(this,D3_DV_GRP2_LEV2,"Getting adapter info");
    this->Adapter = Adapter;
    bool ret = GetModeInfo(Direct3D,Adapter);
    ret &= GetDriverInfo(Direct3D,Adapter);
    GetMonitor(Direct3D,Adapter);
    _LOGE(logE);
    return ret;
}

bool CDirectGraphicsDeviceInfo::GetDeviceInfo(IDirect3D9 &Direct3D, UINT Adapter, D3DDEVTYPE DevType) {
	int logE = _LOGB(this,D3_DV_GRP2_LEV2,"Getting device info");
    DeviceType = DevType;
    bool ret = GetDeviceCaps(Direct3D,Adapter,DevType);
    _LOGE(logE);
    return ret;
}

bool CDirectGraphicsDeviceInfo::GetInfo(IDirect3D9 &Direct3D, UINT Adapter, D3DDEVTYPE DevType) {
	int logE = _LOGB(this,D3_DV_GRP2_LEV2,"Getting info");
    bool ret = GetAdapterInfo(Direct3D,Adapter);
    ret &= GetDeviceCaps(Direct3D,Adapter,DevType);
    _LOGE(logE);
    return ret;
}

D3DFORMAT CDirectGraphicsDeviceInfo::GetModeFormat(UINT Width, UINT Height, UINT Depth, UINT &Frequency) {
    D3DFORMAT ret = D3DFMT_UNKNOWN;
    UINT NearestFreq = 0;
    for (UINT i=0; i<AdapterModeCount; i++) {
        if (Modes[i].Width == Width && Modes[i].Height == Height) { //we found correct dimensions
            if (Depth==16 && (Modes[i].Format == D3DFMT_R5G6B5 || Modes[i].Format == D3DFMT_X1R5G5B5)) {    //we had correct depth
                if (abs(Modes[i].RefreshRate - Frequency)<abs(NearestFreq - Frequency)) {   //this matches it better
                    NearestFreq = Modes[i].RefreshRate;
                    ret = Modes[i].Format;
                }
            } else if (Depth==32 && (Modes[i].Format == D3DFMT_A8R8G8B8 || Modes[i].Format == D3DFMT_X8R8G8B8)) {   //we had correct depth
                if (abs(Modes[i].RefreshRate - Frequency)<abs(NearestFreq - Frequency)) {   //this matches it better
                    NearestFreq = Modes[i].RefreshRate;
                    ret = Modes[i].Format;
                }
            }
        }
    }
    Frequency = NearestFreq;
    return ret;
}
