// SC-MString.cpp: implementation of the CMString class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SC-MString.h"
#include "SU-StrUtils.h"

//###################################################################
//# Construction/Destruction

CMString::CMString() {
	m_sData=NULL;
}

CMString::CMString(const CMString& sSource) {
	m_sData=suStrReallocCopy(NULL,sSource.m_sData);
}

CMString::CMString(char ch, int iRepeat) {
	if (iRepeat<=0) {
		m_sData=NULL;
		return;
	}
	m_sData=(char *)MemMgr->AllocMem(iRepeat+1,0);
	memset(m_sData,ch,iRepeat);
	m_sData[iRepeat]=0;
}

CMString::CMString(char *sSource) {
	if (sSource==NULL) {
		m_sData=NULL;
		return;
	}
	m_sData=suStrReallocCopy(NULL,sSource);
}

CMString::CMString(const unsigned char* sSource) {
	if (sSource==NULL) {
		m_sData=NULL;
		return;
	}
	m_sData=suStrReallocCopy(NULL,(char *)sSource);
}

CMString::CMString(const char* sSource) {
	if (sSource==NULL) {
		m_sData=NULL;
		return;
	}
	m_sData=suStrReallocCopy(NULL,(char *)sSource);
}

CMString::CMString(const char* sSource, const int Length) {
	if (sSource==NULL) {
		m_sData=NULL;
		return;
	}
    m_sData = (char *)MemMgr->AllocMem(Length+1,0);
    memcpy(m_sData, sSource, Length);
    m_sData[Length] = 0;
}

CMString::~CMString() {
	if (m_sData!=NULL) MemMgr->FreeMem(m_sData);
}

//###################################################################
//# Length Attributes & Operations

int CMString::Length() const { 
	if (m_sData==NULL) return 0;
	else return strlen(m_sData);
}

bool CMString::IsEmpty() const {
	if (m_sData==NULL) return TRUE;
	else if (m_sData[0]==0) return TRUE;
	else return FALSE;
}

void CMString::Empty() {
    if (m_sData!=NULL)MemMgr->FreeMem(m_sData);
    m_sData = NULL;
}

//###################################################################
//# Character manipulation

char CMString::GetAt(int iIndex) const {
	if (iIndex>Length()) return 0;
	else return m_sData[iIndex];
}

char CMString::operator[](int iIndex) const {
	if (iIndex>Length()) return 0;
	else return m_sData[iIndex];
}

void CMString::SetAt(int iIndex, char ch) {
	if (iIndex<=Length()) m_sData[iIndex]=ch;
}

char *CMString::c_str() const {
	return m_sData;
}

//###################################################################
//# Overloaded assigment
const CMString& CMString::operator=(const CMString& sSource) {
	if (m_sData!=sSource.m_sData) {
		int len = sSource.Length();
		if (len == 0) Empty();
		else {
			m_sData=(char *)MemMgr->ReAllocMem(m_sData,len+1,0);
			strcpy(m_sData,sSource.m_sData);
		}
	}
	return *this;
}

const CMString& CMString::operator=(char ch) {
	m_sData=(char *)MemMgr->ReAllocMem(m_sData,2,0);
	m_sData[0] = ch;
	m_sData[1] = 0;
	return *this;
}

const CMString& CMString::operator=(char *sSource) {
	if (m_sData!=sSource) {
		int len = strlen(sSource);
		if (len == 0) Empty();
		else {
			m_sData=(char *)MemMgr->ReAllocMem(m_sData,len+1,0);
			strcpy(m_sData,sSource);
		}
	}
	return *this;
}

const CMString& CMString::operator=(const unsigned char* sSource) {
	if (m_sData!=(char *)sSource) {
		int len = strlen((char *)sSource);
		if (len == 0) Empty();
		else {
			m_sData=(char *)MemMgr->ReAllocMem(m_sData,len+1,0);
			strcpy(m_sData,(char *)sSource);
		}
	}
	return *this;
}

//###################################################################
//# String concatenation

//###################################################################
//# out of place (the + operator)

void CMString::ConcatCopy(int iLen1, char *sString1, int iLen2, char *sString2) {
	Empty();
    int iNewLen = iLen1 + iLen2;
    if (iNewLen != 0) {
		iNewLen++;
        m_sData = (char *)MemMgr->ReAllocMem(m_sData,iNewLen,0);
        memcpy(m_sData, sString1, iLen1);
        memcpy(m_sData+iLen1, sString2, iLen2);
		m_sData[iNewLen-1]=0;
    }
}

CMString operator+(const CMString& string1, const CMString& string2) {
    CMString s;
    s.ConcatCopy(string1.Length(), string1.m_sData,string2.Length(), string2.m_sData);
    return s;
}

CMString operator+(const CMString& string, char *lpsz) {
	if (lpsz==NULL) {
		CMString s(string);
		return s;
	} else {
		CMString s;
		s.ConcatCopy(string.Length(), string.m_sData,strlen(lpsz),lpsz);
		return s;
	}
}

CMString operator+(char *lpsz, const CMString& string)	{
	if (lpsz==NULL) {
		CMString s(string);
		return s;
	} else {
		CMString s;
		s.ConcatCopy(strlen(lpsz),lpsz, string.Length(), string.m_sData);
		return s;
	}
}

CMString operator+(const CMString& string, char lpsz) {
	CMString s;
	char tmp[2];
	tmp[0]=lpsz; tmp[1]=0;
	s.ConcatCopy(string.Length(), string.m_sData,2,tmp);
	return s;
}

CMString operator+(char lpsz, const CMString& string)	{
	CMString s;
	char tmp[2];
	tmp[0]=lpsz; tmp[1]=0;
	s.ConcatCopy(2,tmp,string.Length(), string.m_sData);
	return s;
}

//###################################################################
//# in place (the += operator)
 void CMString::ConcatInPlace(int iLen, char *sString) {
	if ((iLen==0) || (sString==NULL)) return;
	int len1 = Length();
	m_sData = (char *)MemMgr->ReAllocMem(m_sData,len1+iLen+1,0);
	memcpy(m_sData+len1,sString,iLen);
	m_sData[len1+iLen] = 0;
}

const CMString& CMString::operator+=(char *lpsz) {
    ConcatInPlace(strlen(lpsz), lpsz);
    return *this;
}

const CMString& CMString::operator+=(char ch) {
    ConcatInPlace(1, &ch);
    return *this;
}

const CMString& CMString::operator+=(const CMString& string) {
    ConcatInPlace(string.Length(), string.m_sData);
    return *this;
}

//###################################################################
//# string formating...
void CMString::Format(char *format, ...) {
    va_list argList;
    va_start(argList, format);
    Format(format,argList);
    va_end(argList);
}

void CMString::Format(char *format, va_list argPtr) {
	Empty();
	if (format==NULL) return;
	int result = -1;
	if (result==-1) {
		char buf[256];
		result = _vsnprintf(buf, 256, format, argPtr);
		if (result!=-1) m_sData = suStrReallocCopy(m_sData,buf);
	}
	if (result==-1) {
		char buf[512];
		result = _vsnprintf(buf, 512, format, argPtr);
		if (result!=-1) m_sData = suStrReallocCopy(m_sData,buf);
	}
	if (result==-1) {
		char *buf = NULL;
		int size = 512;
		while (result==-1) {
			size = size+2048;
			buf = (char *)MemMgr->ReAllocMem(buf,size,0);
			result = _vsnprintf(buf, size, format, argPtr);
		}
		m_sData = suStrReallocCopy(m_sData,buf);
		MemMgr->FreeMem(buf);
	}
}

CMString CMString::ExtractPath() {
    if (m_sData == NULL) {
        CMString s;
        return s;
    } else {
        char *pdest = strrchr(m_sData, '\\');
        if (pdest!=NULL) {
            int result = (int)(pdest - m_sData);
            m_sData[result] = 0;
            CMString s = m_sData;
            m_sData[result] = '\\';
            return s;
        } else {
            CMString s = m_sData;
            return s;
        }
    }
}

CMString CMString::ExtractFile() {
    if (m_sData == NULL) {
        CMString s;
        return s;
    } else {
        char tmp[2048];
        char *pdest = strrchr(m_sData, '\\');
        if (pdest!=NULL) {
            int result = (int)(pdest - m_sData);
            strcpy(tmp,&m_sData[result+1]);
        } else {    //no path part
            strcpy(tmp,m_sData);
        }
        pdest = strrchr(tmp, '.');
        if (pdest!=NULL) {
            int result = (int)(pdest - tmp);
            tmp[result] = 0;
            CMString s = tmp;
            return s;
        } else {    //no extension
            CMString s = tmp;
            return s;
        }
    }
}

CMString CMString::ExtractExt() {
    if (m_sData == NULL) {
        CMString s;
        return s;
    } else {
        char tmp[2048];
        char *pdest = strrchr(m_sData, '\\');
        if (pdest!=NULL) {
            int result = (int)(pdest - m_sData);
            strcpy(tmp,&m_sData[result+1]);
        } else {    //no path part
            strcpy(tmp,m_sData);
        }
        pdest = strrchr(tmp, '.');
        if (pdest!=NULL) {
            int result = (int)(pdest - tmp);
            CMString s = &tmp[result+1];
            return s;
        } else {    //no extension
            CMString s;
            return s;
        }
    }
}
