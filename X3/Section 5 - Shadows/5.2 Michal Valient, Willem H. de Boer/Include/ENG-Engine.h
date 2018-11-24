//############################################################################
//# Engine - Manages whole thing
//############################################################################

#pragma once

#include "global.h"

//############################################################################
//Forward declarations
class CWindowManager;
class CDirectGraphicsManager;
class CRenderSettings;
class CMeshManager;
class CFileManager;
class CTextureManager;
class CEngineDefaults;
class CRuntimeCreator;
class CMaterialManager;
class CShaderManager;

class CEngine {
private:
    HINSTANCE               AppInstance;
    CWindowManager          *Window;
    CDirectGraphicsManager  *Gfx;
    CRenderSettings         *RenderSettings;
    CMeshManager            *MeshManager;
    CFileManager            *FileManager;
    CTextureManager         *TextureManager;
    CEngineDefaults         *EngineDefaults;
    CRuntimeCreator         *RuntimeCreator;
    CMaterialManager        *MaterialManager;
    CShaderManager          *ShaderManager;

    DWORD                   LastTemporaryNumber;
protected:
    CEngine();

    D3_INLINE void SetWindowManager(CWindowManager &WindowManager) {
        Window = &WindowManager;
    }

    D3_INLINE void SetGraphicsManager(CDirectGraphicsManager &GfxManager) {
        Gfx = &GfxManager;
    }

    D3_INLINE void SetRenderSettings(CRenderSettings &Settings) {
        RenderSettings = &Settings;
    }
public:
	void Restore();
	void DeviceLost();
	bool InitEngineFromINI(char *IniFileName);
    static CEngine *CreateEngine(char *sLogFile);
    virtual ~CEngine();

    //This functions returns unique temporary number for resource managers
    DWORD GetUniqueTemporaryNumber();

    //Application instance settings
    D3_INLINE HINSTANCE GetAppInstance() { return AppInstance; }
    //Window manager manipulator
    D3_INLINE CWindowManager *GetWindowManager() { return Window; }
    //Direct graphics manager manipulator
    D3_INLINE CDirectGraphicsManager *GetGraphicsManager() { return Gfx; }
    //Render settings manipulator
    D3_INLINE CRenderSettings *GetRenderSettings() { return RenderSettings; }
    //Mesh manager manipulator
    D3_INLINE CMeshManager *GetMeshManager() { return MeshManager; }
    //Texture manager manipulator
    D3_INLINE CTextureManager *GetTextureManager() { return TextureManager; }
    //Texture manager manipulator
    D3_INLINE CFileManager *GetFileManager() { return FileManager; }
    //Engine defaults
    D3_INLINE CEngineDefaults *GetEngineDefaults() { return EngineDefaults; }
    //runtime creator
    D3_INLINE CRuntimeCreator *GetRuntimeCreator() { return RuntimeCreator; }
    //MaterialManager
    D3_INLINE CMaterialManager *GetMaterialManager() { return MaterialManager; }
    //ShaderManager
    D3_INLINE CShaderManager *GetShaderManager() { return ShaderManager; }
    // This method registers all classes derived from CBaseClass that engine knows for dynamic creation
    void RegisterRuntimeClasses(void);
};

