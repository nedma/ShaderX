// FIL-FileManager.cpp: implementation of the CFileManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "Global.h"
#include "FIL-FileManager.h"
//#include "MGR-ErrorManager.h"
//#include "MGR-MemoryManager.h"
#include "SC-DynamicArray.h"
#include "FIL-BaseDirectory.h"
#include "SC-MString.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileManager::CFileManager(char *ObjName,CEngine *OwnerEngine) : CBaseManager(ObjName, OwnerEngine) {
    int logE = _LOGB(this,D3_DV_GRP0_LEV0,"Creating file manager");
    ArrayOfDirs=NULL;
    NextFreeBase=0;
    _LOGE(logE);
}

CFileManager::~CFileManager() {
    int logE = _LOGB(this,D3_DV_GRP0_LEV0,"Destroying file manager");
    if (ArrayOfDirs!=NULL) delete ArrayOfDirs;
    _LOGE(logE, "Destroyed");
}

int CFileManager::AddNewDirectory(CBaseDirectory &NewDir) {
    int logE = _LOGB(this,D3_DV_GRP0_LEV3, "Adding new directory '%s'",NewDir.GetInitString()->c_str());
    if (ArrayOfDirs==NULL) ArrayOfDirs=new CRootClassArray(TRUE);
    CSingleDirectoryInfo *NewDirInfo =new CSingleDirectoryInfo();
    NewDirInfo->Directory=&NewDir;
    if (NewDir.IsInitialised()) NewDirInfo->Enabled=TRUE;
    else {
        NewDirInfo->Enabled=FALSE;
        _WARN(this,ErrMgr->TFile,"Directory init failed.",NewDir.GetInitString()->c_str());
    }
    NewDirInfo->BaseIndex=NextFreeBase;
    NextFreeBase+=NewDir.GetResourceCount();
    int Index = ArrayOfDirs->Add(NewDirInfo);
    _LOGE(logE, "Directory added at index %i", Index);
    return Index;
}

int CFileManager::Find(CMString &Name) {
    int ret;
    for (int i=0;i<ArrayOfDirs->GetSize();i++) {
        CSingleDirectoryInfo *DirInfo = (CSingleDirectoryInfo *)ArrayOfDirs->GetAt(i);
        if (DirInfo->Enabled) {
            ret = DirInfo->Directory->GetResourceIndex(Name);
            if (ret!=-1) {
                ret = ret+DirInfo->BaseIndex;
                _LOG(this,D3_DV_GRP0_LEV4, "File '%s' found in Directory %u('%s')",Name.c_str(),i,DirInfo->Directory->GetInitString()->c_str());
                _LOG(this,D3_DV_GRP0_LEV4, "    Global index is %u, while real is %u",ret,ret-DirInfo->BaseIndex);
                return ret;
            }
        }
    }
    _LOG(this,D3_DV_GRP0_LEV4, "File '%s' not found ",Name.c_str());
    return -1;
}


CBaseDirectory * CFileManager::Find(int *LocalIndex, CMString &Name) {
    int ret;
    for (int i=0;i<ArrayOfDirs->GetSize();i++) {
        CSingleDirectoryInfo *DirInfo = (CSingleDirectoryInfo *)ArrayOfDirs->GetAt(i);
        if (DirInfo->Enabled) {
            ret = DirInfo->Directory->GetResourceIndex(Name);
            if (ret!=-1) {
                _LOG(this,D3_DV_GRP0_LEV4, "File '%s'(index %u) found in Directory %u('%s')",Name.c_str(),ret,i,DirInfo->Directory->GetInitString()->c_str());
                *LocalIndex=ret;
                return DirInfo->Directory;
            }
        }
    }
    _LOG(this,D3_DV_GRP0_LEV4, "File '%s' not found ",Name.c_str());
    return NULL;
}


bool CFileManager::Open(int GlobalIndex) {
    CSingleDirectoryInfo *dir = GetDirInfoFromIndex(GlobalIndex);
    if (dir!=NULL) {
        int LocalIndex = GlobalIndex - dir->BaseIndex;
        return dir->Directory->Open(LocalIndex);
    } else {
        return FALSE;
    }
}

bool CFileManager::Close(int GlobalIndex) {
    CSingleDirectoryInfo *dir = GetDirInfoFromIndex(GlobalIndex);
    if (dir!=NULL) {
        int LocalIndex = GlobalIndex - dir->BaseIndex;
        return dir->Directory->Close(LocalIndex);
    } else {
        return FALSE;
    }
}

bool CFileManager::IsOpened(int GlobalIndex) {
    CSingleDirectoryInfo *dir = GetDirInfoFromIndex(GlobalIndex);
    if (dir!=NULL) {
        int LocalIndex = GlobalIndex - dir->BaseIndex;
        return dir->Directory->IsResourceOpened(LocalIndex);
    } else {
        return FALSE;
    }
}

int CFileManager::Read(int GlobalIndex, void *OutBuffer, int Count) {
    CSingleDirectoryInfo *dir = GetDirInfoFromIndex(GlobalIndex);
    if (dir!=NULL) {
        int LocalIndex = GlobalIndex - dir->BaseIndex;
        return dir->Directory->Read(LocalIndex,OutBuffer,Count);
    } else {
        return -1;
    }
}

int CFileManager::Load(int GlobalIndex, void *OutBuffer) {
    CSingleDirectoryInfo *dir = GetDirInfoFromIndex(GlobalIndex);
    if (dir!=NULL) {
        int LocalIndex = GlobalIndex - dir->BaseIndex;
        return dir->Directory->LoadResource(LocalIndex,OutBuffer);
    } else {
        return -1;
    }
}

int CFileManager::GetPos(int GlobalIndex) {
    CSingleDirectoryInfo *dir = GetDirInfoFromIndex(GlobalIndex);
    if (dir!=NULL) {
        int LocalIndex = GlobalIndex - dir->BaseIndex;
        return dir->Directory->GetResourcePos(LocalIndex);
    } else {
        return -1;
    }
}

int CFileManager::GetSize(int GlobalIndex) {
    CSingleDirectoryInfo *dir = GetDirInfoFromIndex(GlobalIndex);
    if (dir!=NULL) {
        int LocalIndex = GlobalIndex - dir->BaseIndex;
        return dir->Directory->GetResourceSize(LocalIndex);
    } else {
        return -1;
    }
}

bool CFileManager::Seek(int GlobalIndex, int SeekOperation, int NewPos) {
    CSingleDirectoryInfo *dir = GetDirInfoFromIndex(GlobalIndex);
    if (dir!=NULL) {
        int LocalIndex = GlobalIndex - dir->BaseIndex;
        return dir->Directory->Seek(LocalIndex,SeekOperation,NewPos);
    } else {
        return FALSE;
    }
}

CBaseDirectory * CFileManager::GetDirectory(int GlobalIndex) {
    return GetDirectoryFromIndex(GlobalIndex);
}


CFileManager::CSingleDirectoryInfo *CFileManager::GetDirInfoFromIndex(int Index) {
    int i=0;
    int ArraySize=ArrayOfDirs->GetSize();
    while (i<ArraySize) {
        CSingleDirectoryInfo *DI=(CSingleDirectoryInfo *)ArrayOfDirs->GetAt(i);
        if ( (Index>=DI->BaseIndex) && 
             (Index<DI->BaseIndex + DI->Directory->GetResourceCount()) ) return DI;
        i++;
    }
    return NULL;
}

CBaseDirectory *CFileManager::GetDirectoryFromIndex(int Index) {
    int i=0;
    int ArraySize=ArrayOfDirs->GetSize();
    while (i<ArraySize) {
        CSingleDirectoryInfo *DI=(CSingleDirectoryInfo *)ArrayOfDirs->GetAt(i);
        if ( (Index>=DI->BaseIndex) && 
             (Index<DI->BaseIndex + DI->Directory->GetResourceCount()) ) return DI->Directory;
        i++;
    }
    return NULL;
}

CFileManager::CSingleDirectoryInfo::~CSingleDirectoryInfo() {
    if (Directory!=NULL) delete Directory;
}
