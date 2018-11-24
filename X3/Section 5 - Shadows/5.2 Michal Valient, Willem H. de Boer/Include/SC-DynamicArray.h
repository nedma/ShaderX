//############################################################################
//# DynamicArray - dynamicaly growing array of classes.
//# Stability of indexes is not guaranteed in this array - the only exception
//# is if using only add method. Every Remove method will shift objects after
//# deleted one...
//############################################################################

#pragma once

#include "rootclass.h"

class CPointerArray : public CRootClass {
protected:
    void       **Data;
    int          Size;     // # of objects
    int          MaxSize;  // how big is array really
    int          GrowBy;   // how many elements to add when growing
public:
    D3_INLINE int GetSize() {
        return Size;
    }

    D3_INLINE int GetCapacity() {
        return MaxSize;
    }

    D3_INLINE void *GetDataBuffer() {
        return Data;
    }

    D3_INLINE void *GetAt(int Index) {
        if ((Index>=Size) || (Index<0)) return NULL;
        return Data[Index];
    }

    D3_INLINE void SetAt(int Index, void *It) {
        if (Index<0) return;
        if (Index>=Size) return;
        Data[Index]=It;
    }

    D3_INLINE void DeleteAt(int Index) {
        if (Index<0) return;
        if (Index>Size) return;
        if (Index+1<Size) { //normal case
            Size--;
            Data[Index] = Data[Size];
            Data[Size] = NULL;
        } else {
            Size--;
            Data[Size] = NULL;
        }
    }

    D3_INLINE int Add(void *It) {
        if (Size>=MaxSize) SetSize(Size+GrowBy);
        Data[Size]=It;
        Size++;
        return (Size-1);
    }

    void RemoveAll() {
        SetSize(0);
    }

    void Grow(int GrowBy = -1) {
        this->GrowBy=GrowBy;
        if (GrowBy<=0) GrowBy=1;
        SetSize(MaxSize+GrowBy);
    }

    CPointerArray() {
        Data=NULL;
        Size=0;
        MaxSize=0;
        GrowBy=16;
    }

    ~CPointerArray() {
        if (Data!=NULL)MemMgr->FreeMem(Data);
    }

    void SetSize(int NewSize, int GrowBy = -1) {
        if (GrowBy>0) this->GrowBy=GrowBy;
        if ((NewSize<0) || (NewSize==MaxSize)) return;
        Data = (void **)MemMgr->ReAllocMem(Data,sizeof(void *)*NewSize,0);
        if (NewSize<Size) Size = NewSize;
        if (NewSize>MaxSize) {
            for (int i=MaxSize; i<NewSize; i++) {
                Data[i]=NULL;
            }
        }
        MaxSize=NewSize;
    }
};

//############################################################################
//# RootClass Array....
//############################################################################

class CRootClassArray : public CRootClass {
protected:
    CRootClass **Data;
    int          Size;     // # of objects
    int          MaxSize;  // how big is array really
    int          GrowBy;   // how many elements to add when growing
    bool         OwnsData; // if this owns data...
public:
    D3_INLINE int GetSize() {
        return Size;
    }

    D3_INLINE int GetCapacity() {
        return MaxSize;
    }

    D3_INLINE void *GetDataBuffer() {
        return Data;
    }
    
    D3_INLINE CRootClass *GetAt(int Index) {
        if ((Index>=Size) || (Index<0)) return NULL;
        return Data[Index];
    }

    D3_INLINE void SetAt(int Index, CRootClass *It) {
        if (Index<0) return;
        if (Index>=Size) {  //grow
            SetSize(Index+1);
            Size = Index+1;
        }
        if (Data[Index]!=NULL) {
            DeleteAt(Index);
            Size++;
        }
        Data[Index]=It;
    }

    D3_INLINE void DeleteAt(int Index) {
        if (Index<0) return;
        if (Index>Size) return;
        if (Index+1<Size) { //normal case
            Size--;
            if (OwnsData && Data[Index]!=NULL) {
                delete Data[Index];
            }
            Data[Index] = Data[Size];
            Data[Size] = NULL;
        } else {
            Size--;
            if (OwnsData && Data[Index]!=NULL) {
                delete Data[Index];
            }
            Data[Size] = NULL;
        }
    }

    D3_INLINE CRootClass *EraseAt(int Index) {
        CRootClass *result = NULL;
        if (Index<0) return NULL;
        if (Index>Size) return NULL;
        if (Index+1<Size) { //normal case
            Size--;
            result = Data[Index];
            Data[Index] = Data[Size];
            Data[Size] = NULL;
        } else {
            Size--;
            result = Data[Index];
            Data[Size] = NULL;
        }
        return result;
    }

    D3_INLINE int Add(CRootClass *It) {
        if (Size>=MaxSize) SetSize(Size+GrowBy);
        Data[Size]=It;
        Size++;
        return (Size-1);
    }

    void RemoveAll() {
        SetSize(0);
    }

    void Grow(int GrowBy = -1) {
        this->GrowBy=GrowBy;
        if (GrowBy<=0) GrowBy=1;
        SetSize(MaxSize+GrowBy);
    }

    CRootClassArray(bool OwnsData) {
        Data=NULL;
        Size=0;
        MaxSize=0;
        GrowBy=16;
        this->OwnsData = OwnsData;
    }

    ~CRootClassArray() {
        SetSize(0);
    }

    void SetSize(int NewSize, int GrowBy = -1) {
        if (GrowBy>0) this->GrowBy=GrowBy;
        if ((NewSize<0) || (NewSize==MaxSize)) return;
        if (NewSize<Size) { //decreasing size of array...
            if (OwnsData) {
                for (int i=NewSize-1; i<Size; i++) {
                    if (i>=0) {
                        delete Data[i];
                    }
                }
            }
            Size = NewSize;
        }
        Data = (CRootClass **)MemMgr->ReAllocMem(Data,sizeof(CRootClass *)*NewSize,0);
        if (NewSize>MaxSize) {
            for (int i=MaxSize; i<NewSize; i++) {
                Data[i]=NULL;
            }
        }
        MaxSize=NewSize;
    }
};


//############################################################################
//# Structure Array....
//############################################################################

class CStructureArray : public CRootClass {
protected:
    char        *Data;
    int          Size;     // # of objects
    int          MaxSize;  // how big is array really
    int          GrowBy;   // how many elements to add when growing
    int          SizeOfStructure;
public:
    D3_INLINE int GetSize() {
        return Size;
    }

    D3_INLINE int GetCapacity() {
        return MaxSize;
    }

    D3_INLINE void *GetDataBuffer() {
        return Data;
    }

    D3_INLINE int GetStructureSize() {
        return SizeOfStructure;
    }

    D3_INLINE void SetStructureSize(int SizeOfStructure) {
        if (this->SizeOfStructure==0 && SizeOfStructure>0) this->SizeOfStructure = SizeOfStructure;
    }

    D3_INLINE void *GetAt(int Index) {
        if ((Index>=Size) || (Index<0)) return NULL;
        return &Data[Index*SizeOfStructure];
    }

    D3_INLINE void *CopyFrom(void *Destination, int Index) {
        if ((Index>=Size) || (Index<0)) return NULL;
        memcpy(Destination, &Data[Index*SizeOfStructure], SizeOfStructure);
        return Destination;
    }

    D3_INLINE void *SetAt(int Index, void *Item) {
        if (Index<0) return NULL;
        if (Index>=Size) return NULL;
        memcpy(&Data[Index*SizeOfStructure], Item, SizeOfStructure);
        return &Data[Index*SizeOfStructure];
    }

    D3_INLINE void DeleteAt(int Index) {
        if (Index<0) return;
        if (Index>Size) return;
        if (Index+1<Size) { //normal case
            Size--;
            memcpy(&Data[Index*SizeOfStructure], &Data[Size*SizeOfStructure], SizeOfStructure);
            ZeroMemory(&Data[Size*SizeOfStructure], SizeOfStructure);
        } else {
            Size--;
            ZeroMemory(&Data[Size*SizeOfStructure], SizeOfStructure);
        }
    }

    D3_INLINE int Add(void *Item) {
        if (Size>=MaxSize) SetSize(Size+GrowBy);
        memcpy(&Data[Size*SizeOfStructure], Item, SizeOfStructure);
        Size++;
        return (Size-1);
    }

    void RemoveAll() {
        SetSize(0);
    }

    void Grow(int GrowBy = -1) {
        this->GrowBy=GrowBy;
        if (GrowBy<=0) GrowBy=1;
        SetSize(MaxSize+GrowBy);
    }

    CStructureArray(int SizeOfStructure = 0) {
        Data=NULL;
        Size=0;
        MaxSize=0;
        GrowBy=16;
        this->SizeOfStructure = 0;
        if (SizeOfStructure>0) this->SizeOfStructure = SizeOfStructure;
    }

    ~CStructureArray() {
        if (Data!=NULL)MemMgr->FreeMem(Data);
    }

    void SetSize(int NewSize, int GrowBy = -1) {
        if (GrowBy>0) this->GrowBy=GrowBy;
        if ((NewSize<0) || (NewSize==MaxSize)) return;
        Data = (char *)MemMgr->ReAllocMem(Data,SizeOfStructure*NewSize,0);
        if (NewSize<Size) Size = NewSize;
        if (NewSize>MaxSize) {
            ZeroMemory(&Data[MaxSize*SizeOfStructure], SizeOfStructure*(NewSize-MaxSize));
        }
        MaxSize=NewSize;
    }
};


