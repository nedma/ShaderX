#pragma once

#include "RootClass.h"

//############################################################################
//Forward declarations
class CEngine;

/**
 * @brief A common ancestor for all named classes.
 * @author Michal Valient
 * @version 1.0
 * 
 * This class is common ancestor for all named classes defined in project.
 * It has some additional parameters like <b>class name</b>, <b>object name</b> or
 * <b>garbage collection</b> and <b>runtime creation</b> capabilities.
 * Every CBaseClass (or it's descendants) instance must be owned by some CEngine class.
 * Therefore every constructor has CEngine parameter.
 *
 * Use CRootClass as ancestor if your class is not an utility class like
 * CMesh or CFileManager.
 */
class CBaseClass : public CRootClass {
    protected:
        ///Name of the object
        char *      ObjectName;
    public:
        ///Engine, that own's of this class
        CEngine *   OwnerEngine;    //Owner engine of object

        /**
        * @brief Class constructor
        * Sets OwnerEngine and ObjectName atributes
        *
        * @param ObjName Name of object
        * @param OwnerEngine Engine, that own's this object
        */
        CBaseClass(char *ObjName,CEngine *OwnerEngine);

        /**
        * @brief Class destructor
        *
        * Frees ObjectName buffer
        */
        virtual ~CBaseClass();

        /**
         * @brief Runtime creation method.
         * Method used by CRuntimeCreator to create instance of class by name of class
         *
         * @param ObjName Name of object
         * @param OwnerEngine Engine, that own's this object
         * @param RegisterParameters parameters passed to register method in RuntimeCreator. Optional
         * @param Parameters Initial parameters created object.
         * @return Pointer to created object
         * @retval NULL on error
         */
        virtual CBaseClass *Create(char *ObjName,CEngine *OwnerEngine, void *RegisterParameters, void *Parameters) = 0;

        /**
         * @brief Causes freeing of all (currently) unused memory
         * after call of this method object _should_ compact its size to
         * minimum freeing all unused memory.
         */
        virtual void GarbageCollection() = 0;   //object size compactor

        /**
         * @brief This method returns class name.
         * This method fills desired buffer with class name.
         * By default MAKE_CLSNAME macro is used to declare this method.
         *
         * @param Buffer - pointer to buffer that will be filled with class name
         * @param Size - size of buffer
         * @retval Returns zero on no error
         * @retval Returns required size, if buffer is not big enough
         */
        virtual int GetClsName(char *Buffer,UINT Size) = 0;

        /**
            @brief Returns pointer to object name.
            @warning Use returned pointer as read-only
            @warning Memory pointed by pointer is undefined if object is deleted.
            @return Pointer to object name or NULL if no name was specified
        */
        D3_INLINE char *GetObjName() {
            return ObjectName;
        }

        /**
         * @brief This method returns object name.
         * This method fills desired buffer with object name.
         *
         * @param Buffer - pointer to buffer that will be filled with name
         * @param Size - size of buffer
         * @retval Returns zero on no error
         * @retval Returns required size, if buffer is not big enough
         */
        D3_INLINE int GetObjName(char *Buffer,UINT Size) {
            if (ObjectName==NULL) {
                if (Size<1) return 1;
                else {
                    Buffer[0]=0;
                    return 0;
                }
            } else {
                if (Size<strlen(ObjectName)+1) return strlen(ObjectName)+1;
                else {
                    strcpy(Buffer,ObjectName);
                    return 0;
                }
            }
        }

        /**
         * @brief This method sets object name.
         *
         * @param Buffer - pointer to null terminated string with object name
         */
        D3_INLINE void SetObjName(char *Buffer) {
            if (Buffer==NULL) {
                if (ObjectName!=NULL) free(ObjectName);
                ObjectName=NULL;
            } else {
                ObjectName = (char *)realloc(ObjectName,strlen(Buffer)+1);
                strcpy(ObjectName,Buffer);
            }
        }
};

///This macro creates default garbage collection method, that does nothing.
#define MAKE_DEFAULTGARBAGE() \
    virtual void GarbageCollection() {\
    }

///This macro creates method, that returns class name string.
///@param X is string with class name.
#define MAKE_CLSNAME(X) virtual int GetClsName(char *sBuffer,UINT iSize) { \
            if (iSize<strlen(X)+1) return strlen(X)+1; \
                else { \
                strcpy(sBuffer,X); \
                return 0; \
            } \
}

///This macro creates default method, that is used by CRuntimeCreator for dynamic creation of objects.
///@param X is class name identifier.
#define MAKE_CLSCREATE(X) virtual CBaseClass *Create(char *ObjName,CEngine *OwnerEngine, void *RegisterParameters, void *Parameters) { \
    return new X(ObjName,OwnerEngine); \
}

