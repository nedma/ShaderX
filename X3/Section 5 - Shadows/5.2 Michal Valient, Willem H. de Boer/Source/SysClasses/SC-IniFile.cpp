// SC-IniFile.cpp: implementation of the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SC-IniFile.h"
#include "SC-MString.h"
#include "SC-DynamicArray.h"
#include "SC-SyntaxScanner.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIniFile::CIniFile(char *ObjName,CEngine *OwnerEngine) : CBaseClass(ObjName,OwnerEngine){
    int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Creating INI File");
    Sections = NULL;
    _LOGE(logE);
}

void CIniFile::Load(char *Source, UINT Size) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV2,"Loading and starting of parse");

    if (Sections!=NULL) delete Sections;
    Sections = new CRootClassArray(TRUE);
    CIniFileSection *curr = NULL;

    CSyntaxScanner *scan = new CSyntaxScanner();
    scan->SetSource(Source,Size,FALSE);
    while (1) {
        scan->Flags = SS_FLAG_NONE;
        int ret = scan->ScanNext();
        if (ret == SS_FOUND_EOF) {
            break;
        } else if (ret == SS_FOUND_TERMINAL && scan->AsChar() == '[') {
            ret = scan->ScanNext();
            CMString SectionName = scan->AsString();
            ret = scan->ScanNext();
            if (ret == SS_FOUND_TERMINAL && scan->AsChar() == ']') {
                _LOG(this,D3_DV_GRP2_LEV4,"Found section '%s'",SectionName.c_str());
                curr = new CIniFileSection();
                *curr->Name = SectionName;
                curr->Items = new CRootClassArray(TRUE);
                Sections->Add(curr);
            }
        } else if (ret == SS_FOUND_WORD) {
            CMString name = scan->AsString();
            ret = scan->ScanNext();
            if (ret == SS_FOUND_TERMINAL && scan->AsChar() == '=') {
                scan->Flags = SS_FLAG_NEWLINE || SS_FLAG_SPACE;
                CMString value;
                while (1) {
                    ret = scan->ScanNext();
                    if (ret == SS_FOUND_EOF) break;
                    if (ret == SS_FOUND_EOLN) break;
                    value+=scan->AsString();
                }
                if (curr!=NULL) {
                    CIniFileItem *item = new CIniFileItem();
                    *item->Name = name;
                    *item->Value = value;
                    _LOG(this,D3_DV_GRP2_LEV4,"Found key '%s' with value '%s'",name.c_str(),value.c_str());
                    curr->Items->Add(item);
                }
            }
        }
     }
    delete scan;
    _LOGE(logE,"Parsed INI");
}

void CIniFile::Unload() {
    int logE = _LOGB(this,D3_DV_GRP2_LEV2,"Unloading INI contents");
    if (Sections!=NULL) delete Sections;
    Sections = NULL;
    _LOGE(logE,"Unloaded");
}

CIniFile::~CIniFile() {
    int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Destroying INI");
    Unload();
    _LOGE(logE,"Destroyed INI");
}

CMString CIniFile::ReadString(CMString &Section, CMString &Name, CMString &Default) {
    for (int i=0;i<Sections->GetSize(); i++) {
        CIniFileSection *section = (CIniFileSection *)Sections->GetAt(i);
        if (_stricmp(section->Name->c_str(), Section.c_str())==0) {
            for (int j=0; j<section->Items->GetSize(); j++) {
                CIniFileItem *item = (CIniFileItem *)section->Items->GetAt(j);
                if (_stricmp(item->Name->c_str(),Name.c_str())==0) {
                    _LOG(this,D3_DV_GRP2_LEV4,"Looking for '%s' in section '%s'. Returning founded value '%s'",Name.c_str(),Section.c_str(), item->Value->c_str());
                    return *item->Value;
                }
            }
        }
    }
    _LOG(this,D3_DV_GRP2_LEV4,"Looking for '%s' in section '%s'. Returning default '%s'",Name.c_str(),Section.c_str(), Default.c_str());
    return Default;
}

CMString CIniFile::ReadString(char *Section, char *Name, char *Default) {
    CMString XS = Section;
    CMString XN = Name;
    CMString XD = Default;
    return ReadString(XS,XN,XD);
}

bool CIniFile::ReadBool(CMString &Section, CMString &Name, bool Default) {
    CMString def = (Default) ? "1" : "0";
    CMString ret = ReadString(Section,Name,def);
    if (ret.GetAt(0)=='1') return TRUE;
    else return FALSE;
}

bool CIniFile::ReadBool(char *Section, char *Name, bool Default) {
    CMString XS = Section;
    CMString XN = Name;
    return ReadBool(XS,XN,Default);
}

int CIniFile::ReadInt(CMString &Section, CMString &Name, int Default) {
    CMString def = "X";
    CMString ret = ReadString(Section,Name,def);
    int ret2 = atoi(ret.c_str());
    if (ret2==0 && !isdigit(ret.GetAt(0))) return Default;
    else return ret2;
}

int CIniFile::ReadInt(char *Section, char *Name, int Default) {
    CMString XS = Section;
    CMString XN = Name;
    return ReadInt(XS,XN,Default);
}


CIniFile::CIniFileSection::CIniFileSection() {
    Name = new CMString;
    Items = NULL;
}

CIniFile::CIniFileSection::~CIniFileSection() {
    if (Items!=NULL) delete Items;
    delete Name;
}


CIniFile::CIniFileItem::CIniFileItem() {
    Name = new CMString;
    Value = new CMString;
}

CIniFile::CIniFileItem::~CIniFileItem() {
    delete Name;
    delete Value;
}
