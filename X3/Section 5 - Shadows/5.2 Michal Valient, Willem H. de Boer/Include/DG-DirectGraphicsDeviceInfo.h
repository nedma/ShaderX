// DG-DirectGraphicsDeviceInfo.h: interface for the CDirectGraphicsDeviceInfo class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BaseClass.h"
#include <d3d9.h>

//############################################################################
//Forward declarations

class CDirectGraphicsDeviceInfo : public CBaseClass {
protected:
	bool GetDeviceCaps(IDirect3D9 &Direct3D, UINT Adapter, D3DDEVTYPE DevType);
    void GetMonitor(IDirect3D9 &Direct3D, UINT Adapter);
    bool GetDriverInfo(IDirect3D9 &Direct3D, UINT Adapter);
    bool GetModeInfo(IDirect3D9 &Direct3D, UINT Adapter);
public:
    //variables...
    //modes information
    UINT            AdapterModeCount;
    D3DDISPLAYMODE *Modes;
    //general / count information
    UINT            Adapter;
    D3DDEVTYPE      DeviceType;
    HMONITOR        AdapterMonitor;
    //device caps info
    D3DCAPS9        DevCaps;
    //driver information
    char           *Driver;
    char           *Description;
    DWORD           DriverVersion_Product;
    DWORD           DriverVersion_Version;
    DWORD           DriverVersion_SubVersion;
    DWORD           DriverVersion_Build;
    DWORD           VendorId;
    DWORD           DeviceId;
    DWORD           SubSysId;
    DWORD           Revision;
    GUID            DeviceIdentifier;
public:
	D3DFORMAT GetModeFormat(UINT Width, UINT Height, UINT Depth, UINT &Frequency);
    //methods
	CDirectGraphicsDeviceInfo(char *ObjName,CEngine *OwnerEngine);
	virtual ~CDirectGraphicsDeviceInfo();
    bool GetAdapterInfo(IDirect3D9 &Direct3D, UINT Adapter);
    bool GetDeviceInfo(IDirect3D9 &Direct3D, UINT Adapter, D3DDEVTYPE DevType);
    bool GetInfo(IDirect3D9 &Direct3D, UINT Adapter, D3DDEVTYPE DevType);

    MAKE_CLSCREATE(CDirectGraphicsDeviceInfo);
    MAKE_CLSNAME("CDirectGraphicsDeviceInfo");
    MAKE_DEFAULTGARBAGE();
};

