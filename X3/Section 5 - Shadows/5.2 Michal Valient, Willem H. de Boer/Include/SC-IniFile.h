// SC-IniFile.h: interface for the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BaseClass.h"

//############################################################################
//Forward declarations
class CRootClassArray;
class CMString;

class CIniFile : public CBaseClass {
protected:
    class CIniFileSection : public CRootClass {
    public:
        CMString        *Name;
        CRootClassArray *Items; 

        CIniFileSection();
        virtual ~CIniFileSection();
    };

    class CIniFileItem : public CRootClass {
    public:
        CMString        *Name;
        CMString        *Value;

        CIniFileItem();
        virtual ~CIniFileItem();
    };

    CRootClassArray     *Sections;
public:
    virtual CMString ReadString(CMString &Section, CMString &Name, CMString &Default);
    virtual CMString ReadString(char *Section, char *Name, char *Default);
    virtual bool ReadBool(CMString &Section, CMString &Name, bool Default);
    virtual bool ReadBool(char *Section, char *Name, bool Default);
    virtual int ReadInt(CMString &Section, CMString &Name, int Default);
    virtual int ReadInt(char *Section, char *Name, int Default);

    void Load(char *Source, UINT Size);
    void Unload();
    CIniFile(char *ObjName,CEngine *OwnerEngine);
	virtual ~CIniFile();

    MAKE_CLSCREATE(CIniFile);
    MAKE_CLSNAME("CIniFile");
    MAKE_DEFAULTGARBAGE()
};

