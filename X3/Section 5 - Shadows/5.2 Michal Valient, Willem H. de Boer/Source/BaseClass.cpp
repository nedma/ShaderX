// BaseClass.cpp: implementation of the CBaseClass class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaseClass.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseClass::CBaseClass(char *ObjName,CEngine *OwnerEngine) {
    this->OwnerEngine = OwnerEngine;
    ObjectName = NULL;
    SetObjName(ObjName);
}

CBaseClass::~CBaseClass() {
    if (ObjectName!=NULL) free(ObjectName);
}
