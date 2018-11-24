#include "stdafx.h"
#include "exportedentity.h"


void SaveAttrib(CStdioFile *File, CString &Indent, char *AttName, CString &Value) {
    CString tmp;
    CString tmpAttName = AttName;
    tmpAttName = tmpAttName.MakeUpper();
    tmp.Format("%s%s = \"%s\"\n", (LPCTSTR)Indent, (LPCTSTR)tmpAttName, (LPCTSTR)Value);
    File->WriteString((LPCTSTR)tmp);
}

void SaveAttrib(CStdioFile *File, CString &Indent, char *AttName, int Value) {
    CString tmp;
    CString tmpAttName = AttName;
    tmpAttName = tmpAttName.MakeUpper();
    tmp.Format("%s%s = \"%i\"\n", (LPCTSTR)Indent, (LPCTSTR)tmpAttName, Value);
    File->WriteString((LPCTSTR)tmp);
}

void SaveAttrib(CStdioFile *File, CString &Indent, char *AttName, float Value) {
    CString tmp;
    CString tmpAttName = AttName;
    tmpAttName = tmpAttName.MakeUpper();
    tmp.Format("%s%s = \"%g\"\n", (LPCTSTR)Indent, (LPCTSTR)tmpAttName, Value);
    File->WriteString((LPCTSTR)tmp);
}

void SaveAttrib(CStdioFile *File, CString &Indent, char *AttName, UINT Value) {
    CString tmp;
    CString tmpAttName = AttName;
    tmpAttName = tmpAttName.MakeUpper();
    tmp.Format("%s%s = \"%u\"\n", (LPCTSTR)Indent, (LPCTSTR)tmpAttName, Value);
    File->WriteString((LPCTSTR)tmp);
}

void SaveAttribXY(CStdioFile *File, CString &Indent, char *AttName, float *ValueXY) {
    CString tmp;
    CString tmpAttName = AttName;
    tmpAttName = tmpAttName.MakeUpper();
    tmp.Format("%s<%s  X = \"%g\"  Y = \"%g\" />\n", (LPCTSTR)Indent, (LPCTSTR)tmpAttName, ValueXY[0], ValueXY[1]);
    File->WriteString((LPCTSTR)tmp);
}

void SaveAttribXYZ(CStdioFile *File, CString &Indent, char *AttName, float *ValueXYZ) {
    CString tmp;
    CString tmpAttName = AttName;
    tmpAttName = tmpAttName.MakeUpper();
    tmp.Format("%s<%s  X = \"%g\"  Y = \"%g\"  Z = \"%g\" />\n", (LPCTSTR)Indent, (LPCTSTR)tmpAttName, ValueXYZ[0], ValueXYZ[1], ValueXYZ[2]);
    File->WriteString((LPCTSTR)tmp);
}

void SaveAttribXYZW(CStdioFile *File, CString &Indent, char *AttName, float *ValueXYZW) {
    CString tmp;
    CString tmpAttName = AttName;
    tmpAttName = tmpAttName.MakeUpper();
    tmp.Format("%s<%s  X = \"%g\"  Y = \"%g\"  Z = \"%g\"  W = \"%g\" />\n", (LPCTSTR)Indent, (LPCTSTR)tmpAttName, ValueXYZW[0], ValueXYZW[1], ValueXYZW[2], ValueXYZW[3]);
    File->WriteString((LPCTSTR)tmp);
}

void SaveAttribMatrix_4x3(CStdioFile *File, CString &Indent, char *AttName, float Matrix[4][3]) {
    CString tmp;
    CString tmpAttName = AttName;
    tmpAttName = tmpAttName.MakeUpper();
    tmp.Format("%s<%s  M_00 = \"%g\"  M_10 = \"%g\"  M_20 = \"%g\"  M_30 = \"0\"\n", (LPCTSTR)Indent, (LPCTSTR)tmpAttName, Matrix[0][0], Matrix[0][1], Matrix[0][2]);
    File->WriteString((LPCTSTR)tmp);
    tmp.Format("%s %*s  M_01 = \"%g\"  M_11 = \"%g\"  M_21 = \"%g\"  M_31 = \"0\"\n", (LPCTSTR)Indent, tmpAttName.GetLength(), "", Matrix[1][0], Matrix[1][1], Matrix[1][2]);
    File->WriteString((LPCTSTR)tmp);
    tmp.Format("%s %*s  M_02 = \"%g\"  M_12 = \"%g\"  M_22 = \"%g\"  M_32 = \"0\"\n", (LPCTSTR)Indent, tmpAttName.GetLength(), "", Matrix[2][0], Matrix[2][1], Matrix[2][2]);
    File->WriteString((LPCTSTR)tmp);
    tmp.Format("%s %*s  M_03 = \"%g\"  M_13 = \"%g\"  M_23 = \"%g\"  M_33 = \"1\" />\n", (LPCTSTR)Indent, tmpAttName.GetLength(), "", Matrix[3][0], Matrix[3][1], Matrix[3][2]);
    File->WriteString((LPCTSTR)tmp);
}

CExportedEntity::CExportedEntity(void) {
}

CExportedEntity::~CExportedEntity(void) {
}

void CExportedEntity::SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
/*
//Use this commentet code in descendants for if node has attributes
    if (Tag!=NULL) {
        tmp.Format("%s<%s\n", (LPCTSTR)IndentTag, (LPCTSTR)*Tag);
        File->WriteString((LPCTSTR)tmp);
        IndentAttribs = IndentAttribs + "    ";
    }
    //Put attributes here
    if (Tag!=NULL) {
        tmp.Format("%s>\n", (LPCTSTR)IndentTag);
        File->WriteString((LPCTSTR)tmp);
    }
*/
    if (Tag!=NULL) {
        tmp.Format("%s<%s>\n", (LPCTSTR)IndentTag, (LPCTSTR)*Tag);
        File->WriteString((LPCTSTR)tmp);
    }
}

void CExportedEntity::SaveToXML_EndTAG(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        tmp.Format("%s</%s>\n", (LPCTSTR)IndentTag, (LPCTSTR)*Tag);
        File->WriteString((LPCTSTR)tmp);
    }
}

void CExportedEntity::SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        IndentAttribs = IndentAttribs + "    ";
    }
    //Put subnodes save here
}

void CExportedEntity::SaveToXML(CStdioFile *File, CString *Tag, CString *Indent) {
    SaveToXML_StartTAG(File, Tag, Indent);
    SaveToXML_InnerBody(File, Tag, Indent);
    SaveToXML_EndTAG(File, Tag, Indent);
}
