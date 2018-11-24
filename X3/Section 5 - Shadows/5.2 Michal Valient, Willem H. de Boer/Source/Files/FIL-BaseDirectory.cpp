// BaseDirectory.cpp: implementation of the CBaseDirectory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FIL-BaseDirectory.h"
#include "SC-MString.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseDirectory::CBaseDirectory(char *pObjName, CEngine *pOwnerEngine) : CBaseClass (pObjName,pOwnerEngine) {
    InitString = new CMString;
}

CBaseDirectory::~CBaseDirectory() {
    delete InitString;
}
