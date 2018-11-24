#pragma once

#include "Global.h"

#include "nommgr.h"
/**
 * @brief A common ancestor class
 * @author Michal Valient
 * @version 1.0
 * 
 * This class is common ancestor for all classes defined in project.
 * It has overrided operators for \c new and \c delete that support memory management
 * and memory leak protection defined in CMemoryManager (in debug versions).
 * 
 * If advanced base class is needed, use CBaseClass. It has some additional parameters
 * like <b>class name</b>, <b>object name</b> or <b>garbage collection</b> and 
 * <b>runtime creation</b> capabilities
 *
 * Use CRootClass as ancestor if your class is frequently used utility class like CMString, 
 * CHashTable2 or CStrHashTable are.
 */
class CRootClass {
public:
    /**
     * @brief Class constructor
     *
     * Currently does nothing
     */
	CRootClass();

    /**
     * @brief Class destructor
     *
     * Currently does nothing
     */
	virtual ~CRootClass();

    /**
     * @brief The release version of \c new operator
     * @param s Size of object - passed in by compiler
     * @return a pointer to buffer where object will resist.
     * @retval NULL means error (and no memory for object)
     *
     * Allocates memory for object.
     * If memory manager is presented, it uses release version of \c new method.
     * Else it uses \c malloc method.
     */
    void *operator new(size_t s);

    /**
     * @brief The release version of \c delete operator
     * @param p pointer to object
     *
     * Dealocates memory used by object.
     * If memory manager is presented and \c D3_DEBUG_MEMORY_MANAGER is defined, 
     * it uses debug version of \c delete method. If no \c D3_DEBUG_MEMORY_MANAGER is 
     * presented and memory manager is online it uses release version of \c delete.
     * If no memory manager is available it uses \c free method.
     */
    void operator delete(void *p);
};

#include "mmgr.h"

