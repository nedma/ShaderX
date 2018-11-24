// MGR-DirectGraphicsManager.h: interface for the DirectGraphicsManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "MGR-BaseManager.h"
#include <D3d9.h>

//############################################################################
//Forward declarations
class CDirectGraphicsDeviceInfo;

class CDirectGraphicsManager : public CBaseManager  {
public:
    CDirectGraphicsDeviceInfo  *SelectedDeviceInfo;
    IDirect3DDevice9           *Direct3DDevice;
    IDirect3D9                 *Direct3D;
    UINT                        Adapter;
    D3DDEVTYPE                  DevType;
    HWND                        Window;
    DWORD                       StartBehaviorFlags;
    D3DPRESENT_PARAMETERS       StartPresentationParameters;
public:
	bool CreateDirect3D();
    bool CreateDirect3DDevice(UINT Adapter, D3DDEVTYPE DevType, HWND  FocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* PresentationParameters);
    D3DFORMAT GetBestDepthStencilBuffer(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterMode, int &DesiredZDepth, int &DesiredStencil);
    bool IsDepthFormatValid(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT DepthFormat);
	CDirectGraphicsDeviceInfo *GetDeviceInfo(UINT Adapter, D3DDEVTYPE DevType);
    void SetDefaultRenderStates();
   
    //Const & Dest
	CDirectGraphicsManager(char *ObjName,CEngine *OwnerEngine);
	virtual ~CDirectGraphicsManager();
    MAKE_CLSCREATE(CDirectGraphicsManager);
    MAKE_CLSNAME("CDirectGraphicsManager");
    MAKE_DEFAULTGARBAGE();

protected:
	void Release();
};

