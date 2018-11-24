#include "stdafx.h"
#include "SU-StrUtils.h"
#include "GLobal.h"
#include "MGR-MemoryManager.h"

char *suStrReallocCopy(char *sDest, char *sSrc) {
    UINT len = strlen(sSrc)+1;
    void *ret=MemMgr->ReAllocMem(sDest,len,0);
    memcpy(ret,sSrc,len);
    return (char *)ret;
}

UINT suStrCheckSum(char *sStr) {
    unsigned char c1=0,c2=0,c3=0,c4=0;
    UINT str_len = strlen(sStr);
    while (str_len) {
        c1=(char)(c1+*sStr);
        c2=(char)(c2+c1);
        c3=(char)(c3+c2);
        c4=(char)(c4+c3);
        sStr=sStr+1;
        str_len--;
    }
    return (c1 | (c2<<8) | (c3<<16) | (c4<<24) );
}