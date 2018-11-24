#pragma once
#include "sc-MString.h"

///Hashtable implementation. Has O(1) reads / O(1) writes (O(n) on collision).
///Colisions are not allowed. Automatic grow is performed
///CString keys only
class CStrHashTable : public CRootClass {
private:
    typedef struct HashPairTag {
        char     *Key;
        void     *Value;
    } HashPair;

    HashPair        *Pair;
    UINT            PairCapacity;  //Real capacity of table
    UINT            PairCount;     //Count of stored key/value pairs
    bool            OwnsData;
    UINT            TableBitSize; //How many times do we have to shift left to get table size

    //This one grows (or shrinks) hash table in case of no free space.
    D3_INLINE void GrowTable(const UINT MinSize, const bool Rehash, const bool AddNew, const char *Key, const UINT Length, const void *Value);

    D3_INLINE void AllocateMemory(const UINT ItemCount, bool Reallocate);
public:
//-------------------general methods
    ///sets initial size of the table (works only on empty tables!!!)
    D3_INLINE void SetInitialSize(UINT MinSize);
    ///computes hash key in table for given key (modeulates buffer hash with table size mask)
    D3_INLINE UINT GetHash(const char *Key, const UINT Length) const;
    ///computes hash for given string
    D3_INLINE UINT GetBufferHash(const char *Key, const UINT Length) const;

//-------------------iteration methods (O(n) time)
//general note - do not call any manipulation method (add, delete, erase).
//when iterating table
    ///gets first item in hash table. returns parameter index to GetNextItem 
    ///and GetPrevItem, GetValueAtIndex, GetKeyAtIndex.
    ///returns 0xFFFFFFFF on empty table, so check for item count prior to calling this function
    D3_INLINE UINT GetFirstItem() const;

    ///gets next item to index passed. on no more items returns the same value as 
    ///passed in parameter
    D3_INLINE UINT GetNextItem(const UINT start) const;

    ///gets prev item to index passed. on no more items returns the same value as 
    ///passed in parameter
    D3_INLINE UINT GetPrevItem(const UINT start) const;    

    ///returns number of items in hash table.
    D3_INLINE UINT GetItemCount() const;

    ///returns value at index. Valid indexes are only those from iterator methods
    ///No error returned, because only valid indexes assumed!!!
    D3_INLINE void *GetValueAtIndex(const UINT index) const;

    ///returns key at index. Valid indexes are only those from iterator methods
    ///No error returned, because only valid indexes assumed!!!
    D3_INLINE void GetKeyAtIndex(const UINT index, CMString &Key) const;

//-----------------Get, Set, Add
    ///returns key of specified value (if in table) in 2nd parameter.
    ///returns false if object is not in table.
    D3_INLINE bool GetKey(const void *Object, CMString &Key) const;
    
    ///returns object for specified key (if in table).
    ///returns NULL if key is not in table, so if you are not sure, if you passed 
    ///NULL as a value in add, check this by IsKey() method. If it returns true, then
    ///NULL is valid value
    D3_INLINE void *Get(const char *Key, const UINT Length) const;
    D3_INLINE void *Get(const char *Key) const {
        return Get(Key, strlen(Key));
    }
    D3_INLINE void *Get(const CMString &Key) const {
        return Get(Key.c_str(), Key.Length());
    }

    ///Method returns true if object with given key is in table.
    D3_INLINE bool IsKey(const char *Key, const UINT Length) const;
    D3_INLINE bool IsKey(const char *Key) const {
        return IsKey(Key, strlen(Key));
    }
    D3_INLINE bool IsKey(const CMString &Key) const {
        return IsKey(Key.c_str(), Key.Length());
    }

    ///deletes object from table (and also object from memory).
    ///Does nothing, if key was invalid
	D3_INLINE void Delete(const char *Key, const UINT Length);
    D3_INLINE void Delete(const char *Key) {
        Delete(Key, strlen(Key));
    }
    D3_INLINE void Delete(const CMString &Key) {
        Delete(Key.c_str(), Key.Length());
    }

    ///removes object from table, but not deletes it (if object is class, it is not deleted from memory)
    ///Does nothing, if key was invalid
    D3_INLINE void Erase(const char *Key, const UINT Length);
    D3_INLINE void Erase(const char *Key) {
        Erase(Key, strlen(Key));
    }
    D3_INLINE void Erase(const CMString &Key) {
        Erase(Key.c_str(), Key.Length());
    }

    ///adds object to table. if the same key is already in table, it is replaced 
    ///(and if table owns objects, old one is also deleted)
    D3_INLINE void Add(const char *Key, const UINT Length, const void *Object);
    D3_INLINE void Add(const char *Key, const void *Object) {
        Add(Key, strlen(Key), Object);
    }
    D3_INLINE void Add(const CMString &Key, const void *Object) {
        Add(Key.c_str(), Key.Length(), Object);
    }

	///Clear entire hashtable
    ///If DoShrink is true, then table's memory is cleared too.
    void Clear(const bool DoShrink);

    //OwnsData  - if true, hashtable deletes objects from memory, if they are CRootClass values!!!
    //InitialSize - initial size of table (if exceeded, will grow automaticaly). 
    //              This should be average maximum expected size if you want to 
    //              avoid rehashing (O(n)) of table
    CStrHashTable(const bool OwnsData, const UINT InitialSize);
	virtual ~CStrHashTable();
};

//////////////////////////////////////////////////////////////////////
// General
//////////////////////////////////////////////////////////////////////
#define hashsize(n) ((UINT)1<<(n))
#define hashmask(n) (hashsize(n)-1)

#define mix(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

UINT CStrHashTable::GetBufferHash(const char *Key, const UINT Length) const {
    /* Set up the internal state */
    UINT len = Length;
    char *k = (char *)Key;
    UINT a = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
    UINT b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
    UINT c = 0;           /* the previous hash value */

    /*---------------------------------------- handle most of the key */
    while (len >= 12) {
        a += (k[0] +((UINT)k[1]<<8) +((UINT)k[2]<<16) +((UINT)k[3]<<24));
        b += (k[4] +((UINT)k[5]<<8) +((UINT)k[6]<<16) +((UINT)k[7]<<24));
        c += (k[8] +((UINT)k[9]<<8) +((UINT)k[10]<<16)+((UINT)k[11]<<24));
        mix(a,b,c);
        k += 12; len -= 12;
    }

    /*------------------------------------- handle the last 11 bytes */
    c += Length;
    switch(len) {            /* all the case statements fall through */
        case 11: c+=((UINT)k[10]<<24);
        case 10: c+=((UINT)k[9]<<16);
        case 8 : c+=((UINT)k[8]<<8);
            /* the first byte of c is reserved for the length */
        case 9 : b+=((UINT)k[7]<<24);
        case 7 : b+=((UINT)k[6]<<16);
        case 6 : b+=((UINT)k[5]<<8);
        case 5 : b+=k[4];
        case 4 : a+=((UINT)k[3]<<24);
        case 3 : a+=((UINT)k[2]<<16);
        case 2 : a+=((UINT)k[1]<<8);
        case 1 : a+=k[0];
        /* case 0: nothing left to add */
    }
    mix(a,b,c);
    /*-------------------------------------------- report the result */
    return c;
}

UINT CStrHashTable::GetHash(const char *Key, const UINT Length) const {
    return (GetBufferHash(Key, Length) & hashmask(TableBitSize));
}

void CStrHashTable::SetInitialSize(UINT MinSize) {
    GrowTable(MinSize, false, false, NULL, 0, NULL);
}

//////////////////////////////////////////////////////////////////////
// Add / Get
//////////////////////////////////////////////////////////////////////
bool CStrHashTable::GetKey(const void *Object, CMString &Key) const {
    for (UINT i=0; i<PairCapacity; i++) {
        if ((Pair[i].Key!=NULL) && (Pair[i].Value == Object)) {
            _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"Get Key - Object %p - found",&Object);
            Key = Pair[i].Key;
            return true;
        }
    }
    _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"Get Key - Object %p - not found",&Object);
    return false;
}

void *CStrHashTable::Get(const char *Key, const UINT Length) const {
    UINT hashedKey = GetHash(Key, Length);
    if ((Pair[hashedKey].Key!=NULL) && (strcmp(Pair[hashedKey].Key, Key) == 0)) {   //good case
        _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"Get - Key %s (Hash %u) - found",Key, hashedKey);
        return Pair[hashedKey].Value;
    } else {
        _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"Get - Key %s (Hash %u) - not found",Key, hashedKey);
        return NULL;
    }
}

bool CStrHashTable::IsKey(const char *Key, const UINT Length) const {
    UINT hashedKey = GetHash(Key, Length);
    if ((Pair[hashedKey].Key!=NULL) && (strcmp(Pair[hashedKey].Key, Key) == 0)) {   //good case
        _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"IsKey -  Key %s (Hash %u) - found",Key, hashedKey);
        return true;
    } else {
        _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"IsKey -  Key %s (Hash %u) - not found",Key, hashedKey);
        return false;
    }
}

void CStrHashTable::Add(const char *Key, const UINT Length, const void *Object) {
    UINT hashedKey = GetHash(Key, Length);
    if (Pair[hashedKey].Key==NULL) {   // safe - empty slot
        _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"Adding  Key %s (Hash %u) - Safe",Key, hashedKey);
        Pair[hashedKey].Key   = (char *)MemMgr->AllocMem(Length+1,0);
        memcpy(Pair[hashedKey].Key, Key, Length);
        Pair[hashedKey].Key[Length] = 0;
        Pair[hashedKey].Value = (void *)Object;
        PairCount++;
    } else if (strcmp(Pair[hashedKey].Key, Key) == 0) {    //correct overwrite case
        int logB = _LOGB("StrHashTable", NULL, D3_DV_GRP3_LEV4,"Adding  Key %s (Hash %u) - Overwrite",Key, hashedKey);
        if (OwnsData) delete (CRootClass *)Pair[hashedKey].Value;
        Pair[hashedKey].Value = (void *)Object;
        _LOGE(logB,"Add Overwrite - End");
    } else {    //collision - different keys and identical hash!!!
        int logB = _LOGB("StrHashTable", NULL, D3_DV_GRP3_LEV4,"Add Collision! - Key %s collided with  Key %s (Hash %u)",Key, Pair[hashedKey].Key, hashedKey);
        GrowTable(PairCapacity, true, true, Key, Length, Object);
        _LOGE(logB,"Add Collision - End");
    }
}

//////////////////////////////////////////////////////////////////////
// Erase / Delete
//////////////////////////////////////////////////////////////////////

void CStrHashTable::Erase(const char *Key, const UINT Length) {
    UINT hashedKey = GetHash(Key, Length);
    if ((Pair[hashedKey].Key != NULL) && (strcmp(Pair[hashedKey].Key, Key) == 0)) {
        _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"Erase -  Key %s (Hash %u) - erased",Key, hashedKey);
        MemMgr->FreeMem(Pair[hashedKey].Key);
        Pair[hashedKey].Key = NULL;
        Pair[hashedKey].Value = NULL;
        PairCount--;
    } else {
        _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"Erase -  Key %s (Hash %u) - not found",Key, hashedKey);
    }
}

void CStrHashTable::Delete(const char *Key, const UINT Length) {
    UINT hashedKey = GetHash(Key, Length);
    if ((Pair[hashedKey].Key != NULL) && (strcmp(Pair[hashedKey].Key, Key) == 0)) {
        int logE = _LOGB("StrHashTable", NULL,D3_DV_GRP3_LEV4,"Delete -  Key %s (Hash %u) - ",Key, hashedKey);
        if (OwnsData) delete (CRootClass *)Pair[hashedKey].Value;
        MemMgr->FreeMem(Pair[hashedKey].Key);
        Pair[hashedKey].Key = NULL;
        Pair[hashedKey].Value = NULL;
        PairCount--;
        _LOGE(logE,"Delete key - End");
    } else {
        _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"Delete -  Key %s (Hash %u) - not found",Key, hashedKey);
    }
}

//////////////////////////////////////////////////////////////////////
// Sequential iterators
//////////////////////////////////////////////////////////////////////
UINT CStrHashTable::GetFirstItem() const {
    if (PairCount<=0) {
        _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"GetFirstItem - no items");
        return 0xFFFFFFFF;
    } else {
        for (UINT i=0; i<PairCapacity; i++) {
            if (Pair[i].Key!=NULL) {    //is filled
                _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"GetFirstItem - Key %s Object %p Callback %i", Pair[i].Key, Pair[i].Value, i);
                return i;
            }
        }
        _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"GetFirstItem - no items");
        return 0xFFFFFFFF;
    }
}

UINT CStrHashTable::GetNextItem(const UINT start) const {
    if ((UINT)start >= PairCapacity) {
        _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"GetNextItem - no items");
        return start;
    } else {
        for (UINT i=start+1; i<PairCapacity; i++) {
            if (Pair[i].Key!=NULL) {    //is filled
                _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"GetNextItem - Next to %i -> Key %s Object %p Callback %i", start, Pair[i].Key, Pair[i].Value, i);
                return i;
            }
        }
        _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"GetNextItem - no items");
        return start;
    }
}

UINT CStrHashTable::GetPrevItem(const UINT start) const {
    if (start <= 0) {
        _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"GetNextItem - no items");
        return start;
    } else {
        for (UINT i=start-1; i>=0; i--) {
            if (Pair[i].Key!=NULL) {    //is filled
                _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"GetPrevItem - Next to %i -> Key %s Object %p Callback %i", start, Pair[i].Key, Pair[i].Value, i);
                return i;
            }
        }
        _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"GetNextItem - no items");
        return start;
    }
}

UINT CStrHashTable::GetItemCount() const {
    _LOG("StrHashTable", NULL, D3_DV_GRP3_LEV4,"GetItemCount - %i items",PairCount);
    return PairCount;
}

void *CStrHashTable::GetValueAtIndex(const UINT index) const {
    return Pair[index].Value;
}

void CStrHashTable::GetKeyAtIndex(const UINT index, CMString &Key) const {
    Key = Pair[index].Key;
}

