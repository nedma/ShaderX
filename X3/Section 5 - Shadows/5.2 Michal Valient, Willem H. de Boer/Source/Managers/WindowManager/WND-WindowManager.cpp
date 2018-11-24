// WND-WindowManager.cpp: implementation of the CWindowManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WND-WindowManager.h"
#include "EngineVersion.h"
#include "resource.h"
#include "SU-StrUtils.h"
#include "ENG-Engine.h"

#include "MGR-DirectGraphicsManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef struct sPrivateCSTag {
    void *m_pOriginal;  //original parameter
    void *m_pWndManager;    //ptr to window manager;
} sPrivateCS;

static LRESULT CALLBACK _WinManagerWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch  (uMsg) {
        case WM_MOVE: {
            CWindowManager *wm = (CWindowManager *)GetWindowLongPtr(hwnd,0);
            if (wm!=NULL) {
                GetWindowRect(wm->m_hWnd,&wm->m_tWindowRect);
                GetClientRect(wm->m_hWnd,&wm->m_tClientRect);
                wm->m_iClientWidth = wm->m_tClientRect.right-wm->m_tClientRect.left;
                wm->m_iClientHeight = wm->m_tClientRect.bottom-wm->m_tClientRect.top;
                wm->m_iWidth = wm->m_tWindowRect.right-wm->m_tWindowRect.left;
                wm->m_iHeight = wm->m_tWindowRect.bottom-wm->m_tWindowRect.top;
                wm->OnMove(wm->m_tWindowRect.left,wm->m_tWindowRect.top);
                wm->WindowProcedure(uMsg,wParam,lParam);
                return 0;
            } else return DefWindowProc(hwnd,uMsg,wParam,lParam);
        } break;
        case WM_SIZE: {
            CWindowManager *wm = (CWindowManager *)GetWindowLongPtr(hwnd,0);
            if (wm!=NULL) {
                if (wParam==SIZE_MINIMIZED) wm->m_bActive = FALSE;
                GetWindowRect(wm->m_hWnd,&wm->m_tWindowRect);
                GetClientRect(wm->m_hWnd,&wm->m_tClientRect);
                wm->m_iClientWidth = wm->m_tClientRect.right-wm->m_tClientRect.left;
                wm->m_iClientHeight = wm->m_tClientRect.bottom-wm->m_tClientRect.top;
                wm->m_iWidth = wm->m_tWindowRect.right-wm->m_tWindowRect.left;
                wm->m_iHeight = wm->m_tWindowRect.bottom-wm->m_tWindowRect.top;
                wm->OnSize(wm->m_iClientWidth,wm->m_iClientHeight);
                wm->WindowProcedure(uMsg,wParam,lParam);
                return 0;
            } else return DefWindowProc(hwnd,uMsg,wParam,lParam);
        } break;
        case WM_ACTIVATE: {
            CWindowManager *wm = (CWindowManager *)GetWindowLongPtr(hwnd,0);
            if (wm!=NULL) {
                if (LOWORD(wParam)==WA_INACTIVE) {
                    wm->m_bActive = FALSE;
                    //_LOG("Window","Window",D3_DV_GRP0_LEV0,"Deactivate");
                } else {
                    wm->m_bActive = TRUE;
                    //_LOG("Window","Window",D3_DV_GRP0_LEV0,"Activate");
                }
                wm->WindowProcedure(uMsg,wParam,lParam);
                return 0;
            } else return DefWindowProc(hwnd,uMsg,wParam,lParam);
        } break;
        case WM_SHOWWINDOW: {
            CWindowManager *wm = (CWindowManager *)GetWindowLongPtr(hwnd,0);
            if (wm!=NULL) {
                if (!wParam) wm->m_bActive = FALSE;
//                else wm->m_bActive = TRUE;
                wm->WindowProcedure(uMsg,wParam,lParam);
                return 0;
            } else return DefWindowProc(hwnd,uMsg,wParam,lParam);
        } break;
        case WM_CREATE: {
            //store our things and make it transparent to application
            CREATESTRUCT *cs = (LPCREATESTRUCT) lParam;
            sPrivateCS *pcs = (sPrivateCS *)cs->lpCreateParams;
            SetWindowLongPtr(hwnd,0,(long)pcs->m_pWndManager);
            cs->lpCreateParams = pcs->m_pOriginal;
            CWindowManager *wm = (CWindowManager *)GetWindowLongPtr(hwnd,0);
            if (wm!=NULL) return wm->OnCreate(cs);
            else return DefWindowProc(hwnd,uMsg,wParam,lParam);
        } break;
        case WM_CLOSE: {
            CWindowManager *wm = (CWindowManager *)GetWindowLongPtr(hwnd,0);
            if (wm!=NULL) {
                wm->OnClose();
                wm->DestroyWnd();
            }
            PostMessage(NULL,MSG_WND_CLOSED,0,(int)wm);
            return 0;
        } break;
        default: {
            CWindowManager *wm = (CWindowManager *)GetWindowLongPtr(hwnd,0);
            if (wm!=NULL) return wm->WindowProcedure(uMsg,wParam,lParam);
            else return DefWindowProc(hwnd,uMsg,wParam,lParam);
        }
    }
}


CWindowManager::CWindowManager(char *sObjName, CEngine *pOwnerEngine) : CBaseManager(sObjName, pOwnerEngine) {
    int LogE = _LOGB(this,D3_DV_GRP0_LEV0,"Created...");
    m_sClassName=NULL;
    m_sWindowName=NULL;
    m_hWnd=NULL;
    _LOGE(LogE);
}

CWindowManager::~CWindowManager() {
    int logE = _LOGB(this,D3_DV_GRP0_LEV0,"Being Destroyed");
    if (m_hWnd!=NULL) DestroyWnd();
    if (m_sClassName!=NULL) DestroyWndClass();
    if (m_sWindowName!=NULL) MemMgr->FreeMem(m_sWindowName);
    if (m_sClassName!=NULL) MemMgr->FreeMem(m_sClassName);
    _LOGE(logE,"Destroyed...");
}

bool CWindowManager::CreateWndClass(UINT Flags) {
    //1. find if the class is defined yet...
    WNDCLASSEX ci;
    char sClassName[128];
    strcpy(sClassName,ProjectNameString);
    strcat(sClassName,"__v");
    strcat(sClassName,ProjectVersionString);
    if (!GetClassInfoEx(OwnerEngine->GetAppInstance(),sClassName,&ci)) {
        //2. Create Class if needed and register it
        ci.cbSize=sizeof(WNDCLASSEX);
        ci.style=CS_HREDRAW | CS_OWNDC | CS_VREDRAW;
        ci.lpfnWndProc = _WinManagerWindowProc;
        ci.cbClsExtra = 0;
        ci.cbWndExtra = 4;
        ci.hInstance = OwnerEngine->GetAppInstance();
        ci.hIcon = (HICON)LoadIcon(OwnerEngine->GetAppInstance(),MAKEINTRESOURCE(IDI_APPICON));
        ci.hIconSm = (HICON)LoadIcon(OwnerEngine->GetAppInstance(),MAKEINTRESOURCE(IDI_APPICON));
        ci.hbrBackground = NULL;
        ci.hCursor = (HCURSOR)LoadCursor(NULL,IDC_ARROW);
        ci.lpszMenuName = NULL;
        ci.lpszClassName = sClassName;
        if (RegisterClassEx(&ci)==0) {
            _FATAL(this,ErrMgr->TWindow,"Cannot register window class");
            return FALSE;
        }
        _LOG(this,D3_DV_GRP0_LEV1,"Created window class '%s'",sClassName);
    } else {
        _LOG(this,D3_DV_GRP0_LEV1,"Window class '%s' already registered",sClassName);
    }
    //3. Store name
    m_sClassName = suStrReallocCopy(m_sClassName,sClassName);
    return TRUE;
}

bool CWindowManager::CreateWnd(char *sWindowName, UINT iExStyle, UINT iStyle, int iX, int iY,
               int iWidth, int iHeight, HWND hWndParent, HMENU hMenu, LPVOID lpParam) {
    if (m_hWnd==NULL) {
        //1.create window
        if (m_sClassName==NULL) CreateWndClass(0);
        //old lparam should be saved;
        sPrivateCS PrivateCS;
        PrivateCS.m_pOriginal = lpParam;
        PrivateCS.m_pWndManager = this;

        RECT clientSize;
        clientSize.top = clientSize.left = 0;
        clientSize.right = iWidth;
        clientSize.bottom = iHeight;
        BOOL HasMenu = (hMenu)?TRUE:FALSE;
        AdjustWindowRectEx(&clientSize, iStyle, HasMenu, iExStyle);
        iWidth = clientSize.right - clientSize.left;
        iHeight = clientSize.bottom - clientSize.top;
        //printf("\n%i x %i\n", iWidth, iHeight);

        HWND ret = CreateWindowEx(iExStyle,m_sClassName,sWindowName,iStyle,iX,iY,iWidth,
            iHeight,hWndParent,hMenu,OwnerEngine->GetAppInstance(),&PrivateCS);
        //2.copy name and handle
        if (ret!=NULL) {
            SetWindowLongPtr(ret,0,(long)this);
            m_hWnd = ret;
            m_sWindowName = suStrReallocCopy(m_sWindowName,sWindowName);
            _LOG(this,D3_DV_GRP0_LEV1,"Created window named '%s'",sWindowName);
            return TRUE;
        } else {
            _FATAL(this,ErrMgr->TWindow,"Failed to create window named '%s'",sWindowName);
            return FALSE;
        }
    } else {
        _ERROR(this,ErrMgr->TWindow,"Already has window '%s'. Cannot create new",m_sWindowName);
        return FALSE;
    }
}

bool CWindowManager::DestroyWnd() {
    if (m_hWnd!=NULL) {
        if (DestroyWindow(m_hWnd)) {
            m_hWnd=NULL;
            _LOG(this,D3_DV_GRP0_LEV1,"Destroyed window named '%s'",m_sWindowName);
            MemMgr->FreeMem(m_sWindowName);
            m_sWindowName=NULL;
            DestroyWndClass();
            return TRUE;
        } else {
            _ERROR(this,ErrMgr->TWindow,"Failed to destroy window named '%s'",m_sWindowName);
            return FALSE;
        }
    } else {
        _ERROR(this,ErrMgr->TWindow,"Cannot destroy window before creating one!");
        return FALSE;
    }
}

bool CWindowManager::DestroyWndClass() {
    if (m_sClassName!=NULL) {
        if (UnregisterClass(m_sClassName,OwnerEngine->GetAppInstance())) {
            _LOG(this,D3_DV_GRP0_LEV1,"Destroyed window class named '%s'",m_sClassName);
            MemMgr->FreeMem(m_sClassName);
            m_sClassName=NULL;
            return TRUE;
        } else {
            _WARN(this,ErrMgr->TWindow,"Class cannot be destroyed. Probably other windows still use it");
            return FALSE;
        }
    } else {
        _ERROR(this,ErrMgr->TWindow,"Cannot destroy window class before creating one!");
        return FALSE;
    }
}

bool CWindowManager::Show(bool bShow) {
    if (bShow) return ShowWnd(SW_SHOW);
    else return ShowWnd(SW_HIDE);
}

bool CWindowManager::ShowWnd(int cmd) {
    if (m_hWnd!=NULL) {
        return (ShowWindow(m_hWnd,cmd) == TRUE) ? true : false;
    } else {
        _ERROR(this,ErrMgr->TWindow,"No window created, cannot operate");
        return FALSE;
    }
}

LRESULT CWindowManager::WindowProcedure(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(m_hWnd,uMsg,wParam,lParam);
}

int D3_FASTCALL CWindowManager::OnMove(int iX, int iY) {
    return 0;
}

int D3_FASTCALL CWindowManager::OnSize(int iClientWidth,int iClientHeight) {
    if (this->OwnerEngine==NULL) return -1;
    CDirectGraphicsManager *man = this->OwnerEngine->GetGraphicsManager();
    if (man==NULL) return -1;

    this->OwnerEngine->DeviceLost();
    //Scene->DeviceLost(*eng->GetRenderSettings());
    this->OwnerEngine->GetGraphicsManager()->StartPresentationParameters.BackBufferHeight = iClientHeight;
    this->OwnerEngine->GetGraphicsManager()->StartPresentationParameters.BackBufferWidth = iClientWidth;
    this->OwnerEngine->GetGraphicsManager()->Direct3DDevice->Reset(&this->OwnerEngine->GetGraphicsManager()->StartPresentationParameters);
    this->OwnerEngine->Restore();
    //Scene->Restore(*eng->GetRenderSettings());
    return 0;
}

int D3_FASTCALL CWindowManager::OnCreate(CREATESTRUCT *pCS) {
    return 0;
}

int CWindowManager::OnClose() {
    return 0;
}

