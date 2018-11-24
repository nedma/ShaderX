#include "stdafx.h"
#include "exportedlight.h"

CExportedLight::CExportedLight(void) : CExportedObject() {
}

CExportedLight::~CExportedLight(void) {
}

void CExportedLight::CreateFromFlexporter(const LightDescriptor& Obj) {
    CExportedObject::CreateFromFlexporter(Obj);
    if (Obj.mLightType == LTYPE_OMNI) LightType = "OMNI";
    else if (Obj.mLightType == LTYPE_TSPOT) LightType = "TARGETED_SPOTLIGHT";
    else if (Obj.mLightType == LTYPE_DIR) LightType = "DIRECTIONAL";
    else if (Obj.mLightType == LTYPE_FSPOT) LightType = "FREE_SPOTLIGHT";
    else if (Obj.mLightType == LTYPE_TDIR) LightType = "TARGETED_DIRECTIONAL";
    Color[0] = Obj.mColor.r;
    Color[1] = Obj.mColor.g;
    Color[2] = Obj.mColor.b;
    Intensity = Obj.mIntensity;
    Contrast = Obj.mContrast;
    DiffuseSoft = Obj.mDiffuseSoft;
    LightUsed = (Obj.mLightUsed)?1:0;
	AffectDiffuse = (Obj.mAffectDiffuse)?1:0;
	AffectSpecular = (Obj.mAffectSpecular)?1:0;
	UseAttenNear = (Obj.mUseAttenNear)?1:0;
    UseAttenFar = (Obj.mUseAtten)?1:0;
    NearAttenStart = Obj.mNearAttenStart;
	NearAttenEnd = Obj.mNearAttenEnd;
    FarAttenStart = Obj.mAttenStart;
	FarAttenEnd = Obj.mAttenEnd;
    if (Obj.mDecayType == DECAYTYPE_NONE) DecayType = "NONE";
    else if (Obj.mDecayType == DECAYTYPE_INV) DecayType = "INVERSE";
    else if (Obj.mDecayType == DECAYTYPE_INVSQ) DecayType = "INVERSE_SQUARE";
    HotSpot = Obj.mHotSpot;
    Falloff = Obj.mFallsize;
    Aspect = Obj.mAspect;
    if (Obj.mSpotShape == SPOTSHP_RECT) SpotShape = "RECTANGLE";
    else if (Obj.mSpotShape == SPOTSHP_CIRCLE) SpotShape = "CIRCLE";
    ShadowType = Obj.mShadowType;
    AbsMapBias = Obj.mAbsMapBias;
    RayBias = Obj.mRayBias;
    MapBias = Obj.mMapBias;
    MapRange = Obj.mMapRange;
    MapSize = Obj.mMapSize;
    CastShadows = (Obj.mCastShadows)?1:0;
    ShadowColor[0] = Obj.mShadowColor.r;
    ShadowColor[1] = Obj.mShadowColor.g;
    ShadowColor[2] = Obj.mShadowColor.b;
    LightAffectsShadow = (Obj.mLightAffectsShadow)?1:0;
    ShadowDensity = Obj.mShadowDensity;
    ProjMapID = Obj.mProjMapID;
    ShadowProjMapID = Obj.mShadowProjMapID;
}

void CExportedLight::SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        tmp.Format("%s<%s\n", (LPCTSTR)IndentTag, (LPCTSTR)*Tag);
        File->WriteString((LPCTSTR)tmp);
        IndentAttribs = IndentAttribs + "    ";
    }
    CExportedObject::SaveToXML_StartTAG(File, NULL, &IndentAttribs);
    SaveAttrib(File, IndentAttribs, "LightType", LightType);
    SaveAttrib(File, IndentAttribs, "Intensity", Intensity);
    SaveAttrib(File, IndentAttribs, "Contrast", Contrast);
    SaveAttrib(File, IndentAttribs, "DiffuseSoft", DiffuseSoft);
    SaveAttrib(File, IndentAttribs, "LightUsed", LightUsed);
    SaveAttrib(File, IndentAttribs, "AffectDiffuse", AffectDiffuse);
    SaveAttrib(File, IndentAttribs, "AffectSpecular", AffectSpecular);
    SaveAttrib(File, IndentAttribs, "UseAttenNear", UseAttenNear);
    SaveAttrib(File, IndentAttribs, "UseAttenFar", UseAttenFar);
    SaveAttrib(File, IndentAttribs, "NearAttenStart", NearAttenStart);
    SaveAttrib(File, IndentAttribs, "NearAttenEnd", NearAttenEnd);
    SaveAttrib(File, IndentAttribs, "FarAttenStart", FarAttenStart);
    SaveAttrib(File, IndentAttribs, "FarAttenEnd", FarAttenEnd);
    SaveAttrib(File, IndentAttribs, "DecayType", DecayType);
    SaveAttrib(File, IndentAttribs, "HotSpot", HotSpot);
    SaveAttrib(File, IndentAttribs, "Falloff", Falloff);
    SaveAttrib(File, IndentAttribs, "Aspect", Aspect);
    SaveAttrib(File, IndentAttribs, "SpotShape", SpotShape);
    SaveAttrib(File, IndentAttribs, "ShadowType", ShadowType);
    SaveAttrib(File, IndentAttribs, "AbsMapBias", AbsMapBias);
    SaveAttrib(File, IndentAttribs, "RayBias", RayBias);
    SaveAttrib(File, IndentAttribs, "MapBias", MapBias);
    SaveAttrib(File, IndentAttribs, "MapRange", MapRange);
    SaveAttrib(File, IndentAttribs, "MapSize", MapSize);
    SaveAttrib(File, IndentAttribs, "CastShadows", CastShadows);
    SaveAttrib(File, IndentAttribs, "LightAffectsShadow", LightAffectsShadow);
    SaveAttrib(File, IndentAttribs, "ShadowDensity", ShadowDensity);
    SaveAttrib(File, IndentAttribs, "ProjMapID", ProjMapID);
    SaveAttrib(File, IndentAttribs, "ShadowProjMapID", ShadowProjMapID);
    if (Tag!=NULL) {
        tmp.Format("%s>\n", (LPCTSTR)IndentTag);
        File->WriteString((LPCTSTR)tmp);
    }
}

void CExportedLight::SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        IndentAttribs = IndentAttribs + "    ";
    }
    CExportedObject::SaveToXML_InnerBody(File, NULL, &IndentAttribs);
    SaveAttribXYZ(File, IndentAttribs, "ShadowColor", ShadowColor);
    SaveAttribXYZ(File, IndentAttribs, "Color", Color);
}
