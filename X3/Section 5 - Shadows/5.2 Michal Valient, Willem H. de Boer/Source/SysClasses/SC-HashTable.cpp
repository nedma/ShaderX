// SC-HashTable.cpp: implementation of the CHashTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SC-HashTable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHashTable::CHashTable(bool OwnsData, int InitialSize) {
    //int logE = _LOGB("HashTable", NULL ,D3_DVLEVEL6,"Hashtable initialization (InitialSize is %i)",InitialSize);
    //find good initial size
    if (InitialSize<=Primes10MB[sizeof(Primes10MB)/4 -1]) {
        for (int i=0; i<sizeof(Primes10MB)/4; i++) {
            if (Primes10MB[i]>=InitialSize) {
                InitialSize = Primes10MB[i];
                break;
            }
        }
    } else {
        InitialSize = NextPrime(InitialSize);
    }
    //_LOG("HashTable", NULL, D3_DVLEVEL6,"Real InitialSize is %i",InitialSize);
    //alloc mem for primes
    this->Pair = (HashPair *)MemMgr->AllocMem(InitialSize * sizeof(HashPair), 0);
    this->PairCount = 0;
    this->PairCapacity = InitialSize;
    this->OwnsData = OwnsData;
    //_LOGE(logE,"Initialized");
}

CHashTable::~CHashTable() {
    //int logE = _LOGB("HashTable", NULL,D3_DVLEVEL6,"Hashtable destroyment");
    if (OwnsData) {
        for (UINT i=0; i<this->PairCapacity; i++) {
            if ((Pair[i].Flags & HPAIR_CLASS)!=0) {
                delete (CRootClass *)Pair[i].Value;
            }
        }
    }
    MemMgr->FreeMem(Pair);
    //_LOGE(logE,"Destroyed");
}

//////////////////////////////////////////////////////////////////////
// Grow / Shrink
//////////////////////////////////////////////////////////////////////
void CHashTable::GrowTable(int MinSize) {
    //int logE = _LOGB("HashTable", NULL,D3_DVLEVEL6,"Grow. MinSize is %i", MinSize);
    if ((UINT)MinSize<PairCount) {    //special case - bad parameter, that caues collision.
        GrowTable(PairCount);
        //_LOGE(logE,"Grown");
        return;
    }
    //Backup old items
    int OldCapacity = PairCapacity;
    HashPair *OldPair = Pair;
    //find good new size
    if (MinSize<Primes10MB[sizeof(Primes10MB)/4 - 1]) {
        for (int i=0; i<sizeof(Primes10MB)/4; i++) {
            if (Primes10MB[i]>MinSize) {
                PairCapacity = Primes10MB[i];
                break;
            }
        }
    } else {
        MinSize = MinSize + (MinSize/10);
        PairCapacity = NextPrime(MinSize);
    }
    //_LOG("HashTable", NULL, D3_DVLEVEL6,"Real PairCapacity is %i",PairCapacity);
    //alloc new mem for primes
    Pair = (HashPair *)MemMgr->AllocMem(PairCapacity * sizeof(HashPair), 0);
    //rehash all stored values
    bool RehashAgain = false;   //collision detection in loop
    for (int i=0; i<OldCapacity; i++) {
        if ((OldPair[i].Flags & HPAIR_FILLED) != 0) {
            int hashedKey = GetHash(OldPair[i].Key);
            if ((Pair[hashedKey].Flags & HPAIR_FILLED) != 0) {   //collision !!!
                //_LOG("HashTable", NULL, D3_DVLEVEL6,"Collision! - Key %i collided with Key %i (Hash %i)",Pair[hashedKey].Key, OldPair[i].Key, hashedKey);
                RehashAgain = true;
                break;
            } else {
                Pair[hashedKey].Flags = OldPair[i].Flags;
                Pair[hashedKey].Key = OldPair[i].Key;
                Pair[hashedKey].Value = OldPair[i].Value;
            }
        }
    }
    if (RehashAgain) {
        MemMgr->FreeMem(Pair);
        Pair = OldPair;
        MinSize = PairCapacity;
        PairCapacity = OldCapacity;
        GrowTable(MinSize);
    } else {
        MemMgr->FreeMem(OldPair);
    }
    //_LOGE(logE,"Grown");
}

void CHashTable::Clear() {
    //int logE = _LOGB("HashTable", NULL,D3_DVLEVEL6,"Clear");
    for (UINT i=0; i<PairCapacity; i++) {
        if (((Pair[i].Flags & HPAIR_CLASS) != 0) && (OwnsData)) delete (CRootClass *)Pair[i].Value;
        Pair[i].Flags = 0;
        Pair[i].Key = 0;
        Pair[i].Value = NULL;
    }
    PairCount = 0;
    GrowTable(0);
    //_LOGE(logE,"Clear End");
}
