//############################################################################
//# BaseManager is common root for all managers
//############################################################################

#pragma once

#include "BaseClass.h"

//############################################################################
//Forward declarations


class CBaseManager : public CBaseClass {
public:
    CBaseManager(char *pObjName,CEngine *pOwnerEngine);
    virtual ~CBaseManager();
    MAKE_CLSNAME("CBaseManager");
};
