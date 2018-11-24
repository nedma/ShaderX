// SC-HashTable.h: interface for the CHashTable class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "RootClass.h"

//############################################################################
//Forward declarations


typedef struct sHashItemTag {
public:
    int         CallbackValue;
    int         Value;
    int         Key;
} sHashItem;

//Static array of prime numbers used in automatic grow method.
//all are less than 10MB
static int Primes10MB[] = {5,11,17,37,67,131,257,521,1031,2053,4099,8209,16411,32771,65537,131101,
                           262147,524309,1048583,1150987,1253377,1355777,1458179,1560589,1662977,
                           1765403,1867783,1970183,2072579,2174981,2699269,3223567,3747847,4272139,
                           4796423,5320709,5844997,6369283,6893573,7417859,7942163,8466433,8990731,
                           9515017,10039297};

//Hashtable implementation. Has O(1) reads / O(1) writes (O(n) on collision).
//Colisions are not allowed. Automatic grow is performed
//Integer keys only
class CHashTable : public CRootClass {
private:
    static const int HPAIR_FILLED = 1;
    static const int HPAIR_CLASS = 2;

    //flags for pairs
    typedef struct HashPairTag {
        int Flags;
        int Key;
        int Value;
    } HashPair;

    HashPair        *Pair;
    UINT             PairCapacity;  //Real capacity of table
    UINT             PairCount;     //Count of stored key/value pairs
    //CRootClassArray *Values;
    bool    OwnsData;

    //Internal method, that tests if number is a prime number 
    bool IsPrime( int n ) {
        if( n == 2 || n == 3 ) return true;
        if( n == 1 || n % 2 == 0 ) return false;
        for( int i = 3; i * i <= n; i += 2 ) if( n % i == 0 ) return false;
        return true;
    }

    //Internal method to return a prime number at least as large as n. Assumes n > 0.
    int NextPrime( int n ) {
        if( n % 2 == 0 ) n++;
        while (!IsPrime(n)) n=n+2;
        return n;
    }

protected:
public:
    //Linear iteration
    //gets first item in hash table. on empty table returns -1.
    D3_INLINE GetFirstItem(sHashItem &result);
    //gets next item to item passed. on no more items returns -1
    D3_INLINE GetNextItem(sHashItem &result);
    //returns number of items in hash table
    D3_INLINE GetItemCount();

    //computes hash for given key
    D3_INLINE int GetHash(int key) {
        key += (key << 12);
        key ^= (key >> 22);
        key += (key << 4);
        key ^= (key >> 9);
        key += (key << 10);
        key ^= (key >> 2);
        key += (key << 7);
        key ^= (key >> 12);
        return key % PairCapacity;
    }

    //returns key of specified object / pointer / int. -1 if not found.
    D3_INLINE int GetKey(CRootClass &Object) { return GetKey((int)&Object); }
    D3_INLINE int GetKey(void *Object) { return GetKey((int)Object); }
    D3_INLINE int GetKey(int Object);
    
    //returns object / pointer / int for specified key, null returned if not found
    //WARNING - returning 0 (or NULL) means error, so be aware, that if you put in 0 as value, you
    //will not be able to distinguish error from normal value 0!!!!
    D3_INLINE void *GetPointer(int Key) { return (void *)Get(Key); }
    D3_INLINE CRootClass *GetClass(int Key) { return (CRootClass *)Get(Key); }
    D3_INLINE int Get(int Key);

    //deletes object / pointer / int from table (and also object from memory)
	D3_INLINE void Delete(int Key);

    //erases object / pointer / int from table (if object is class, it is not deleted from memory)
    //WARNING - returning 0 (or NULL) means error, so be aware, that if you put in 0 as value, you
    //will not be able to distinguish error from normal value 0!!!!
    D3_INLINE void* ErasePointer(int Key) { return (void *)Erase(Key); }
    D3_INLINE CRootClass* EraseClass(int Key) { return (CRootClass *)Erase(Key); }
	D3_INLINE int Erase(int Key);

    //adds int / pointer / class to table. if the same key is already in table, it is replaced 
    //(and if table owns objects, old one is also deleted if it is class)
	D3_INLINE void Add(int Key, int Object, bool IsClass = false);
    D3_INLINE void Add(int Key, void *Object) { Add(Key,(int)Object); }
    D3_INLINE void Add(int Key, CRootClass &Object) { Add(Key,(int)&Object, true); } 

	//Clear entire hashtable
    void Clear();

    //This one grows (or shrinks) hash table in case of no free space.
    void GrowTable(int MinSize);

    //OwnsData  - if true, hashtable deletes objects from memory, if they are CRootClass values!!!
    //InitialSize - initial size of table (if exceeded, will grow automaticaly). 
    //              This should be average maximum expected size if you want to 
    //              avoid rehashing (O(n)) of table
    CHashTable(bool OwnsData, int InitialSize = 0);
	virtual ~CHashTable();
};

int CHashTable::GetKey(int Object) {
    for (UINT i=0; i<PairCapacity; i++) {
        if (((Pair[i].Flags & HPAIR_FILLED)!=0) && (Pair[i].Value == Object)) {
            //_LOG("HashTable", NULL, D3_DVLEVEL6,"Get Key - Object %p - found",&Object);
            return Pair[i].Key;
        }
    }
    //_LOG("HashTable", NULL, D3_DVLEVEL6,"Get Key - Object %p - not found",&Object);
    return -1;
}

//////////////////////////////////////////////////////////////////////
// Add / Get
//////////////////////////////////////////////////////////////////////

int CHashTable::Get(int Key) {
    int hashedKey = GetHash(Key);
    if ((Pair[hashedKey].Key == Key) && ((Pair[hashedKey].Flags & HPAIR_FILLED) != 0)) {   //good case
        //_LOG("HashTable", NULL, D3_DVLEVEL6,"Get - Key %i (Hash %i) - found",Key, hashedKey);
        return Pair[hashedKey].Value;
    } else {
        //_LOG("HashTable", NULL, D3_DVLEVEL6,"Get - Key %i (Hash %i) - not found",Key, hashedKey);
        return 0;
    }
}

void CHashTable::Add(int Key, int Object, bool IsClass) {
    int hashedKey = GetHash(Key);
    if ((Pair[hashedKey].Flags & HPAIR_FILLED) == 0) {   // safe - empty slot
        //_LOG("HashTable", NULL, D3_DVLEVEL6,"Adding Key %i (Hash %i) - Safe",Key, hashedKey);
        if (IsClass) Pair[hashedKey].Flags = HPAIR_FILLED | HPAIR_CLASS;
        else Pair[hashedKey].Flags = HPAIR_FILLED;
        Pair[hashedKey].Key = Key;
        Pair[hashedKey].Value = Object;
        PairCount++;
    } else if (Pair[hashedKey].Key == Key) {    //correct overwrite case
        //int logB = _LOGB("HashTable", NULL, D3_DVLEVEL6,"Adding Key %i (Hash %i) - Overwrite",Key, hashedKey);
        if (((Pair[hashedKey].Flags & HPAIR_CLASS) !=0) && (OwnsData)) delete (CRootClass *)Pair[hashedKey].Value;
        if (IsClass) Pair[hashedKey].Flags = HPAIR_FILLED | HPAIR_CLASS;
        else Pair[hashedKey].Flags = HPAIR_FILLED;
        Pair[hashedKey].Key = Key;
        Pair[hashedKey].Value = Object;
        //_LOGE(logB,"Add Overwrite - End");
    } else {    //collision - different keys and identical hash!!!
        //int logB = _LOGB("HashTable", NULL, D3_DVLEVEL6,"Add Collision! - Key %i collided with Key %i (Hash %i)",Key, Pair[hashedKey].Key, hashedKey);
        GrowTable(PairCapacity);
        Add(Key,Object,IsClass);
        //_LOGE(logB,"Add Collision - End");
    }
}

//////////////////////////////////////////////////////////////////////
// Erase / Delete
//////////////////////////////////////////////////////////////////////

int CHashTable::Erase(int Key) {
    int hashedKey = GetHash(Key);
    if ((Pair[hashedKey].Key == Key) && ((Pair[hashedKey].Flags & HPAIR_FILLED) != 0)) {   //good case
        //_LOG("HashTable", NULL, D3_DVLEVEL6,"Erase - Key %i (Hash %i) - erased",Key, hashedKey);
        int retv = Pair[hashedKey].Value;
        Pair[hashedKey].Flags = 0;
        Pair[hashedKey].Key = 0;
        Pair[hashedKey].Value = NULL;
        PairCount--;
        return retv;
    } else {
        //_LOG("HashTable", NULL, D3_DVLEVEL6,"Erase - Key %i (Hash %i) - not found",Key, hashedKey);
        return 0;
    }
}

void CHashTable::Delete(int Key) {
    int hashedKey = GetHash(Key);
    if ((Pair[hashedKey].Key == Key) && ((Pair[hashedKey].Flags & HPAIR_FILLED) != 0)) {   //good case
        //int logE = _LOGB("HashTable", NULL,D3_DVLEVEL6,"Delete - Key %i (Hash %i) - ",Key, hashedKey);
        if ((Pair[hashedKey].Flags & HPAIR_CLASS) != 0) delete (CRootClass *)Pair[hashedKey].Value;
        Pair[hashedKey].Flags = 0;
        Pair[hashedKey].Key = 0;
        Pair[hashedKey].Value = NULL;
        PairCount--;
        //_LOGE(logE,"Delete key - End");
    } else {
        //_LOG("HashTable", NULL, D3_DVLEVEL6,"Delete - Key %i (Hash %i) - not found",Key, hashedKey);
    }
}

//////////////////////////////////////////////////////////////////////
// Sequential iterators
//////////////////////////////////////////////////////////////////////
int CHashTable::GetFirstItem(sHashItem &result) {
    if (PairCount<=0) {
        //_LOG("HashTable", NULL, D3_DVLEVEL6,"GetFirstItem - no items");
        return -1;
    } else {
        for (UINT i=0; i<PairCapacity; i++) {
            if ((Pair[i].Flags & HPAIR_FILLED)!=0) {    //is filled
                result.CallbackValue = i;
                result.Key = Pair[i].Key;
                result.Value = Pair[i].Value;
                //_LOG("HashTable", NULL, D3_DVLEVEL6,"GetFirstItem - Key %i Object %p Callback %i", result.Key, result.Value, result.CallbackValue);
                return 1;
            }
        }
        //_LOG("HashTable", NULL, D3_DVLEVEL6,"GetFirstItem - no items");
        return -1;
    }
}


int CHashTable::GetNextItem(sHashItem &result) {
    if ((UINT)result.CallbackValue+1 >= PairCapacity) {
        //_LOG("HashTable", NULL, D3_DVLEVEL6,"GetNextItem - no items");
        return -1;
    } else {
        //int oldCallback = result.CallbackValue;
        for (UINT i=result.CallbackValue+1; i<PairCapacity; i++) {
            if ((Pair[i].Flags & HPAIR_FILLED)!=0) {    //is filled
                result.CallbackValue = i;
                result.Key = Pair[i].Key;
                result.Value = Pair[i].Value;
                //_LOG("HashTable", NULL, D3_DVLEVEL6,"GetNextItem - Next to %i -> Key %i Object %p Callback %i",oldCallback, result.Key, result.Value, result.CallbackValue);
                return 1;
            }
        }
        //_LOG("HashTable", NULL, D3_DVLEVEL6,"GetNextItem - no items");
        return -1;
    }
}

int CHashTable::GetItemCount() {
    //_LOG("HashTable", NULL, D3_DVLEVEL6,"GetItemCount - %i items",PairCount);
    return PairCount;
}

