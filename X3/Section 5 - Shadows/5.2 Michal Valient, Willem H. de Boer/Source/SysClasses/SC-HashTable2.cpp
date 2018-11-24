// SC-HashTable.cpp: implementation of the CHashTable2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SC-HashTable2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHashTable2::CHashTable2(const bool OwnsData, const UINT InitialSize) {
    UINT tmpInitialSize = InitialSize;
    int logE = _LOGB("HashTable", NULL ,D3_DV_GRP3_LEV4,"Hashtable initialization (InitialSize is %i)",tmpInitialSize);
    this->PairCount = 0;
    this->OwnsData = OwnsData;
    //find good initial size
    if (tmpInitialSize<=Primes10MB[(sizeof(Primes10MB)/4)-1]) {
        for (int i=0; i<sizeof(Primes10MB)/4; i++) {
            if (Primes10MB[i]>=tmpInitialSize) {
                tmpInitialSize = Primes10MB[i];
                break;
            }
        }
    } else {
        tmpInitialSize = NextPrime(tmpInitialSize);
    }
    _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"Real InitialSize is %i",tmpInitialSize);
    //alloc mem for primes
    AllocateMemory(tmpInitialSize, false);
    _LOGE(logE,"Initialized");
}

CHashTable2::~CHashTable2() {
    int logE = _LOGB("HashTable", NULL,D3_DV_GRP3_LEV4,"Hashtable destroyment");
    if (OwnsData) {
        for (UINT i=0; i<this->PairCapacity; i++) {
            if ((Pair[i].Value!=(void *)&EmptyValue) && ((int *)Pair[i].Key!=&EmptyKey)) {
                delete (CRootClass *)Pair[i].Value;
            }
        }
    }
    MemMgr->FreeMem(Pair);
    _LOGE(logE,"Destroyed");
}

//////////////////////////////////////////////////////////////////////
// Grow / Shrink
//////////////////////////////////////////////////////////////////////
void CHashTable2::AllocateMemory(const UINT ItemCount, bool Reallocate) {
    _LOG("HashTable", NULL,D3_DV_GRP3_LEV4,"Allocating memory for %u items", ItemCount);
    if (Reallocate) Pair = (HashPair *)MemMgr->ReAllocMem(Pair, ItemCount * sizeof(HashPair), 0);
    else Pair = (HashPair *)MemMgr->AllocMem(ItemCount * sizeof(HashPair), 0);
    for (UINT i=0; i<ItemCount; i++) {
        Pair[i].Key = (int)&EmptyKey;
        Pair[i].Value = (void *)&EmptyValue;
    }
    this->PairCapacity = ItemCount;
}

void CHashTable2::GrowTable(const UINT MinSize, const bool Rehash, const bool AddNew, const UINT Key, const void *Value) {
    UINT MinimumSize = MinSize;
    int logE = _LOGB("HashTable", NULL,D3_DV_GRP3_LEV4,"Grow. MinimumSize is %i", MinimumSize);
    if ((UINT)MinimumSize<PairCount + ((AddNew)?1:0)) {    //special case - bad parameter, that caues collisions.
        _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"Minimum size is less than item count (%i<%i) - change of parameter",MinimumSize, PairCount + ((AddNew)?1:0));
        MinimumSize = PairCount;
    }
    //Backup old items
    UINT OldCapacity = PairCapacity;
    HashPair *OldPair = Pair;
    bool RehashAgain = true;
    while (RehashAgain) {
        //find good new size
        if (MinimumSize<Primes10MB[sizeof(Primes10MB)/4 - 1]) {
            for (int i=0; i<sizeof(Primes10MB)/4; i++) {
                if (Primes10MB[i]>MinimumSize) {
                    PairCapacity = Primes10MB[i];
                    break;
                }
            }
        } else {
            MinimumSize = MinimumSize + (MinimumSize / 8);
            PairCapacity = NextPrime(MinimumSize);
        }
        _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"Real PairCapacity is %i",PairCapacity);
        AllocateMemory(PairCapacity, false);       //alloc new mem for values
        if (Rehash) {                       //rehash all stored values
            RehashAgain = false;            //collision detection in loop
            if (AddNew) {                   //If adding new one, add it first to catch collisions earlier
                UINT hashedKey = GetHash(Key);
                Pair[hashedKey].Key = Key;
                Pair[hashedKey].Value = (void *)Value;
            }
            for (UINT i=0; i<OldCapacity; i++) {
                if ((OldPair[i].Value!=(void *)&EmptyValue) && ((int *)OldPair[i].Key!=&EmptyKey)) {
                    UINT hashedKey = GetHash(OldPair[i].Key);
                    if ((Pair[hashedKey].Value!=(void *)&EmptyValue) && ((int *)Pair[hashedKey].Key!=&EmptyKey)) {   //collision !!!
                        _LOG("HashTable", NULL, D3_DV_GRP3_LEV4,"Collision! - Key %i collided with Key %i (Hash %i)",Pair[hashedKey].Key, OldPair[i].Key, hashedKey);
                        RehashAgain = true;
                        break;
                    } else {
                        Pair[hashedKey].Key = OldPair[i].Key;
                        Pair[hashedKey].Value = OldPair[i].Value;
                    }
                }
            }
            if (RehashAgain) {
                MemMgr->FreeMem(Pair);
                MinimumSize = PairCapacity;
            } else {
                MemMgr->FreeMem(OldPair);
                if (AddNew) PairCount++;
            }
        } else {
            RehashAgain = false;
            MemMgr->FreeMem(OldPair);
        }
    }
    _LOGE(logE,"Grown");
}

void CHashTable2::Clear(const bool DoShrink) {
    int logE = _LOGB("HashTable", NULL,D3_DV_GRP3_LEV4,"Clear");
    for (UINT i=0; i<PairCapacity; i++) {
        if ((OwnsData) && (Pair[i].Value!=(void *)&EmptyValue) && ((int *)Pair[i].Key!=&EmptyKey)) {
            delete (CRootClass *)Pair[i].Value;
        }
        Pair[i].Key = (int)&EmptyKey;
        Pair[i].Value = (void *)&EmptyValue;
    }
    PairCount = 0;
    if (DoShrink) GrowTable(0, false, false, 0, NULL);
    _LOGE(logE,"Clear End");
}
