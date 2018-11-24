#pragma once

#include "nommgr.h"

//Flags for memory manager
///Instructs memory manager to leave allocated memory uncleaned.
const long D3_MEM_DONOTCLEAR=1;

/**
 * @brief Managed memory manipulation.
 * @author Michal Valient
 * @version 1.0
 * 
 * Memory manager provides allocation / deallocation and reallocation of memory buffers,
 * routimes for creation / destroyment of objects.
 * In debug builds it validates freed pointers and reports memory leaks.
 * There is only one memory manager per application.
 * For object creation use new and delete operators
 */
class CMemoryManager {
private:
    ///debug structure for heap manager.
    typedef struct sMemDebugInfoTag {
        void       *m_pPointer;
        UINT        m_iSize;
        char       *m_sAllocFile;
        UINT        m_iAllocLine;
        char       *m_sAllocFunction;
    } sMemDebugInfo;

    ///This is reference count.
    static int      m_MemoryManagerCount;
    ///Windows heap handle
    HANDLE          m_hHeap;
    ///Pointer to memory informations
    sMemDebugInfo  *m_pDebugInfo;
    ///Count of items in m_pDebugInfo.
    UINT            m_iDebugCount;
    ///Real capacity of m_pDebugInfo.
    UINT            m_iDebugSize;
    ///Peak heap size.
    UINT            m_iHeapSizePeak;
    ///Current heap size.
    UINT            m_iHeapCurrSize;

    ///Current file
    char            m_sCurrentFile[2048];
    ///Current line
    UINT            m_uCurrentLine;
    ///Current function
    char            m_sCurrentFunction[2048];
    
    ///Write info about memory leaks into log file
    void LogMemoryStatus();
    ///Resize internal debug buffers
    void Resize();
    ///Delete field from internal debug buffers
    void DeleteField(UINT index);
    ///Debug version of AllocMem

    ///Protected constructor. Creates heap and initializes debug buffers
    CMemoryManager();
    ///Protected destructor. Frees heap and other memory. Reports leaks.
    ~CMemoryManager();
public:
    void* xAllocMem_Debug(long iSize, long iFlags, char *sFileName, UINT iCodeLine, char *sFunctionName);
    ///Debug version of FreeMem
    void xFreeMem_Debug(void *pPointer, char *sFileName, UINT iCodeLine, char *sFunctionName);
    ///Debug version of ReAllocMem
    void *xReAllocMem_Debug(void *pOld, long iSize, long iFlags, char *sFileName, UINT iCodeLine, char *sFunctionName);
    ///Debug version of new
    void* xNew_Debug(size_t iSize);
    ///Debug version of delete
    void xDelete_Debug(void *pPointer);

    /**
     * @brief Returns valid instance of MemoryManager.
     *
     * If no instance of memory manager is active, it creates new one. Reference count will be 1<br>
     * If MemoryManager is active it returns that one and increments reference count.
     * @param CallerName Name of object, that called this method.
     * @return Pointer to CMemoryManager object
     * @retval NULL on error
     */
    static CMemoryManager *GetMemoryManager(char *CallerName);

    /**
     * @brief Deletes MemoryManager.
     *
     * If reference count of MemoryManager is 1, it destroys memory manager. 
     * Otherwise it decrements reference count.
     * @param CallerName Name of object, that called this method.
     */
    static void DeleteMemoryManager(char *CallerName);

    ///This method performs deallocation of all unused memory
    void GarbageCollection();

    ///Release version of AllocMem
    void *xAllocMem_Release(long iSize, long iFlags);

    ///Release version of FreeMem
    void xFreeMem_Release(void *pPointer);

    ///Release version of ReAllocMem
    void *xReAllocMem_Release(void *pOld, long iSize, long iFlags);

    ///Release version of new
    void *xNew_Release(size_t iSize);

    ///Release version of delete
    void xDelete_Release(void *pPointer);

    /**
     * @brief Memory allocation.
     *
     * Allocates buffer of memory. In debug builds info about file and line of code
     * (where this method was called), allocated pointer and it's size is stored.
     * @param iSize Size of buffer to allocate
     * @param iFlags Flags for creation. See MGR-MemoryManager.h for info about flags.
     * @return Pointer to buffer
     * @retval NULL on error
     */
    void *AllocMem(long iSize, long iFlags) { iSize = iFlags = 0; return NULL; };

    /**
     * @brief Memory deallocation.
     *
     * dellocates buffer of memory. In debug builds additional buffer info is freed from 
     * memory and pointer is checked for validity (if it was allocated by manager).
     * @param pPointer Pointer to free
     */
    void FreeMem(void *pPointer) { pPointer = NULL; };

    /**
     * @brief Memory reallocation.
     *
     * reallocates buffer of memory. In debug builds info about file and line of code
     * (where this method was called), reallocated pointer and it's size is stored
     * (in reality previous info is overwritten). Pointer validity checks are performed.
     * @param pOld Old memory pointer
     * @param iSize Size of new buffer size
     * @param iFlags Flags for creation. See MGR-MemoryManager.h for info about flags.
     * @return Pointer to new buffer
     * @retval NULL on error
     */
    void *ReAllocMem(void *pOld, long iSize, long iFlags) { pOld = NULL; iSize = iFlags = 0; return NULL; };

    /**
     * This function is used by debug versions of new / delete to validate objects.
     */
    void SetOwner(char *File, UINT Line, char *Function);
};

extern CMemoryManager *MemMgr;

#include "mmgr.h"