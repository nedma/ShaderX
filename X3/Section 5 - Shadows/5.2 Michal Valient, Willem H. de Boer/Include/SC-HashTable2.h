// SC-HashTable2.h: interface for the CHashTable2 class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "RootClass.h"

//############################################################################
//Forward declarations

//Static array of prime numbers used in automatic grow method.
//all are less than 10MB
static UINT Primes10MB[] = {5,11,17,37,67,131,257,521,1031,2053,4099,8209,16411,32771,65537,131101,
                           262147,524309,1048583,1150987,1253377,1355777,1458179,1560589,1662977,
                           1765403,1867783,1970183,2072579,2174981,2699269,3223567,3747847,4272139,
                           4796423,5320709,5844997,6369283,6893573,7417859,7942163,8466433,8990731,
                           9515017,10039297};

///Hashtable implementation. Has O(1) reads / O(1) writes (O(n) on collision).
///Colisions are not allowed. Automatic grow is performed
///Integer keys only
class CHashTable2 : public CRootClass {
private:
    //If bucket is empty, key casted to pointer points to this address of this constant
    static const int EmptyKey = -1;
    //If bucket is empty, value points to this address of this constant
    static const int EmptyValue = -1;

    typedef struct HashPairTag {
        UINT    Key;
        void   *Value;
    } HashPair;

    HashPair        *Pair;
    UINT            PairCapacity;  //Real capacity of table
    UINT            PairCount;     //Count of stored key/value pairs
    bool            OwnsData;

    ///Internal method, that tests if number is a prime number 
    D3_INLINE bool IsPrime(UINT n) const;

    ///Internal method to return a prime number at least as large as n. Assumes n > 0.
    D3_INLINE UINT NextPrime(UINT n) const;

    //This one grows (or shrinks) hash table in case of no free space.
    D3_INLINE void GrowTable(const UINT MinSize, const bool Rehash, const bool AddNew, const UINT Key, const void *Value);

    D3_INLINE void AllocateMemory(const UINT ItemCount, bool Reallocate);
protected:
public:
//-------------------general methods

    ///computes hash for given key
    D3_INLINE UINT GetHash(const UINT key) const;

//-------------------iteration methods (O(n) time)
//general note - do not call any manipulation method (add, delete, erase).
//when iterating table
    ///gets first item in hash table. returns parameter index to GetNextItem 
    ///and GetPrevItem, GetValueAtIndex, GetKeyAtIndex.
    ///returns 0xFFFFFFFF on empty table, so check for item count prior to calling this function
    //check!
    D3_INLINE UINT GetFirstItem() const;

    ///gets next item to index passed. on no more items returns the same value as 
    ///passed in parameter
    //check!
    D3_INLINE UINT GetNextItem(const UINT start) const;

    ///gets prev item to index passed. on no more items returns the same value as 
    ///passed in parameter
    //check!
    D3_INLINE UINT GetPrevItem(const UINT start) const;    

    ///returns number of items in hash table.
    //check!
    D3_INLINE UINT GetItemCount() const;

    ///returns value at index. Valid indexes are only those from iterator methods
    ///No error returned, because only valid indexes assumed!!!
    D3_INLINE void *GetValueAtIndex(const UINT index) const;

    ///returns key at index. Valid indexes are only those from iterator methods
    ///No error returned, because only valid indexes assumed!!!
    D3_INLINE UINT GetKeyAtIndex(const UINT index) const;

//-----------------Get, Set, Add
    ///returns key of specified value (if in table) in 2nd parameter.
    ///returns false if object is not in table.
    //check!
    D3_INLINE bool GetKey(const void *Object, UINT *Key) const;
    
    ///returns object for specified key (if in table).
    ///returns NULL if key is not in table, so if you are not sure, if you passed 
    ///NULL as a value in add, check this by IsKey() method. If it returns true, then
    ///NULL is valid value
    //check!
    D3_INLINE void *Get(const UINT Key) const;

    ///Method returns true if object with given key is in table.
    D3_INLINE bool IsKey(const UINT Key) const;

    ///deletes object from table (and also object from memory).
    ///Does nothing, if key was invalid
	D3_INLINE void Delete(const UINT Key);

    ///removes object from table, but not deletes it (if object is class, it is not deleted from memory)
    ///Does nothing, if key was invalid
    D3_INLINE void Erase(const UINT Key);

    ///adds object to table. if the same key is already in table, it is replaced 
    ///(and if table owns objects, old one is also deleted)
    D3_INLINE void Add(const UINT Key, const void *Object);

	///Clear entire hashtable
    ///If DoShrink is true, then table's memory is cleared too.
    void Clear(const bool DoShrink);

    //OwnsData  - if true, hashtable deletes objects from memory, if they are CRootClass values!!!
    //InitialSize - initial size of table (if exceeded, will grow automaticaly). 
    //              This should be average maximum expected size if you want to 
    //              avoid rehashing (O(n)) of table
    CHashTable2(const bool OwnsData, const UINT InitialSize);
	virtual ~CHashTable2();
};

//////////////////////////////////////////////////////////////////////
// Add / Get
//////////////////////////////////////////////////////////////////////

bool CHashTable2::GetKey(const void *Object, UINT *Key) const {
    for (UINT i=0; i<PairCapacity; i++) {
        if ((Pair[i].Value == Object) && ((int *)Pair[i].Key!=&EmptyKey)) {
            _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"Get Key - Object %p - found",&Object);
            *Key = Pair[i].Key;
            return true;
        }
    }
    _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"Get Key - Object %p - not found",&Object);
    return false;
}

void *CHashTable2::Get(const UINT Key) const {
    UINT hashedKey = GetHash(Key);
    if ((Pair[hashedKey].Key == Key) && (Pair[hashedKey].Value!=(void *)&EmptyValue)) {   //good case
        _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"Get - Key %i (Hash %i) - found",Key, hashedKey);
        return Pair[hashedKey].Value;
    } else {
        _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"Get - Key %i (Hash %i) - not found",Key, hashedKey);
        return NULL;
    }
}

bool CHashTable2::IsKey(const UINT Key) const {
    UINT hashedKey = GetHash(Key);
    if ((Pair[hashedKey].Key == Key) && (Pair[hashedKey].Value!=(void *)&EmptyValue)) {   //good case
        _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"IsKey - Key %i (Hash %i) - found",Key, hashedKey);
        return true;
    } else {
        _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"IsKey - Key %i (Hash %i) - not found",Key, hashedKey);
        return false;
    }
}

void CHashTable2::Add(const UINT Key, const void *Object) {
    UINT hashedKey = GetHash(Key);
    if (Pair[hashedKey].Value==(void *)&EmptyValue) {   // safe - empty slot
        _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"Adding Key %i (Hash %i) - Safe",Key, hashedKey);
        Pair[hashedKey].Key = Key;
        Pair[hashedKey].Value = (void *)Object;
        PairCount++;
    } else if (Pair[hashedKey].Key == Key) {    //correct overwrite case
        int logB = _LOGB("HashTable", NULL, D3_DV_GRP3_LEV4,"Adding Key %i (Hash %i) - Overwrite",Key, hashedKey);
        if (OwnsData) delete (CRootClass *)Pair[hashedKey].Value;
        Pair[hashedKey].Key = Key;
        Pair[hashedKey].Value = (void *)Object;
        _LOGE(logB,"Add Overwrite - End");
    } else {    //collision - different keys and identical hash!!!
        int logB = _LOGB("HashTable", NULL, D3_DV_GRP3_LEV4,"Add Collision! - Key %i collided with Key %i (Hash %i)",Key, Pair[hashedKey].Key, hashedKey);
        GrowTable(PairCapacity, true, true, Key, Object);
        _LOGE(logB,"Add Collision - End");
    }
}

//////////////////////////////////////////////////////////////////////
// Erase / Delete
//////////////////////////////////////////////////////////////////////

void CHashTable2::Erase(const UINT Key) {
    UINT hashedKey = GetHash(Key);
    if ((Pair[hashedKey].Key == Key) && (Pair[hashedKey].Value!=(void *)&EmptyValue)) {   //good case
        _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"Erase - Key %i (Hash %i) - erased",Key, hashedKey);
        Pair[hashedKey].Key = (int)&EmptyKey;
        Pair[hashedKey].Value = (void *)&EmptyValue;
        PairCount--;
    } else {
        _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"Erase - Key %i (Hash %i) - not found",Key, hashedKey);
    }
}

void CHashTable2::Delete(const UINT Key) {
    UINT hashedKey = GetHash(Key);
    if ((Pair[hashedKey].Key == Key) && (Pair[hashedKey].Value!=(void *)&EmptyValue)) {   //good case
        int logE = _LOGB("HashTable", NULL,D3_DV_GRP3_LEV4,"Delete - Key %i (Hash %i) - ",Key, hashedKey);
        if (OwnsData) delete (CRootClass *)Pair[hashedKey].Value;
        Pair[hashedKey].Key = (int)&EmptyKey;
        Pair[hashedKey].Value = (void *)&EmptyValue;
        PairCount--;
        _LOGE(logE,"Delete key - End");
    } else {
        _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"Delete - Key %i (Hash %i) - not found",Key, hashedKey);
    }
}

//////////////////////////////////////////////////////////////////////
// Sequential iterators
//////////////////////////////////////////////////////////////////////
UINT CHashTable2::GetFirstItem() const {
    if (PairCount<=0) {
        _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"GetFirstItem - no items");
        return 0xFFFFFFFF;
    } else {
        for (UINT i=0; i<PairCapacity; i++) {
            if ((Pair[i].Value!=(void *)&EmptyValue) && ((int *)Pair[i].Key!=&EmptyKey)) {    //is filled
                _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"GetFirstItem - Key %i Object %p Callback %i", Pair[i].Key, Pair[i].Value, i);
                return i;
            }
        }
        _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"GetFirstItem - no items");
        return 0xFFFFFFFF;
    }
}

UINT CHashTable2::GetNextItem(const UINT start) const {
    if ((UINT)start >= PairCapacity) {
        _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"GetNextItem - no items");
        return start;
    } else {
        for (UINT i=start+1; i<PairCapacity; i++) {
            if ((Pair[i].Value!=(void *)&EmptyValue) && ((int *)Pair[i].Key!=&EmptyKey)) {    //is filled
                _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"GetNextItem - Next to %i -> Key %i Object %p Callback %i", start, Pair[i].Key, Pair[i].Value, i);
                return i;
            }
        }
        _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"GetNextItem - no items");
        return start;
    }
}

UINT CHashTable2::GetPrevItem(const UINT start) const {
    if (start <= 0) {
        _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"GetNextItem - no items");
        return start;
    } else {
        for (UINT i=start-1; i>=0; i--) {
            if ((Pair[i].Value!=(void *)&EmptyValue) && ((int *)Pair[i].Key!=&EmptyKey)) {    //is filled
                _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"GetPrevItem - Next to %i -> Key %i Object %p Callback %i", start, Pair[i].Key, Pair[i].Value, i);
                return i;
            }
        }
        _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"GetNextItem - no items");
        return start;
    }
}

UINT CHashTable2::GetItemCount() const {
    _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"GetItemCount - %i items",PairCount);
    return PairCount;
}

void *CHashTable2::GetValueAtIndex(const UINT index) const {
    return Pair[index].Value;
}

UINT CHashTable2::GetKeyAtIndex(const UINT index) const {
    return Pair[index].Key;
}

//computes hash for given key
UINT CHashTable2::GetHash(const UINT key) const {
    UINT tmpKey = key;
    tmpKey += (tmpKey << 12);
    tmpKey ^= (tmpKey >> 22);
    tmpKey += (tmpKey << 4);
    tmpKey ^= (tmpKey >> 9);
    tmpKey += (tmpKey << 10);
    tmpKey ^= (tmpKey >> 2);
    tmpKey += (tmpKey << 7);
    tmpKey ^= (tmpKey >> 12);
    return tmpKey % PairCapacity;
}


//Internal method, that tests if number is a prime number 
bool CHashTable2::IsPrime(UINT n) const {
    if( n == 2 || n == 3 ) return true;
    if( n == 1 || n % 2 == 0 ) return false;
    for( UINT i = 3; i * i <= n; i += 2 ) if( n % i == 0 ) return false;
    return true;
}

//Internal method to return a prime number at least as large as n. Assumes n > 0.
UINT CHashTable2::NextPrime(UINT n) const {
    if( n % 2 == 0 ) n++;
    while (!IsPrime(n)) n=n+2;
    return n;
}
