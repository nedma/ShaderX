//############################################################################
//# BaseDirectory is common root for all directory like interfaces
//############################################################################

#pragma once

#include "BaseClass.h"

//############################################################################
//Forward declarations
class CMString;

const int VSEEK_ABSOLUTE        = 1;
const int VSEEK_RELATIVE_POS    = 2;
const int VSEEK_RELATIVE_END    = 3;

class CBaseDirectory : public CBaseClass {
protected:
    CMString *InitString;
public:
    //settings functions
    //Will return TRUE, if Initialise was success
    virtual bool IsInitialised() = 0;
    //Will return init string
    D3_INLINE CMString *GetInitString() const {
        return InitString;
    };

    //global information functions
    //Will return count of resources in care.
    virtual int GetResourceCount() = 0;

    //Resource Manipulation
    //Will open resource and return TRUE if success
    virtual bool Open(int ResourceIndex) = 0;
    //Will close resource and return TRUE if success
    virtual bool Close(int ResourceIndex) = 0;
    //Will move cursor within resource and return TRUE if success
    virtual bool Seek(int ResourceIndex, int SeekOperation, int NewPos) = 0; 

    //Resource Loading
    //Will read iCount from resource into pOutBuffer and returns count of bytes really readed.
    virtual int Read(int ResourceIndex, void *OutBuffer, int Count) = 0;
    //Will read whole resource into pOutBuffer and returns count of bytes really readed.
    virtual int LoadResource(int ResourceIndex, void *OutBuffer) = 0;

    //RESOURCE Informations...
    //Will return TRUE if resource is Opened.
    virtual bool IsResourceOpened(int ResourceIndex) = 0;
    //Will return size of resource. Returns -1 on error.
    virtual int GetResourceSize(int Index) = 0;
    //Will return cursor pos inside resource. Returns -1 on error.
    virtual int GetResourcePos(int Index) = 0;
    //Will return index (handle) of resource by given name. Returns -1 on error.
    virtual int GetResourceIndex(CMString &ResourceName) = 0;

    int GetResourceSize(CMString &ResourceName) {
        int r = GetResourceIndex(ResourceName);
        if (r>=0) return GetResourceSize(r);
        else return -1;
    }

    int GetResourcePos(CMString &ResourceName) {
        int r = GetResourceIndex(ResourceName);
        if (r>=0) return GetResourcePos(r);
        else return -1;
    }

    //Constructor, Destructor and Init and Reset
    //Init with new string. TRUE=succ.
    virtual bool Initialise(CMString &NewInitString) = 0;
    //Back to "after construction" state. After this IsInitialised _MUST_ return FALSE
    virtual void Reset() = 0;
    CBaseDirectory(char *ObjName, CEngine *OwnerEngine);
    virtual ~CBaseDirectory();
    MAKE_CLSNAME("CBaseDirectory");
};
