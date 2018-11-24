// RootClass.cpp: implementation of the CRootClass class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RootClass.h"

#include "nommgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRootClass::CRootClass() {
}

CRootClass::~CRootClass() {
}

void *CRootClass::operator new(size_t s) {
    if (MemMgr==NULL) return malloc(s);
#ifndef D3_DEBUG_MEMORY_MANAGER
    else return MemMgr->xNew_Release(s);
#else
    else return MemMgr->xNew_Debug(s);
#endif
}

void CRootClass::operator delete(void *p) {
    if (MemMgr==NULL) free(p);
#ifndef D3_DEBUG_MEMORY_MANAGER
    else MemMgr->xDelete_Release(p);
#else
    else MemMgr->xDelete_Debug(p);
#endif
}


