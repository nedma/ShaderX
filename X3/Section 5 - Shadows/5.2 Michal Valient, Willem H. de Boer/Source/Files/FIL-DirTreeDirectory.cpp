// DirTreeDirectory.cpp: implementation of the CDirTreeDirectory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FIL-DirTreeDirectory.h"
#include "SU-StrUtils.h"
#include "SC-MString.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirTreeDirectory::CDirTreeDirectory(char *ObjName,CEngine *OwnerEngine) : CBaseDirectory(ObjName, OwnerEngine) {
    int logE = _LOGB(this,D3_DV_GRP1_LEV0,"Created");
    DirCount=0;
    FilesCount=0;
    Initialised=FALSE;
    _LOGE(logE);
}

CDirTreeDirectory::~CDirTreeDirectory() {
    int logE = _LOGB(this,D3_DV_GRP1_LEV0,"Destroying for '%s'",InitString->c_str());
    Reset();
    _LOGE(logE,"Destroyed");
}

void CDirTreeDirectory::Reset() {
    int logE = _LOGB(this,D3_DV_GRP1_LEV0,"Reseting and deallocating memory");
    if (DirCount!=0) {
        for (int i=0; i<DirCount;i++) 
            if (Dirs[i]!=NULL) MemMgr->FreeMem(Dirs[i]);
        MemMgr->FreeMem(Dirs);
    }
    if (FilesCount!=0) {
        for (int i=0;i<FilesCount;i++) {
            if (Files[i].ResourceName!=NULL) MemMgr->FreeMem(Files[i].ResourceName);
            if (Files[i].FileHandle!=NULL) fclose(Files[i].FileHandle);
        }
        MemMgr->FreeMem(Files);
    }
    DirCount=0;
    FilesCount=0;
    Initialised=FALSE;
    _LOGE(logE,"Reseted");
}

bool CDirTreeDirectory::Initialise(CMString &NewInitString) {
    int logE = _LOGB(this,D3_DV_GRP1_LEV0,"Initialising for %s",NewInitString.c_str());
    *InitString = NewInitString;
    Reset();
    GetTreeCounts(*InitString,DirCount,FilesCount);
    Dirs = (char**)MemMgr->AllocMem(DirCount*sizeof(char *),0);
    Files = (sResourceInfo *)MemMgr->AllocMem(FilesCount * sizeof(sResourceInfo), 0);
    int pd=0,pf=0;
    AddTree(*InitString,pd,pf);
    Initialised=TRUE;
    _LOG(this,D3_DV_GRP1_LEV0,"Initialised for %u files in %u dirs",FilesCount,DirCount);
    _LOGE(logE,"Initialised");
    return TRUE;
}

void CDirTreeDirectory::GetTreeCounts(CMString &Dir, int &DirCount, int &FileCount) {
    WIN32_FIND_DATA *d;
    HANDLE h;
    CMString srchtmp = Dir + "\\*.*";

    int FileCountTmp = 0;
    int SubDirCountTmp = 0;

    d = (WIN32_FIND_DATA *)MemMgr->AllocMem(sizeof(WIN32_FIND_DATA),0);
    h = FindFirstFile(srchtmp.c_str(),d);
    bool ContinueLoop = TRUE;
    while ((h!=INVALID_HANDLE_VALUE) && (ContinueLoop)) {
        if ((d->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=0) {
            if (d->cFileName[0]!='.') {
                CMString tmp = Dir + "\\"+ d->cFileName;
                GetTreeCounts(tmp,DirCount,FileCount);
                SubDirCountTmp++;
            }
        } else {
            FileCountTmp++;
        }
        ContinueLoop = (FindNextFile(h,d) == TRUE) ? true : false;
    }
    FindClose(h);
    MemMgr->FreeMem(d);
    _LOG(this,D3_DV_GRP1_LEV4,"Count found %u files and %u subdirs in '%s'",FileCountTmp,SubDirCountTmp,Dir.c_str());
    if (FileCountTmp>0) {
        DirCount=DirCount++;
        FileCount=FileCount+FileCountTmp;
    }
}

bool CDirTreeDirectory::Open(int ResourceIndex) {
    if (IsValidIndex(ResourceIndex)) {
        if (Files[ResourceIndex].FileHandle==NULL) {
            CMString tmp = Files[ResourceIndex].ResourceDir;
            tmp += "\\";
            tmp +=Files[ResourceIndex].ResourceName;
            Files[ResourceIndex].FileHandle = fopen(tmp.c_str(),"rb");
            Files[ResourceIndex].Pos=0;
            if (Files[ResourceIndex].FileHandle!=NULL) {
                _LOG(this,D3_DV_GRP1_LEV4,"Opened (physicaly) resource %u(%s)",ResourceIndex,tmp.c_str());
                return TRUE;
            } else {
                _LOG(this,D3_DV_GRP1_LEV0,"Failed to open resource %u(%s)",ResourceIndex,tmp.c_str());
                return FALSE;
            }
        } else {
            Seek(ResourceIndex,VSEEK_ABSOLUTE,0);
            _LOG(this,D3_DV_GRP1_LEV4,"Opened (virtualy) resource %u(%s\\%s)",ResourceIndex,Files[ResourceIndex].ResourceDir,Files[ResourceIndex].ResourceName);
            return TRUE;
        }
    } else {
        _LOG(this,D3_DV_GRP1_LEV0,"Resource index %u is out of bounds",ResourceIndex);
        return FALSE;
    }
}

bool CDirTreeDirectory::Close(int ResourceIndex) {
    if (IsValidIndex(ResourceIndex)) {
        if (Files[ResourceIndex].FileHandle==NULL) {
            Files[ResourceIndex].Pos=0;
            _LOG(this,D3_DV_GRP1_LEV4,"Closed (virtualy) resource %u(%s\\%s)",ResourceIndex,Files[ResourceIndex].ResourceDir,Files[ResourceIndex].ResourceName);
            return TRUE;
        } else {
            Files[ResourceIndex].Pos=0;
            if (fclose(Files[ResourceIndex].FileHandle)==0) {
                _LOG(this,D3_DV_GRP1_LEV4,"Closed (physicaly) resource %u(%s\\%s)",ResourceIndex,Files[ResourceIndex].ResourceDir,Files[ResourceIndex].ResourceName);
                Files[ResourceIndex].FileHandle=NULL;
                return TRUE;
            } else {
                _LOG(this,D3_DV_GRP1_LEV0,"Failed to close resource %u(%s\\%s)",ResourceIndex,Files[ResourceIndex].ResourceDir,Files[ResourceIndex].ResourceName);
                return FALSE;
            }
        }
    } else {
        _LOG(this,D3_DV_GRP1_LEV0,"Resource index %u is out of bounds",ResourceIndex);
        return FALSE;
    }
}

bool CDirTreeDirectory::Seek(int ResourceIndex, int SeekOperation, int NewPos) {
    if (IsValidIndex(ResourceIndex)) {
        if (Files[ResourceIndex].FileHandle==NULL) {
            _LOG(this,D3_DV_GRP1_LEV0,"Cannot work on closed resource %u(%s\\%s)",ResourceIndex,Files[ResourceIndex].ResourceDir,Files[ResourceIndex].ResourceName);
            return FALSE;
        } else {
            unsigned int NewPosTmp;
            //determine new position
            switch (SeekOperation) {
                case VSEEK_RELATIVE_END : {
                    if (NewPos<0) NewPosTmp=Files[ResourceIndex].Size;
                    else if (Files[ResourceIndex].Size<(unsigned int)NewPos) NewPosTmp=0;
                    else NewPosTmp=Files[ResourceIndex].Size-NewPos;
                } break;
                case VSEEK_RELATIVE_POS : NewPosTmp=Files[ResourceIndex].Pos+NewPos; break;
                default: NewPosTmp=NewPos;
            }
            if (NewPosTmp==Files[ResourceIndex].Pos) {
                _LOG(this,D3_DV_GRP1_LEV4,"Seek not moved in resource %u(%s\\%s)",ResourceIndex,Files[ResourceIndex].ResourceDir,Files[ResourceIndex].ResourceName);
                return TRUE;   //return if we are on that position
            }
            if (fseek(Files[ResourceIndex].FileHandle,NewPosTmp,SEEK_SET)==0) {
                _LOG(this,D3_DV_GRP1_LEV4,"Seek (physicaly) moved in resource %u(%s\\%s)",ResourceIndex,Files[ResourceIndex].ResourceDir,Files[ResourceIndex].ResourceName);
                _LOG(this,D3_DV_GRP1_LEV4,"    Operation '%u' with pos '%d' moved from '%u' to '%u'",SeekOperation,NewPos,Files[ResourceIndex].Pos,NewPosTmp);
                Files[ResourceIndex].Pos = NewPosTmp;
                return TRUE;
            } else {
                _LOG(this,D3_DV_GRP1_LEV0,"Failed to seek resource %u(%s\\%s)",ResourceIndex,Files[ResourceIndex].ResourceDir,Files[ResourceIndex].ResourceName);
                return FALSE;
            }
        }
    } else {
        _LOG(this,D3_DV_GRP1_LEV0,"Resource index %u is out of bounds",ResourceIndex);
        return FALSE;
    }
}

int CDirTreeDirectory::Read(int ResourceIndex, void *OutBuffer, int Count) {
    if (IsValidIndex(ResourceIndex)) {
        if (Files[ResourceIndex].FileHandle==NULL) {
            _LOG(this,D3_DV_GRP1_LEV0,"Cannot work on closed resource %u(%s\\%s)",ResourceIndex,Files[ResourceIndex].ResourceDir,Files[ResourceIndex].ResourceName);
            return -1;
        } else {
            int Readed = fread(OutBuffer,1,Count,Files[ResourceIndex].FileHandle);
            if (Readed>0) Files[ResourceIndex].Pos+=Readed;
            return Readed;
        }
    } else {
        _LOG(this,D3_DV_GRP1_LEV0,"Resource index %u is out of bounds",ResourceIndex);
        return -1;
    }
}

int CDirTreeDirectory::LoadResource(int ResourceIndex, void *OutBuffer) {
    if (IsValidIndex(ResourceIndex)) {
        if (Files[ResourceIndex].FileHandle==NULL) {
            if (Open(ResourceIndex)) {
                int Readed = Read(ResourceIndex,OutBuffer,Files[ResourceIndex].Size);
                _LOG(this,D3_DV_GRP1_LEV4,"Whole resource %u(%s\\%s) loaded (%u bytes)",ResourceIndex,Files[ResourceIndex].ResourceDir,Files[ResourceIndex].ResourceName,Readed);
                Close(ResourceIndex);
                return Readed;
            } else {
                return -1;
            }
        } else {
            UINT OldPos = Files[ResourceIndex].Pos;
            if (Seek(ResourceIndex,VSEEK_ABSOLUTE,0)) {
                int Readed = Read(ResourceIndex,OutBuffer,Files[ResourceIndex].Size);
                _LOG(this,D3_DV_GRP1_LEV4,"Whole resource %u(%s\\%s) loaded (%u bytes)",ResourceIndex,Files[ResourceIndex].ResourceDir,Files[ResourceIndex].ResourceName,Readed);
                Seek(ResourceIndex,VSEEK_ABSOLUTE,OldPos);
                return Readed;
            } else {
                return -1;
            }
        }
    } else {
        _LOG(this,D3_DV_GRP1_LEV0,"Resource index %u is out of bounds",ResourceIndex);
        return -1;
    }
}

int CDirTreeDirectory::GetResourceSize(int ResourceIndex) {
    if (IsValidIndex(ResourceIndex)) {
        return Files[ResourceIndex].Size;
    } else {
        _LOG(this,D3_DV_GRP1_LEV0,"Resource index %u is out of bounds",ResourceIndex);
        return -1;
    }
}

int CDirTreeDirectory::GetResourcePos(int ResourceIndex) {
    if (IsValidIndex(ResourceIndex)) {
        return Files[ResourceIndex].Pos;
    } else {
        _LOG(this,D3_DV_GRP1_LEV0,"Resource index %u is out of bounds",ResourceIndex);
        return -1;
    }
}

int CDirTreeDirectory::GetResourceIndex(CMString &ResourceName) {
    CMString tmp = ResourceName;
    _strlwr(tmp.c_str());
    UINT para_crc = suStrCheckSum(tmp.c_str());
    for (int i=0;i<FilesCount;i++) {
        if (para_crc==Files[i].Checksum) {
            _LOG(this,D3_DV_GRP1_LEV4,"(GetResourceIndex) Searched (%s) CRC 0x%X meets CRC of %s",tmp.c_str(),
                para_crc,Files[i].ResourceName);
            if (strcmp(Files[i].ResourceName,tmp.c_str())==0) {
                return i;
            }
        }
    }
    return -1;
}

bool CDirTreeDirectory::IsValidIndex(int Index) {
    if (Index>=FilesCount || Index<0) return FALSE;
    else return TRUE;
}


void CDirTreeDirectory::AddTree(CMString &Dir, int &CurDir, int &CurFile) {
    WIN32_FIND_DATA *d;
    HANDLE h;
    CMString srchtmp = Dir+"\\*.*";
    int FileCountTmp = 0;
    bool ContinueLoop = TRUE;
    char *DirPtr = NULL;

    d = (WIN32_FIND_DATA *)MemMgr->AllocMem(sizeof(WIN32_FIND_DATA),0);
    h = FindFirstFile(srchtmp.c_str(),d);
    while ((h!=INVALID_HANDLE_VALUE) && (ContinueLoop)) {
        if ((d->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=0) {
            if (d->cFileName[0]!='.') {
                CMString tmp = Dir + "\\"+d->cFileName;
                AddTree(tmp,CurDir,CurFile);
            }
        } else {
            if (FileCountTmp==0) { //first file, so add Dir to List and inc other stuff
                DirPtr = Dirs[CurDir] = suStrReallocCopy(Dirs[CurDir],Dir.c_str());
                CurDir=CurDir++;
            }
            FileCountTmp++;
            Files[CurFile].ResourceDir=DirPtr;
            Files[CurFile].ResourceName = suStrReallocCopy(Files[CurFile].ResourceName,d->cFileName);
            _strlwr(Files[CurFile].ResourceName);
            Files[CurFile].Checksum = suStrCheckSum(Files[CurFile].ResourceName);
            Files[CurFile].Pos = 0;
            if (d->nFileSizeHigh!=0) _ERROR(this,ErrMgr->TNoSupport,"Sorry, but files greater than 4GB are not currently supported");
            Files[CurFile].Size = d->nFileSizeLow;
            Files[CurFile].FileHandle=NULL;
            _LOG(this,D3_DV_GRP1_LEV4,"Added resource '%s' from dir '%s' (%u bytes)",
                Files[CurFile].ResourceName,Files[CurFile].ResourceDir,Files[CurFile].Size);
            CurFile=CurFile+1;
        }
        ContinueLoop = (FindNextFile(h,d) == TRUE) ? true : false;
    }
    FindClose(h);
    MemMgr->FreeMem(d);
}

int CDirTreeDirectory::GetResourceCount() {
    return FilesCount;
}

bool CDirTreeDirectory::IsResourceOpened(int ResourceIndex) {
    if (IsValidIndex(ResourceIndex)) {
        if (Files[ResourceIndex].FileHandle!=NULL) return TRUE;
        else return FALSE;
    } else {
        _LOG(this,D3_DV_GRP1_LEV0,"Resource index %u is out of bounds",ResourceIndex);
        return FALSE;
    }
}
