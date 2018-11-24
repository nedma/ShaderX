#pragma once
#include "baseclass.h"

//############################################################################
//Forward declarations
class CStrHashTable;

class CRuntimeCreator : public CBaseClass {
protected:
    class CRuntimeInfo : public CRootClass {
    public:
        CBaseClass *ExistingInstance;   //existing instance of this class.
                                        //This instance is entirely managed by RuntimeCreator
                                        //and cannot be used (referenced) in runtime by nothing other.
        void       *RegisterInfo;       //This pointer is passed to Create method of class.
                                        //It is optional and is defined in RegisterClass method
                                        //It has to be allocated by MemMgr->AllocMem and 
                                        //is entirely managed by RuntimeCreator and cannot be used 
                                        //(referenced) in runtime by nothing other.
        CRuntimeInfo() {
            RegisterInfo = ExistingInstance = NULL;
        }

        virtual ~CRuntimeInfo() {
            if (ExistingInstance!=NULL) delete ExistingInstance;
            if (RegisterInfo!=NULL) MemMgr->FreeMem(RegisterInfo);
        }
    };

    CStrHashTable  *Classes;
public:
    //Class is registered by system.
    bool RegisterClass(CBaseClass *Class, void *RegisterInfo);
    //Class is created by RuntimeCreator. Parameters are passed to Create method of class.
    CBaseClass *CreateClass(char *ClassName, char *ObjectName, void *Parameters);

    CRuntimeCreator(char *ObjName,CEngine *OwnerEngine);
    virtual ~CRuntimeCreator(void);

    MAKE_CLSCREATE(CRuntimeCreator);
    MAKE_CLSNAME("CRuntimeCreator");
    MAKE_DEFAULTGARBAGE();
};
