#include "stdafx.h"
#include "exportedobject.h"

CExportedObject::CExportedObject(void) {
    ID = ParentID = MasterID = TargetID = MeshID = -1;
    LocalPRS = 0;
    IsTarget = 0;
}

CExportedObject::~CExportedObject(void) {
}

void CExportedObject::CreateFromFlexporter(const ObjectDescriptor& Obj) {
    Name = Obj.mName;

    ID = Obj.mObjectID;
    ParentID = Obj.mParentID;
    MasterID = Obj.mMasterID;
    TargetID = Obj.mTargetID;

    PRS.Position[0] = Obj.mPrs.Position.x;
    PRS.Position[1] = Obj.mPrs.Position.y;
    PRS.Position[2] = Obj.mPrs.Position.z;

    PRS.Rotation[0] = Obj.mPrs.Rotation.x;
    PRS.Rotation[1] = Obj.mPrs.Rotation.y;
    PRS.Rotation[2] = Obj.mPrs.Rotation.z;
    PRS.Rotation[3] = Obj.mPrs.Rotation.w;

    PRS.Scale[0] = Obj.mPrs.Scale.x;
    PRS.Scale[1] = Obj.mPrs.Scale.y;
    PRS.Scale[2] = Obj.mPrs.Scale.z;

    LocalPRS = (Obj.mLocalPRS)?1:0;

    LocalPivot.Position[0] = Obj.mPivot.LocalPivot.Position.x;
    LocalPivot.Position[1] = Obj.mPivot.LocalPivot.Position.y;
    LocalPivot.Position[2] = Obj.mPivot.LocalPivot.Position.z;

    LocalPivot.Rotation[0] = Obj.mPivot.LocalPivot.Rotation.x;
    LocalPivot.Rotation[1] = Obj.mPivot.LocalPivot.Rotation.y;
    LocalPivot.Rotation[2] = Obj.mPivot.LocalPivot.Rotation.z;
    LocalPivot.Rotation[3] = Obj.mPivot.LocalPivot.Rotation.w;

    LocalPivot.Scale[0] = Obj.mPivot.LocalPivot.Scale.x;
    LocalPivot.Scale[1] = Obj.mPivot.LocalPivot.Scale.y;
    LocalPivot.Scale[2] = Obj.mPivot.LocalPivot.Scale.z;

    WorldPivot.Position[0] = Obj.mPivot.WorldPivot.Position.x;
    WorldPivot.Position[1] = Obj.mPivot.WorldPivot.Position.y;
    WorldPivot.Position[2] = Obj.mPivot.WorldPivot.Position.z;

    WorldPivot.Rotation[0] = Obj.mPivot.WorldPivot.Rotation.x;
    WorldPivot.Rotation[1] = Obj.mPivot.WorldPivot.Rotation.y;
    WorldPivot.Rotation[2] = Obj.mPivot.WorldPivot.Rotation.z;
    WorldPivot.Rotation[3] = Obj.mPivot.WorldPivot.Rotation.w;

    WorldPivot.Scale[0] = Obj.mPivot.WorldPivot.Scale.x;
    WorldPivot.Scale[1] = Obj.mPivot.WorldPivot.Scale.y;
    WorldPivot.Scale[2] = Obj.mPivot.WorldPivot.Scale.z;

    IsInstance = (Obj.mIsInstance)?1:0;
}

void PosRotScale::SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        tmp.Format("%s<%s>\n", (LPCTSTR)IndentTag, (LPCTSTR)*Tag);
        File->WriteString((LPCTSTR)tmp);
        IndentAttribs = IndentAttribs + "    ";
    }
}

void PosRotScale::SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        IndentAttribs = IndentAttribs + "    ";
    }
    SaveAttribXYZ(File, IndentAttribs, "Position", Position);
    SaveAttribXYZW(File, IndentAttribs, "Rotation", Rotation);
    SaveAttribXYZ(File, IndentAttribs, "Scale", Scale);
}

void CExportedObject::SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        tmp.Format("%s<%s\n", (LPCTSTR)IndentTag, (LPCTSTR)*Tag);
        File->WriteString((LPCTSTR)tmp);
        IndentAttribs = IndentAttribs + "    ";
    }
    SaveAttrib(File, IndentAttribs, "NAME", Name);
    SaveAttrib(File, IndentAttribs, "ID", ID);
    SaveAttrib(File, IndentAttribs, "PARENTID", ParentID);
    SaveAttrib(File, IndentAttribs, "MASTERID", MasterID);
    SaveAttrib(File, IndentAttribs, "TARGETID", TargetID);
    SaveAttrib(File, IndentAttribs, "MESHID", MeshID);
    SaveAttrib(File, IndentAttribs, "LOCALPRS", LocalPRS);
    SaveAttrib(File, IndentAttribs, "ISTARGET", IsTarget);
    SaveAttrib(File, IndentAttribs, "ISINSTANCE", IsInstance);
    if (Tag!=NULL) {
        tmp.Format("%s>\n", (LPCTSTR)IndentTag);
        File->WriteString((LPCTSTR)tmp);
    }
}

void CExportedObject::SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        IndentAttribs = IndentAttribs + "    ";
    }
    tmp = "PRS";
    PRS.SaveToXML(File, &tmp, &IndentAttribs);
    tmp = "LOCALPIVOT";
    LocalPivot.SaveToXML(File, &tmp, &IndentAttribs);
    tmp = "WORLDPIVOT";
    WorldPivot.SaveToXML(File, &tmp, &IndentAttribs);
}
