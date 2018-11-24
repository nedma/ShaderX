//############################################################################
//# DirTreeDirectory manages resources as files in directory tree.
//# Note: It manages files only by names, so take care of duplicate names
//#       the second one will be ignored.
//############################################################################


#pragma once

#include "FIL-BaseDirectory.h"

//############################################################################
//Forward declarations

class CDirTreeDirectory : public CBaseDirectory {
private:
    typedef struct sResourceInfoTag {
        char   *ResourceName;
        char   *ResourceDir;
        UINT    Size;
        UINT    Pos;
        UINT    Checksum;
        FILE   *FileHandle;
    } sResourceInfo;

    int             DirCount;
    char          **Dirs;
    int             FilesCount;
    sResourceInfo  *Files;
    bool            Initialised;
    
    void GetTreeCounts(CMString &Dir, int &DirCount, int &FileCount);
    void AddTree(CMString &Dir, int &CurDir, int &CurFile);
    inline bool IsValidIndex(int Index);
public:
    virtual bool IsInitialised() { return Initialised; }
    virtual int GetResourceCount();
    virtual bool Open(int ResourceIndex);
    virtual bool Close(int ResourceIndex);
    virtual bool Seek(int ResourceIndex, int SeekOperation, int NewPos);

    //Partial Resource Loading
    virtual int Read(int ResourceIndex, void *OutBuffer, int Count);

    //Full Resource loading
    virtual int LoadResource(int ResourceIndex, void *OutBuffer);

    //RESOURCE Informations...
    virtual int GetResourceSize(int ResourceIndex);
    virtual int GetResourcePos(int ResourceIndex);
    virtual int GetResourceIndex(CMString &ResourceName);
    virtual bool IsResourceOpened(int ResourceIndex);

    virtual bool Initialise(CMString &NewInitString);
    virtual void Reset();
    
    CDirTreeDirectory(char *ObjName,CEngine *OwnerEngine);
    virtual ~CDirTreeDirectory();
    
    MAKE_CLSCREATE(CDirTreeDirectory);
    MAKE_CLSNAME("CDirTreeDirectory");
    MAKE_DEFAULTGARBAGE();
};
