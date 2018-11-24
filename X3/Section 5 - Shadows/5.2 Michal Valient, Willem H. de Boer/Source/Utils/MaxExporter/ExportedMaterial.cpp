#include "stdafx.h"
#include "exportedmaterial.h"

CExportedMaterial::CExportedMaterial(void) {
}

CExportedMaterial::~CExportedMaterial(void) {
}

#define SetM(Att) {Att = Obj.m##Att;}
#define SetMBool(Att) {Att = (Obj.m##Att)?1:0;}
#define SetColor(Att) {Att[0] = Obj.m##Att.r; Att[1] = Obj.m##Att.g; Att[2] = Obj.m##Att.b;}

void CExportedMaterial::CreateFromFlexporter(const MaterialDescriptor& Obj) {
    ID = Obj.mObjectID;
    Name = Obj.mName;

    SetM(AmbientMapID);
    SetM(DiffuseMapID);
    SetM(SpecularMapID);
    SetM(ShininessMapID);
    SetM(ShiningStrengthMapID);
    SetM(SelfIllumMapID);
    SetM(OpacityMapID);
    SetM(FilterMapID);
    SetM(BumpMapID);
    SetM(ReflexionMapID);
    SetM(RefractionMapID);
    SetM(DisplacementMapID);

    SetM(AmbientCoeff);
    SetM(DiffuseCoeff);
    SetM(SpecularCoeff);
    SetM(ShininessCoeff);
    SetM(ShiningStrengthCoeff);
    SetM(SelfIllumCoeff);
    SetM(OpacityCoeff);
    SetM(FilterCoeff);
    SetM(BumpCoeff);
    SetM(ReflexionCoeff);
    SetM(RefractionCoeff);
    SetM(DisplacementCoeff);

    SetColor(MtlAmbientColor);
    SetColor(MtlDiffuseColor);
    SetColor(MtlSpecularColor);
    SetColor(MtlFilterColor);

    SetM(Shading);
    SetMBool(Soften);
    SetMBool(FaceMap);
    SetMBool(TwoSided);
    SetMBool(Wire);
    SetMBool(WireUnits);
    SetMBool(FalloffOut);
    SetM(TransparencyType);

    SetM(Shininess);
    SetM(ShiningStrength);
    SetM(SelfIllum);
    SetM(Opacity);
    SetM(OpaFalloff);
    SetM(WireSize);
    SetM(IOR);

    SetM(Bounce);
    SetM(StaticFriction);
    SetM(SlidingFriction);

    SetM(SelfIllumOn);
    SetM(SelfIllumValue);
    SetColor(SelfIllumColor);
}

void CExportedMaterial::SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        tmp.Format("%s<%s\n", (LPCTSTR)IndentTag, (LPCTSTR)*Tag);
        File->WriteString((LPCTSTR)tmp);
        IndentAttribs = IndentAttribs + "    ";
    }
    SaveAttrib(File, IndentAttribs, "ID", ID);
    SaveAttrib(File, IndentAttribs, "Name", Name);

    SaveAttrib(File, IndentAttribs, "AmbientMapID", AmbientMapID);
    SaveAttrib(File, IndentAttribs, "DiffuseMapID", DiffuseMapID);
    SaveAttrib(File, IndentAttribs, "SpecularMapID", SpecularMapID);
    SaveAttrib(File, IndentAttribs, "ShininessMapID", ShininessMapID);
    SaveAttrib(File, IndentAttribs, "ShiningStrengthMapID", ShiningStrengthMapID);
    SaveAttrib(File, IndentAttribs, "SelfIllumMapID", SelfIllumMapID);
    SaveAttrib(File, IndentAttribs, "OpacityMapID", OpacityMapID);
    SaveAttrib(File, IndentAttribs, "FilterMapID", FilterMapID);
    SaveAttrib(File, IndentAttribs, "BumpMapID", BumpMapID);
    SaveAttrib(File, IndentAttribs, "ReflectionMapID", ReflexionMapID);
    SaveAttrib(File, IndentAttribs, "RefractionMapID", RefractionMapID);
    SaveAttrib(File, IndentAttribs, "DisplacementMapID", DisplacementMapID);

    SaveAttrib(File, IndentAttribs, "AmbientCoeff", AmbientCoeff);
    SaveAttrib(File, IndentAttribs, "DiffuseCoeff", DiffuseCoeff);
    SaveAttrib(File, IndentAttribs, "SpecularCoeff", SpecularCoeff);
    SaveAttrib(File, IndentAttribs, "ShininessCoeff", ShininessCoeff);
    SaveAttrib(File, IndentAttribs, "ShiningStrengthCoeff", ShiningStrengthCoeff);
    SaveAttrib(File, IndentAttribs, "SelfIllumCoeff", SelfIllumCoeff);
    SaveAttrib(File, IndentAttribs, "OpacityCoeff", OpacityCoeff);
    SaveAttrib(File, IndentAttribs, "FilterCoeff", FilterCoeff);
    SaveAttrib(File, IndentAttribs, "BumpCoeff", BumpCoeff);
    SaveAttrib(File, IndentAttribs, "ReflectionCoeff", ReflexionCoeff);
    SaveAttrib(File, IndentAttribs, "RefractionCoeff", RefractionCoeff);
    SaveAttrib(File, IndentAttribs, "DisplacementCoeff", DisplacementCoeff);

    SaveAttrib(File, IndentAttribs, "Shading", Shading);
    SaveAttrib(File, IndentAttribs, "Soften", Soften);
    SaveAttrib(File, IndentAttribs, "FaceMap", FaceMap);
    SaveAttrib(File, IndentAttribs, "TwoSided", TwoSided);
    SaveAttrib(File, IndentAttribs, "Wire", Wire);
    SaveAttrib(File, IndentAttribs, "WireUnits", WireUnits);
    SaveAttrib(File, IndentAttribs, "FalloffOut", FalloffOut);
    SaveAttrib(File, IndentAttribs, "TransparencyType", TransparencyType);

    SaveAttrib(File, IndentAttribs, "Shininess", Shininess);
    SaveAttrib(File, IndentAttribs, "ShiningStrength", ShiningStrength);
    SaveAttrib(File, IndentAttribs, "SelfIllum", SelfIllum);
    SaveAttrib(File, IndentAttribs, "Opacity", Opacity);
    SaveAttrib(File, IndentAttribs, "OpacityFalloff", OpaFalloff);
    SaveAttrib(File, IndentAttribs, "WireSize", WireSize);
    SaveAttrib(File, IndentAttribs, "IOR", IOR);

    SaveAttrib(File, IndentAttribs, "Bounce", Bounce);
    SaveAttrib(File, IndentAttribs, "StaticFriction", StaticFriction);
    SaveAttrib(File, IndentAttribs, "SlidingFriction", SlidingFriction);

    SaveAttrib(File, IndentAttribs, "SelfIllumOn", SelfIllumOn);
    SaveAttrib(File, IndentAttribs, "SelfIllumValue", SelfIllumValue);

    if (Tag!=NULL) {
        tmp.Format("%s>\n", (LPCTSTR)IndentTag);
        File->WriteString((LPCTSTR)tmp);
    }
}

void CExportedMaterial::SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        IndentAttribs = IndentAttribs + "    ";
    }
    SaveAttribXYZ(File, IndentAttribs, "AmbientColor", MtlAmbientColor);
    SaveAttribXYZ(File, IndentAttribs, "DiffuseColor", MtlDiffuseColor);
    SaveAttribXYZ(File, IndentAttribs, "SpecularColor", MtlSpecularColor);
    SaveAttribXYZ(File, IndentAttribs, "FilterColor", MtlFilterColor);
    SaveAttribXYZ(File, IndentAttribs, "SelfIllumColor", SelfIllumColor);
}
