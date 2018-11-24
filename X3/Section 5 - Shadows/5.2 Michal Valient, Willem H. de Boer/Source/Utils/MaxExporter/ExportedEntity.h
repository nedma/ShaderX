#pragma once
#include "afx.h"

void SaveAttrib(CStdioFile *File, CString &Indent, char *AttName, CString &Value);
void SaveAttrib(CStdioFile *File, CString &Indent, char *AttName, int Value);
void SaveAttrib(CStdioFile *File, CString &Indent, char *AttName, float Value);
void SaveAttrib(CStdioFile *File, CString &Indent, char *AttName, UINT Value);
void SaveAttribXY(CStdioFile *File, CString &Indent, char *AttName, float *ValueXY);
void SaveAttribXYZ(CStdioFile *File, CString &Indent, char *AttName, float *ValueXYZ);
void SaveAttribXYZW(CStdioFile *File, CString &Indent, char *AttName, float *ValueXYZW);
void SaveAttribMatrix_4x3(CStdioFile *File, CString &Indent, char *AttName, float Matrix[4][3]);

class CExportedEntity : public CObject {
public:
    CExportedEntity(void);
    virtual ~CExportedEntity(void);

    //If Tag is NULL, tag name is not written to XML
    //save whole structure
    virtual void SaveToXML(CStdioFile *File, CString *Tag, CString *Indent);

    //Parts of XML export - save starting tag with attributes
    virtual void SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent);

    //Parts of XML export - save end tag
    virtual void SaveToXML_EndTAG(CStdioFile *File, CString *Tag, CString *Indent);

    //Parts of XML export - save inner nodes (subnodes) and tag text
    virtual void SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent);
};
