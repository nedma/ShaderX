#pragma once
#include "exportedentity.h"

class CExportedMaterial : public CExportedEntity {
public:
    int      ID;
    CString  Name;

    int      AmbientMapID;
    int      DiffuseMapID;
    int      SpecularMapID;
    int      ShininessMapID;
    int      ShiningStrengthMapID;
    int      SelfIllumMapID;
    int      OpacityMapID;
    int      FilterMapID;
    int      BumpMapID;
    int      ReflexionMapID;
    int      RefractionMapID;
    int      DisplacementMapID;
    // Amounts
    float    AmbientCoeff;
    float    DiffuseCoeff;
    float    SpecularCoeff;
    float    ShininessCoeff;
    float    ShiningStrengthCoeff;
    float    SelfIllumCoeff;
    float    OpacityCoeff;
    float    FilterCoeff;
    float    BumpCoeff;
    float    ReflexionCoeff;
    float    RefractionCoeff;
    float    DisplacementCoeff;
    // Colors
    float    MtlAmbientColor[3];
    float    MtlDiffuseColor[3];
    float    MtlSpecularColor[3];
    float    MtlFilterColor[3];
    // Static properties
    int      Shading;
    int      Soften;
    int      FaceMap;
    int      TwoSided;
    int      Wire;
    int      WireUnits;
    int      FalloffOut;
    int      TransparencyType;
    // Dynamic properties
    float    Shininess;
    float    ShiningStrength;
    float    SelfIllum;
    float    Opacity;
    float    OpaFalloff;
    float    WireSize;
    float    IOR;
    // Dynamic properties
    float    Bounce;
    float    StaticFriction;
    float    SlidingFriction;
    // 1.15: enhanced self-illum
    int      SelfIllumOn;
    float    SelfIllumValue;
    float    SelfIllumColor[3];

    CExportedMaterial(void);
    virtual ~CExportedMaterial(void);

    virtual void CreateFromFlexporter(const MaterialDescriptor& Obj);

    //Parts of XML export - save starting tag with attributes
    virtual void SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent);

    //Parts of XML export - save inner nodes (subnodes) and tag text
    virtual void SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent);
};
