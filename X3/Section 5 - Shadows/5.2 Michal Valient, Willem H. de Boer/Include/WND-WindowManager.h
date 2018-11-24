// WND-WindowManager.h: interface for the CWindowManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "MGR-BaseManager.h"

const int MSG_WND_CLOSED=WM_APP+1;  //this message is send to thread! , when window is closed, lparam is pointer to CWindowManager.

class CWindowManager : public CBaseManager {
public:
    char   *m_sClassName;
    char   *m_sWindowName;
    HWND    m_hWnd;
    int     m_iHeight;
    int     m_iWidth;
    int     m_iClientHeight;
    int     m_iClientWidth;
    RECT    m_tWindowRect;
    RECT    m_tClientRect;
    bool    m_bActive;

	virtual int OnClose();
	virtual int D3_FASTCALL OnCreate(CREATESTRUCT *pCS);
    //message handlers
	virtual int D3_FASTCALL OnSize(int iClientWidth,int iClientHeight);
	virtual int D3_FASTCALL OnMove(int iX,int iY);
	virtual LRESULT WindowProcedure(UINT uMsg, WPARAM wParam, LPARAM lParam);
    //informations and settings
	bool ShowWnd(int cmd);
	bool Show(bool bShow = true);
    bool IsVisible() {
        if (m_hWnd!=NULL) {
            return (IsWindowVisible(m_hWnd)==TRUE)? true : false;
        } else return false;
    }

    //base construction set of tools
    bool CreateWnd(char *sWindowName, UINT iExStyle, UINT iStyle, int iX, int iY,
        int iWidth, int iHeight, HWND hWndParent, HMENU hMenu, LPVOID lpParam);
    bool CreateWnd(char *sWindowName, UINT iExStyle, UINT iStyle, int iX, int iY,
        int iWidth, int iHeight) {
        return CreateWnd(sWindowName,iExStyle,iStyle,iX,iY,iWidth,iHeight,NULL,NULL,NULL);
    }

    //destroy set of tools
	bool DestroyWnd();
	bool DestroyWndClass();

    //constructor & destructor
	CWindowManager(char *sObjName, CEngine *pOwnerEngine);
	virtual ~CWindowManager();
    MAKE_CLSCREATE(CWindowManager);
    MAKE_CLSNAME("CWindowManager");
    MAKE_DEFAULTGARBAGE();
protected:
	bool CreateWndClass(UINT Flags);
};

