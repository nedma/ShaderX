/*
Used to enable memory manager macros
*/

#ifdef	new
#undef	new
#endif

#ifdef	delete
#undef	delete
#endif

//. Debug Memmanager versions...
#ifdef D3_DEBUG_MEMORY_MANAGER
    #define AllocMem(X,Y) xAllocMem_Debug(X,Y,__FILE__,__LINE__, __FUNCTION__)
    #define FreeMem(X) xFreeMem_Debug(X,__FILE__,__LINE__, __FUNCTION__)
    #define ReAllocMem(X,Y,Z) xReAllocMem_Debug(X,Y,Z,__FILE__,__LINE__, __FUNCTION__)
    #define new	(MemMgr->SetOwner(__FILE__,__LINE__,__FUNCTION__),false) ? NULL : new
    #define delete (MemMgr->SetOwner(__FILE__,__LINE__,__FUNCTION__),false) ? MemMgr->SetOwner("",0,"") : delete
#else
    #define AllocMem(X,Y) xAllocMem_Release(X,Y)
    #define FreeMem(X) xFreeMem_Release(X)
    #define ReAllocMem(X,Y,Z) xReAllocMem_Release(X,Y,Z)
#endif //D3_DEBUG_MEMORY_MANAGER