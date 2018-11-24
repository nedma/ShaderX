// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

//TODO: rozbehat

#include "stdafx.h"

#include "ENG-Engine.h"

#include "nommgr.h"
#include <D3dx9.h>
#include "mmgr.h"
#include "WND-WindowManager.h"
#include "MGR-DirectGraphicsManager.h"
#include "ENG-RenderSettings.h"
#include "ENG-MeshManager.h"
#include "FIL-FileManager.h"
#include "FIL-DirTreeDirectory.h"
#include "ENG-TextureManager.h"
#include "eng-enginedefaults.h"
#include "sc-runtimecreator.h"
#include "io.h"
#include "SC-IniFile.h"    
#include "SC-MString.h"
#include "DG-DirectGraphicsDeviceInfo.h"
#include "Material\Mat-MaterialManager.h"
#include "Shader\SHD-ShaderManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEngine::CEngine() {
    int logE = _LOGB("Engine",NULL,D3_DV_GRP0_LEV0,"Creating engine");
    AppInstance = (HINSTANCE) GetModuleHandle(NULL);
    if (AppInstance==NULL) {
        _ERROR("ENG", "Eng" ,CErrorManager::TSystem,"Cannot get application instance");
    }

    LastTemporaryNumber = D3_GRI_TemporaryStart;

    RenderSettings = new CRenderSettings;
    RenderSettings->Engine = this;
    Gfx = NULL;
    Window = NULL;
    MeshManager = new CMeshManager("MeshManager",this);
    TextureManager = new CTextureManager("TextureManager",this);
    FileManager = new CFileManager("FileManager",this);
    EngineDefaults = new CEngineDefaults("EngineDefaults",this);
    RuntimeCreator = new CRuntimeCreator("RuntimeCreator",this);
    MaterialManager = new CMaterialManager("MaterialManager",this);
    ShaderManager = new CShaderManager("ShaderManager", this);

    //at last we register runtime classes
    RegisterRuntimeClasses();
    _LOGE(logE,"Engine created");
    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"------------------------------------------------------------------------");
    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"-[Construct]------------------------------------------------------------");
    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"------------------------------------------------------------------------");
}

CEngine::~CEngine() {
    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"------------------------------------------------------------------------");
    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"-[Destruct]-------------------------------------------------------------");
    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"------------------------------------------------------------------------");
    int logE = _LOGB("Engine",NULL,D3_DV_GRP0_LEV0,"Destroying engine");
    if (MeshManager!=NULL) 
        delete MeshManager;
    if (MaterialManager!=NULL) 
        delete MaterialManager;
    if (ShaderManager!=NULL) 
        delete ShaderManager;
    if (TextureManager!=NULL) 
        delete TextureManager;
    if (FileManager!=NULL) 
        delete FileManager;
    if (RenderSettings!=NULL) 
        delete RenderSettings;
    if (Gfx!=NULL) 
        delete Gfx;
    if (Window!=NULL) 
        delete Window;
    if (EngineDefaults!=NULL) 
        delete EngineDefaults;
    if (RuntimeCreator!=NULL) 
        delete RuntimeCreator;
    _LOGE(logE,"Engine destroyed");
    CMemoryManager::DeleteMemoryManager("CEngine");
    CErrorManager::DeleteErrorManager("CEngine");
}

CEngine *CEngine::CreateEngine(char *LogFile) {
    CErrorManager::GetErrorManager("CEngine",LogFile);
    CMemoryManager::GetMemoryManager("CEngine");
    return new CEngine();
}

DWORD CEngine::GetUniqueTemporaryNumber() {
    if (LastTemporaryNumber==0xFFFFFFFF) {
        _WARN("Engine",NULL, ErrMgr->TSystem, "Last temporary number");
        return LastTemporaryNumber;
    }
    DWORD retVal = LastTemporaryNumber;
    LastTemporaryNumber++;
    if (LastTemporaryNumber==0xFFFFFFFF) {
        _WARN("Engine",NULL, ErrMgr->TSystem, "Last temporary number");
    }
    return retVal;
}

bool CEngine::InitEngineFromINI(char *IniFileName) {
    if (IniFileName==NULL) return FALSE;
    int logE = _LOGB("Engine",NULL, D3_DV_GRP0_LEV0,"Initialising engine from INI file '%s'", IniFileName);
    FILE *f = fopen(IniFileName,"rt");
    int handle = _fileno(f);
    int filesize = _filelength(handle);
    char *buffer = (char *)MemMgr->AllocMem(filesize+1,0);
    int readed = fread(buffer,1 , filesize,f);
    fclose(f);
    CIniFile *ini = new CIniFile("EngineINI",this);
    ini->Load(buffer,readed);
    MemMgr->FreeMem(buffer);
    //read settings from ini...
    //Log file
    CMString tmp = ini->ReadString("engine","logfile","Engine.Log");
    ErrMgr->RenameLogFile(tmp.c_str());

    //Window settings
    bool Fullscreen = ini->ReadBool("graphics","fullscreen",TRUE);

    //HACK: We had to disable the fullscreen mode
    Fullscreen = FALSE;

    int Width = ini->ReadInt("graphics","width",640);
    int Height = ini->ReadInt("graphics","height",480);
    Window = new CWindowManager("Window",this);
    if (!Fullscreen) {
//        Window->CreateWnd("MainEngineWindow",0,WS_OVERLAPPEDWINDOW,0,0,Width,Height);
        Window->CreateWnd("MainEngineWindow",0,WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,0,0,Width,Height);
    } else {
//        Window->CreateWnd("MainEngineWindow",WS_EX_TOPMOST,WS_OVERLAPPEDWINDOW,0,0,Width,Height);
        Window->CreateWnd("MainEngineWindow",WS_EX_TOPMOST,0,0,0,Width,Height);
    }
    Window->Show();

    //RECT wRect;
    //GetClientRect(Window->m_hWnd, &wRect);
    //Width = wRect.right - wRect.left;
    //Height = wRect.bottom - wRect.top;
    //printf("\n%i x %i\n", Width, Height);
    //DirectGraphics settings
    int Adapter =  ini->ReadInt("graphics","adapter",0);
    D3DDEVTYPE  DeviceType =  (D3DDEVTYPE)ini->ReadInt("graphics","devicetype",1);
    UINT Refresh = ini->ReadInt("graphics","refresh",0);
    int Bpp = ini->ReadInt("graphics","bpp",32);
    int Zbuffer = ini->ReadInt("graphics","zbuffer",0);
    int Stencil = ini->ReadInt("graphics","stencil",-1);
    int BackBufferCount = ini->ReadInt("graphics","backbuffercount",2);
    int MultiSampleLevel = ini->ReadInt("graphics","multisample",0);
    Gfx = new CDirectGraphicsManager("DX",this);
    Gfx->CreateDirect3D();

    D3DPRESENT_PARAMETERS presentation_params;
    memset(&presentation_params,0,sizeof(D3DPRESENT_PARAMETERS));
    D3DDISPLAYMODE mode;

    if (!Fullscreen) {
        Gfx->Direct3D->GetAdapterDisplayMode(Adapter,&mode);
        presentation_params.BackBufferCount = BackBufferCount;
        presentation_params.MultiSampleType = (D3DMULTISAMPLE_TYPE)MultiSampleLevel;
        presentation_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
        if (mode.Format != D3DFMT_X8R8G8B8)
            _ERROR("ENG", "Eng" ,CErrorManager::TSystem,"This application works only in 32bit windowed mode",Width,Height,Bpp,Refresh);
        presentation_params.BackBufferFormat = D3DFMT_A8R8G8B8;
        presentation_params.Windowed = TRUE;
        presentation_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
        if (Zbuffer>0) {
            presentation_params.EnableAutoDepthStencil = TRUE;
            presentation_params.AutoDepthStencilFormat = Gfx->GetBestDepthStencilBuffer(Adapter, DeviceType, mode.Format, Zbuffer, Stencil);
            if (presentation_params.AutoDepthStencilFormat == D3DFMT_UNKNOWN) {
                presentation_params.EnableAutoDepthStencil = FALSE;
                Zbuffer = 0;
                Stencil = 0;
            }
        } else {
            presentation_params.EnableAutoDepthStencil = FALSE;
            Zbuffer = 0;
            Stencil = 0;
        }
    } else {
        CDirectGraphicsDeviceInfo *dev = Gfx->GetDeviceInfo(Adapter,DeviceType);
        D3DFORMAT FsMode = dev->GetModeFormat(Width,Height,Bpp,Refresh);
        delete dev;
        if (FsMode == D3DFMT_UNKNOWN) {
            _ERROR("ENG", "Eng" ,CErrorManager::TSystem,"Cannot find specified mode %u x %u x %u at %uHz",Width,Height,Bpp,Refresh);
        }
        presentation_params.BackBufferWidth = Width;
        presentation_params.BackBufferHeight = Height;
        presentation_params.BackBufferFormat = FsMode;
        presentation_params.BackBufferCount = BackBufferCount;
        presentation_params.FullScreen_RefreshRateInHz = Refresh;
        presentation_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
        presentation_params.hDeviceWindow = Window->m_hWnd;
        presentation_params.Windowed = FALSE;
        presentation_params.EnableAutoDepthStencil = TRUE;
        presentation_params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
        if (Zbuffer>0) {
            presentation_params.EnableAutoDepthStencil = TRUE;
            presentation_params.AutoDepthStencilFormat = Gfx->GetBestDepthStencilBuffer(Adapter, DeviceType, presentation_params.BackBufferFormat, Zbuffer, Stencil);
            if (presentation_params.AutoDepthStencilFormat == D3DFMT_UNKNOWN) {
                presentation_params.EnableAutoDepthStencil = FALSE;
                Zbuffer = 0;
                Stencil = 0;
            }
        } else {
            presentation_params.EnableAutoDepthStencil = FALSE;
            Zbuffer = 0;
            Stencil = 0;
        }
    }
    
    //Behavior flags settings
    DWORD PassedBehaviorFlags = 0;
    CMString tmpBehFlags = ini->ReadString("graphics","behaviorflags","D3DCREATE_HARDWARE_VERTEXPROCESSING");
    strupr(tmpBehFlags.c_str());
    if (strstr(tmpBehFlags.c_str(), "D3DCREATE_FPU_PRESERVE") != NULL) {
        PassedBehaviorFlags |= D3DCREATE_FPU_PRESERVE;
    } else if (strstr(tmpBehFlags.c_str(), "D3DCREATE_HARDWARE_VERTEXPROCESSING") != NULL) {
        PassedBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
    } else if (strstr(tmpBehFlags.c_str(), "D3DCREATE_MIXED_VERTEXPROCESSING") != NULL) {
        PassedBehaviorFlags |= D3DCREATE_MIXED_VERTEXPROCESSING;
    } else if (strstr(tmpBehFlags.c_str(), "D3DCREATE_MULTITHREADED") != NULL) {
        PassedBehaviorFlags |= D3DCREATE_MULTITHREADED;
    } else if (strstr(tmpBehFlags.c_str(), "D3DCREATE_PUREDEVICE") != NULL) {
        PassedBehaviorFlags |= D3DCREATE_PUREDEVICE;
    } else if (strstr(tmpBehFlags.c_str(), "D3DCREATE_SOFTWARE_VERTEXPROCESSING") != NULL) {
        PassedBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    } else if (strstr(tmpBehFlags.c_str(), "D3DCREATE_DISABLE_DRIVER_MANAGEMENT") != NULL) {
        PassedBehaviorFlags |= D3DCREATE_DISABLE_DRIVER_MANAGEMENT;
    }
    
    //Gfx->CreateDirect3DDevice(Adapter,DeviceType,Window->m_hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&presentation_params);
    Gfx->CreateDirect3DDevice(Adapter,DeviceType,Window->m_hWnd,PassedBehaviorFlags,&presentation_params);
    RenderSettings->Direct3DDevice = Gfx->Direct3DDevice;
    
    //Add directories for file manager
    int tmpResourceCount = ini->ReadInt("resources","resourcelocationcount",0);
    for (int i=0; i<tmpResourceCount; i++) {
        CMString tmpLocation;
        tmpLocation.Format("resourcelocation_%i", i+1);
        CMString tmpLocation2 = ini->ReadString("resources", tmpLocation.c_str(), "");
        if (tmpLocation2.Length() > 0) {
            CDirTreeDirectory *dir = new CDirTreeDirectory("Textures",this);
            if (dir->Initialise(tmpLocation2)) {
                GetFileManager()->AddNewDirectory(*dir);
            } else {
                _WARN("Engine", NULL, ErrMgr->TFile, "Cannot initialize directory %s (from key %s)", tmpLocation2.c_str(), tmpLocation.c_str());
            }
        } else {
            _WARN("Engine", NULL, ErrMgr->TFile, "Cannot load resource location from key %s", tmpLocation.c_str());
        }
    }

    delete ini;
    _LOGE(logE,"Engine initialised");
    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"------------------------------------------------------------------------");
    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"-[INI]------------------------------------------------------------------");
    _LOG("Engine",NULL,D3_DV_GRP0_LEV0,"------------------------------------------------------------------------");
    return TRUE;
}

void CEngine::DeviceLost() {
    int logE = _LOGB("Engine",NULL,D3_DV_GRP0_LEV0,"Engine sending Device Lost");
    if (this->MeshManager!=NULL) this->MeshManager->DeviceLost(*this->RenderSettings);
    if (this->TextureManager!=NULL) this->TextureManager->DeviceLost(*this->RenderSettings);
    if (this->MaterialManager!=NULL) this->MaterialManager->DeviceLost(*this->RenderSettings);
    if (this->ShaderManager!=NULL) this->ShaderManager->DeviceLost(*this->RenderSettings);
    _LOGE(logE,"Engine sended");
}

void CEngine::Restore() {
    int logE = _LOGB("Engine",NULL,D3_DV_GRP0_LEV0,"Engine sending Restore");
    if (this->Gfx!=NULL) this->Gfx->SetDefaultRenderStates();
    if (this->MeshManager!=NULL) this->MeshManager->Restore(*this->RenderSettings);
    if (this->TextureManager!=NULL) this->TextureManager->Restore(*this->RenderSettings);
    if (this->MaterialManager!=NULL) this->MaterialManager->Restore(*this->RenderSettings);
    if (this->ShaderManager!=NULL) this->ShaderManager->Restore(*this->RenderSettings);
    _LOGE(logE,"Engine sended");
}
