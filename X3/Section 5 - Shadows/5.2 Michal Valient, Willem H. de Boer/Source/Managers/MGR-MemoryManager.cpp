#include "stdafx.h"
#include "MGR-MemoryManager.h"
#include "MGR-ErrorManager.h"
#include "crtdbg.h"
#include "Global.h"

///Static reference count of MemoryManager
int CMemoryManager::m_MemoryManagerCount=0;
///Pointer to MemoryManager instance
CMemoryManager *MemMgr=NULL;


CMemoryManager *CMemoryManager::GetMemoryManager(char *CallerName) {
    if (m_MemoryManagerCount==0) {
        CErrorManager::GetErrorManager("Mem.MGR","MemManager_Requested_Log_File.log");
        MemMgr = new CMemoryManager();
        m_MemoryManagerCount=1;
    } else {
        m_MemoryManagerCount++;
        _LOG("Mem","Mgr",D3_DV_GRP0_LEV1,"%s called request for MemoryManager. Reference count is %d",CallerName,m_MemoryManagerCount);
    }
    return MemMgr;
}

void CMemoryManager::DeleteMemoryManager(char *CallerName) {
    if ((MemMgr==NULL) || (m_MemoryManagerCount==0)) return;
    else {
        m_MemoryManagerCount--;
        _LOG("Mem","Mgr",D3_DV_GRP0_LEV1,"%s wanted to delete MemoryManager. Reference count is %d",CallerName,m_MemoryManagerCount);
        if (m_MemoryManagerCount==0) {
            delete MemMgr;
            MemMgr=NULL;
            CErrorManager::DeleteErrorManager("Mem.MGR");
        }
    }
}

CMemoryManager::CMemoryManager() {
    _LOG("Mem","Mgr",D3_DV_GRP0_LEV0,"Starting up...");
    m_pDebugInfo=NULL;
    m_iDebugCount=0;
    m_iDebugSize=0;
    m_iHeapSizePeak=0;
    m_iHeapCurrSize=0;
    strcpy(m_sCurrentFile,"");
    strcpy(m_sCurrentFunction,"");
    m_uCurrentLine = 0;
    m_hHeap = HeapCreate(0,4*1024*1024,0);
}

CMemoryManager::~CMemoryManager() {
    int logE = _LOGB("Mem","Mgr",D3_DV_GRP0_LEV0,"Destroying ...");
    LogMemoryStatus();
    for (UINT i=0;i<m_iDebugCount;i++) {
        if (m_pDebugInfo[i].m_sAllocFile!=NULL) free(m_pDebugInfo[i].m_sAllocFile);
        if (m_pDebugInfo[i].m_sAllocFunction!=NULL) free(m_pDebugInfo[i].m_sAllocFunction);
        if (m_pDebugInfo[i].m_pPointer!=NULL) HeapFree(m_hHeap,0,m_pDebugInfo[i].m_pPointer);
    }
    free(m_pDebugInfo);
    HeapDestroy(m_hHeap);
    _LOGE(logE,"Destroyed");
}


void CMemoryManager::LogMemoryStatus() {
    char tmp[1024];
    for (UINT i=0;i<m_iDebugCount;i++) {
        sprintf(tmp,"%s(%u) : MemMgr - Memory was not freed (%u bytes at %p).\n",m_pDebugInfo[i].m_sAllocFile,m_pDebugInfo[i].m_iAllocLine,
            m_pDebugInfo[i].m_iSize,m_pDebugInfo[i].m_pPointer);
        _RPT0(_CRT_WARN,tmp);
//        ErrMgr->Log("Mem","Mgr","MEM_LEAK: %s(%u) : %u bytes at %p.",m_pDebugInfo[i].m_sAllocFile,m_pDebugInfo[i].m_iAllocLine,
//            m_pDebugInfo[i].m_iSize,m_pDebugInfo[i].m_pPointer);
        _LOG("Mem","Mgr",D3_DV_GRP0_LEV0,"MEM_LEAK: %s(%u) : %u bytes at %p.",m_pDebugInfo[i].m_sAllocFile,m_pDebugInfo[i].m_iAllocLine,
            m_pDebugInfo[i].m_iSize,m_pDebugInfo[i].m_pPointer);
    }
    int logE = _LOGB("Mem","Mgr",D3_DV_GRP0_LEV0,"MEMORY_INFO:");
    _LOG("Mem","Mgr",D3_DV_GRP0_LEV0,"Peak heap size: %u",m_iHeapSizePeak);
    _LOG("Mem","Mgr",D3_DV_GRP0_LEV0,"Leak heap size: %u",m_iHeapCurrSize);
    _LOGE(logE);
}

void CMemoryManager::GarbageCollection() {
    if (m_iDebugCount<m_iDebugSize) {
        m_iDebugSize=m_iDebugCount;
        m_pDebugInfo=(sMemDebugInfo *)realloc(m_pDebugInfo,m_iDebugSize*sizeof(sMemDebugInfo));
    }
}

void CMemoryManager::Resize() {
    if (m_iDebugCount>=m_iDebugSize) {
        m_iDebugSize=m_iDebugSize+(4096/sizeof(sMemDebugInfo));
        m_pDebugInfo=(sMemDebugInfo *)realloc(m_pDebugInfo,m_iDebugSize*sizeof(sMemDebugInfo));
        for (UINT i=m_iDebugCount;i<m_iDebugSize;i++) {
            m_pDebugInfo[i].m_iAllocLine=0;
            m_pDebugInfo[i].m_iSize=0;
            m_pDebugInfo[i].m_pPointer=NULL;
            m_pDebugInfo[i].m_sAllocFile=NULL;
        }
    }
}

void CMemoryManager::DeleteField(UINT index) {
    if (index<m_iDebugCount-1) { //not the last one
        m_pDebugInfo[index].m_iAllocLine=m_pDebugInfo[m_iDebugCount-1].m_iAllocLine;
        m_pDebugInfo[index].m_iSize=m_pDebugInfo[m_iDebugCount-1].m_iSize;
        if (m_pDebugInfo[index].m_pPointer!=NULL) xFreeMem_Release(m_pDebugInfo[index].m_pPointer);
        m_pDebugInfo[index].m_pPointer=m_pDebugInfo[m_iDebugCount-1].m_pPointer;
        if (m_pDebugInfo[index].m_sAllocFile!=NULL) free(m_pDebugInfo[index].m_sAllocFile);
        m_pDebugInfo[index].m_sAllocFile=m_pDebugInfo[m_iDebugCount-1].m_sAllocFile;
        if (m_pDebugInfo[index].m_sAllocFunction!=NULL) free(m_pDebugInfo[index].m_sAllocFunction);
        m_pDebugInfo[index].m_sAllocFunction=m_pDebugInfo[m_iDebugCount-1].m_sAllocFunction;
    } else { //the last one
        if (m_pDebugInfo[index].m_pPointer!=NULL) xFreeMem_Release(m_pDebugInfo[index].m_pPointer);
        if (m_pDebugInfo[index].m_sAllocFile!=NULL) free(m_pDebugInfo[index].m_sAllocFile);
        if (m_pDebugInfo[index].m_sAllocFunction!=NULL) free(m_pDebugInfo[index].m_sAllocFunction);
    }
    m_pDebugInfo[m_iDebugCount-1].m_iAllocLine=0;
    m_pDebugInfo[m_iDebugCount-1].m_iSize=0;
    m_pDebugInfo[m_iDebugCount-1].m_pPointer=NULL;
    m_pDebugInfo[m_iDebugCount-1].m_sAllocFile=NULL;
    m_pDebugInfo[m_iDebugCount-1].m_sAllocFunction=NULL;
    m_iDebugCount--;
}

void* CMemoryManager::xAllocMem_Debug(long iSize, long iFlags, char *sFileName, UINT iCodeLine, char *sFunctionName) {
    void *ret = xAllocMem_Release(iSize,iFlags);
    Resize();
    
    m_pDebugInfo[m_iDebugCount].m_iAllocLine=iCodeLine;
    m_pDebugInfo[m_iDebugCount].m_iSize=iSize;
    m_pDebugInfo[m_iDebugCount].m_pPointer=ret;
    m_pDebugInfo[m_iDebugCount].m_sAllocFile = (char*)calloc(strlen(sFileName)+1,1);
    m_pDebugInfo[m_iDebugCount].m_sAllocFunction = (char*)calloc(strlen(sFunctionName)+1,1);
    strcpy(m_pDebugInfo[m_iDebugCount].m_sAllocFile, sFileName);
    strcpy(m_pDebugInfo[m_iDebugCount].m_sAllocFunction, sFunctionName);

    m_iHeapCurrSize+=iSize;
    if (m_iHeapCurrSize>m_iHeapSizePeak) m_iHeapSizePeak=m_iHeapCurrSize;
    _LOG("Mem","Mgr",D3_DV_GRP0_LEV4,"AllocateMem (%p) %u bytes in %s(%u)",m_pDebugInfo[m_iDebugCount].m_pPointer,
        m_pDebugInfo[m_iDebugCount].m_iSize,m_pDebugInfo[m_iDebugCount].m_sAllocFile,
        m_pDebugInfo[m_iDebugCount].m_iAllocLine);
    m_iDebugCount++;
    return ret;
}

void CMemoryManager::xFreeMem_Debug(void *pPointer, char *sFileName, UINT iCodeLine, char *sFunctionName) {
    for (UINT i=0;i<m_iDebugCount;i++) {
        if (m_pDebugInfo[i].m_pPointer==pPointer) { //we found matching pointer
            m_iHeapCurrSize-=m_pDebugInfo[i].m_iSize;
            _LOG("Mem","Mgr",D3_DV_GRP0_LEV4,"FreeMem (%p) %u bytes in %s(%u)",m_pDebugInfo[i].m_pPointer,
                    m_pDebugInfo[i].m_iSize,sFileName,iCodeLine);
            DeleteField(i);
            return;
        }
    }
    _ERROR("Mem","MGR",ErrMgr->TMemory,"Unknown pointer (%p) was tried to be freed in file '%s' at line (%u) in function '%s'",pPointer,sFileName,iCodeLine, sFunctionName);
}

void *CMemoryManager::xReAllocMem_Debug(void *pOld, long iSize, long iFlags, char *sFileName, UINT iCodeLine, char *sFunctionName) {
    _LOG("Mem","Mgr",D3_DV_GRP0_LEV4,"Generic ReAllocMem (%p) %u bytes in %s(%u) func '%s'",pOld,iSize,sFileName,iCodeLine, sFunctionName);
    void *ret = pOld;
    if (pOld==NULL) {
        ret = xAllocMem_Debug(iSize, iFlags, sFileName, iCodeLine, sFunctionName);
    } else {
        if (iSize==0) {
            xFreeMem_Debug(pOld,sFileName,iCodeLine, sFunctionName);
            ret = NULL;
        } else {
            if (iSize<0) _FATAL("Mem","MGR",ErrMgr->TMemory,"Memory reallocation error : size cannot be negative");
            for (UINT i=0;i<m_iDebugCount;i++) {
                if (m_pDebugInfo[i].m_pPointer==pOld) { //we found matching pointer
                    m_iHeapCurrSize-=m_pDebugInfo[i].m_iSize;
                    
                    long flags = 0;
                    if ((iFlags & D3_MEM_DONOTCLEAR)==0) flags = HEAP_ZERO_MEMORY;
                    m_pDebugInfo[i].m_pPointer = HeapReAlloc(m_hHeap,flags,pOld,iSize);
                    if (m_pDebugInfo[i].m_pPointer==NULL) _FATAL("Mem","MGR",ErrMgr->TMemory,"Cannot reallocate memory");

                    m_pDebugInfo[i].m_sAllocFile = (char *)realloc(m_pDebugInfo[i].m_sAllocFile,strlen(sFileName)+1);
                    strcpy(m_pDebugInfo[i].m_sAllocFile,sFileName);
                    m_pDebugInfo[i].m_sAllocFunction = (char *)realloc(m_pDebugInfo[i].m_sAllocFunction,strlen(sFunctionName)+1);
                    strcpy(m_pDebugInfo[i].m_sAllocFunction,sFunctionName);
                    m_pDebugInfo[i].m_iAllocLine=iCodeLine;
                    m_pDebugInfo[i].m_iSize=iSize;
                    m_iHeapCurrSize+=m_pDebugInfo[i].m_iSize;
                    if (m_iHeapCurrSize>m_iHeapSizePeak) m_iHeapSizePeak=m_iHeapCurrSize;
                    _LOG("Mem","Mgr",D3_DV_GRP0_LEV4,"ReAllocate (%p) %u bytes in %s(%u)",m_pDebugInfo[i].m_pPointer,
                            m_pDebugInfo[i].m_iSize,m_pDebugInfo[i].m_sAllocFile,
                            m_pDebugInfo[i].m_iAllocLine);
                    return m_pDebugInfo[i].m_pPointer;
                }
            }
            _ERROR("Mem","MGR",ErrMgr->TMemory,"Unknown pointer (%p) was tried to be reallocated in file '%s' at line (%u) in function '%s'",pOld,sFileName,iCodeLine, sFunctionName);
            return NULL;
        }
    }
    return ret;
}

void* CMemoryManager::xNew_Debug(size_t iSize) {
    void *ret = xNew_Release(iSize);
    Resize();
    m_pDebugInfo[m_iDebugCount].m_iAllocLine=m_uCurrentLine;
    m_pDebugInfo[m_iDebugCount].m_iSize=iSize;
    m_pDebugInfo[m_iDebugCount].m_pPointer=ret;
    m_pDebugInfo[m_iDebugCount].m_sAllocFile = (char *)calloc(strlen(m_sCurrentFile)+1,1);
    m_pDebugInfo[m_iDebugCount].m_sAllocFunction = (char *)calloc(strlen(m_sCurrentFunction)+1,1);
    strcpy(m_pDebugInfo[m_iDebugCount].m_sAllocFile,m_sCurrentFile);
    strcpy(m_pDebugInfo[m_iDebugCount].m_sAllocFunction,m_sCurrentFunction);
    m_iHeapCurrSize+=m_pDebugInfo[m_iDebugCount].m_iSize;
    if (m_iHeapCurrSize>m_iHeapSizePeak) m_iHeapSizePeak=m_iHeapCurrSize;
    _LOG("Mem","Mgr",D3_DV_GRP0_LEV4,"New (%p) %u bytes in %s(%u)",m_pDebugInfo[m_iDebugCount].m_pPointer,
            m_pDebugInfo[m_iDebugCount].m_iSize,m_pDebugInfo[m_iDebugCount].m_sAllocFile,
            m_pDebugInfo[m_iDebugCount].m_iAllocLine);
    m_iDebugCount++;
    return ret;
}

void CMemoryManager::xDelete_Debug(void *pPointer) {
    for (UINT i=0;i<m_iDebugCount;i++) {
        if (m_pDebugInfo[i].m_pPointer==pPointer) { //we found matching pointer
            m_iHeapCurrSize-=m_pDebugInfo[i].m_iSize;
            _LOG("Mem","Mgr",D3_DV_GRP0_LEV4,"Delete (%p) %u bytes",m_pDebugInfo[i].m_pPointer,m_pDebugInfo[i].m_iSize);
            DeleteField(i);
            return;
        }
    }
    _ERROR("Mem","MGR",ErrMgr->TMemory,"Unknown pointer (%p) was tried to be deleted in file '%s' at line (%u) in function %s",pPointer,m_sCurrentFile,m_uCurrentLine, m_sCurrentFunction);
}


///Release version of AllocMem
void *CMemoryManager::xAllocMem_Release(long iSize, long iFlags) {
    if (iSize<=0) _FATAL("Mem","MGR",ErrMgr->TMemory,"Memory allocation error : size cannot be negative or zero");
    long flags = 0;
    if ((iFlags & D3_MEM_DONOTCLEAR)==0) flags = HEAP_ZERO_MEMORY;
    void *ret = HeapAlloc(m_hHeap,flags,iSize);
    if (ret==NULL) _FATAL("Mem","MGR",ErrMgr->TMemory,"Cannot allocate memory");
    return ret;
}

///Release version of FreeMem
void CMemoryManager::xFreeMem_Release(void *pPointer) {
    if (HeapFree(m_hHeap,0,pPointer)==0) _FATAL("Mem","MGR",ErrMgr->TMemory,"Cannot free memory");
}

///Release version of ReAllocMem
void *CMemoryManager::xReAllocMem_Release(void *pOld, long iSize, long iFlags) {
    void *ret = pOld;
    if (pOld==NULL) {
        ret = xAllocMem_Release(iSize,iFlags);
    } else {
        if (iSize==0) {
            xFreeMem_Release(pOld);
            ret = NULL;
        } else {
            if (iSize<0) _FATAL("Mem","MGR",ErrMgr->TMemory,"Memory reallocation error : size cannot be negative");
            long flags = 0;
            if ((iFlags & D3_MEM_DONOTCLEAR)==0) flags = HEAP_ZERO_MEMORY;
            ret = HeapReAlloc(m_hHeap,flags,pOld,iSize);
            if (ret==NULL) _FATAL("Mem","MGR",ErrMgr->TMemory,"Cannot reallocate memory");
        }
    }
    return ret;
}

///Release version of new
void *CMemoryManager::xNew_Release(size_t iSize) {
    void *ret = HeapAlloc(m_hHeap,HEAP_ZERO_MEMORY,iSize);
    if (ret==NULL) _FATAL("Mem","MGR",ErrMgr->TMemory,"Cannot allocate memory for new object");
    return ret;
}

///Release version of delete
void CMemoryManager::xDelete_Release(void *pPointer) {
    if (HeapFree(m_hHeap,0,pPointer)==0) _FATAL("Mem","MGR",ErrMgr->TMemory,"Cannot free memory of object");
}


void CMemoryManager::SetOwner(char *File, UINT Line, char *Function) {
    if (m_MemoryManagerCount>0) {
        strcpy(m_sCurrentFile, File);
        m_uCurrentLine = Line;
        strcpy(m_sCurrentFunction, Function);
    }
}
