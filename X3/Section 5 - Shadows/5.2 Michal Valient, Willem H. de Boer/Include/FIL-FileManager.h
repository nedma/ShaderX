//############################################################################
//# FileManager unifies access to all files, no matter where they are or how
//# they are stored on disk (or if they are really stored on disk)
//############################################################################


#pragma once

#include "MGR-BaseManager.h"

//############################################################################
//Forward declarations
class CRootClassArray;
class CBaseDirectory;
class CMString;

class CFileManager : public CBaseManager {
protected:
    CRootClassArray  *ArrayOfDirs;
    UINT            NextFreeBase;

private:

    class CSingleDirectoryInfo : public CRootClass {
    public:
        CBaseDirectory     *Directory;
        bool                Enabled;
        int                 BaseIndex;
        
        CSingleDirectoryInfo() {
            Directory=NULL;
            Enabled=FALSE;
            BaseIndex=0;
        }
        virtual ~CSingleDirectoryInfo();
    };

    CSingleDirectoryInfo *GetDirInfoFromIndex(int Index);
    CBaseDirectory *GetDirectoryFromIndex(int Index);
public:
	CBaseDirectory * GetDirectory(int GlobalIndex);
	bool Seek(int GlobalIndex, int SeekOperation, int NewPos);
	int GetSize(int GlobalIndex);
	int GetPos(int GlobalIndex);
	int Load(int GlobalIndex, void *OutBuffer);
	int Read(int GlobalIndex, void *OutBuffer, int Count);
	bool IsOpened(int GlobalIndex);
	bool Close(int GlobalIndex);
	bool Open(int GlobalIndex);
	CBaseDirectory * Find(int *LocalIndex, CMString &Name);
	int Find(CMString &Name);
	int AddNewDirectory(CBaseDirectory &NewDir);

	CFileManager(char *ObjName,CEngine *OwnerEngine);
	virtual ~CFileManager();

    MAKE_CLSCREATE(CFileManager);
    MAKE_CLSNAME("CFileManager");
    MAKE_DEFAULTGARBAGE();
};

