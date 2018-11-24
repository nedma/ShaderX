#include "stdafx.h"
#include "d3format\d3f_Material.h"

CD3F_Material::CD3F_Material(char *ObjName,CEngine *OwnerEngine) : CBaseClass(ObjName,OwnerEngine) {
}

CD3F_Material::~CD3F_Material(void) {
}

void CD3F_Material::LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition) {
    memcpy(&m,&Buffer[CurrPosition],sizeof(m));
    CurrPosition+=sizeof(m);
    if (m.TAG!='RTAM') {
        char tmp[5];
        memcpy(tmp,&m.TAG,4);
        tmp[4] = 0;
        _WARN(NULL,"CD3F_Material",ErrMgr->TFile,"Bad tag at %u found '%s' expecting 'MATR' (DataSize is %u)",CurrPosition,tmp,sizeof(m));
    }

    this->ID = m.ID;
    this->Name = m.Name;
    SelfIllumColor.LoadBIN(Buffer, Length, CurrPosition);
    MtlAmbientColor.LoadBIN(Buffer, Length, CurrPosition);
    MtlDiffuseColor.LoadBIN(Buffer, Length, CurrPosition);
    MtlSpecularColor.LoadBIN(Buffer, Length, CurrPosition);
    MtlFilterColor.LoadBIN(Buffer, Length, CurrPosition);
}

void CD3F_Material::SaveBIN(FILE *OutFile) {
    m.TAG = 'RTAM';
    memset(m.Name,' ',sizeof(m.Name));
    strcpy(m.Name,Name.c_str());
    m.ID = ID;
    fwrite(&m,sizeof(m),1,OutFile);
    SelfIllumColor.SaveBIN(OutFile);
    MtlAmbientColor.SaveBIN(OutFile);
    MtlDiffuseColor.SaveBIN(OutFile);
    MtlSpecularColor.SaveBIN(OutFile);
    MtlFilterColor.SaveBIN(OutFile);
}

#define SaveXML_MemI(n) fprintf(OutFile,"%s<ATTRIBUTE %s='%i'/>\n", Indent.c_str(), #n, m.n);
#define SaveXML_MemF(n) fprintf(OutFile,"%s<ATTRIBUTE %s='%f'/>\n", Indent.c_str(), #n, m.n);

void CD3F_Material::SaveXML(FILE *OutFile, CMString &Indent) {
    CMString pInt = Indent;
    Indent = Indent + "  ";
    fprintf(OutFile,"%s<D3F_Material ID='%i' NAME='%s'>\n", pInt.c_str(), ID, Name.c_str());

    // Textures
        SaveXML_MemI(AmbientMapID);				//!< Ambient texture map (seems not to exist anymore in MAX 3)
        SaveXML_MemI(DiffuseMapID);				//!< Diffuse texture map
        SaveXML_MemI(SpecularMapID);				//!< Specular texture map
        SaveXML_MemI(ShininessMapID);			//!< Shininess texture map
        SaveXML_MemI(ShiningStrengthMapID);		//!< Shining Strength texture map
        SaveXML_MemI(SelfIllumMapID);			//!< Self Illum texture map
        SaveXML_MemI(OpacityMapID);				//!< Opacity texture map
        SaveXML_MemI(FilterMapID);				//!< Filter texture map
        SaveXML_MemI(BumpMapID);					//!< Bump texture map
        SaveXML_MemI(ReflexionMapID);			//!< Reflexion texture map
        SaveXML_MemI(RefractionMapID);			//!< Refraction texture map
        SaveXML_MemI(DisplacementMapID);			//!< Displacement texture map
    // Amounts
        SaveXML_MemF(AmbientCoeff);				//!< Ambient texture %									[Animatable]
        SaveXML_MemF(DiffuseCoeff);				//!< Diffuse texture %									[Animatable]
        SaveXML_MemF(SpecularCoeff);				//!< Specular texture %									[Animatable]
        SaveXML_MemF(ShininessCoeff);			//!< Shininess texture %								[Animatable]
        SaveXML_MemF(ShiningStrengthCoeff);		//!< Shining Strength texture %							[Animatable]
        SaveXML_MemF(SelfIllumCoeff);			//!< Self Illum texture %								[Animatable]
        SaveXML_MemF(OpacityCoeff);				//!< Opacity texture %									[Animatable]
        SaveXML_MemF(FilterCoeff);				//!< Filter texture %									[Animatable]
        SaveXML_MemF(BumpCoeff);					//!< Bump texture %										[Animatable]
        SaveXML_MemF(ReflexionCoeff);			//!< Reflexion texture %								[Animatable]
        SaveXML_MemF(RefractionCoeff);			//!< Refraction texture %								[Animatable]
        SaveXML_MemF(DisplacementCoeff);			//!< Displacement texture %								[Animatable]
    // Static properties
        SaveXML_MemI(Shading);					//!< Material Shading
        SaveXML_MemI(Soften);					//!< MaterialSoften
        SaveXML_MemI(FaceMap);					//!< MaterialFaceMap
        SaveXML_MemI(TwoSided);					//!< MaterialTwoSided
        SaveXML_MemI(Wire);						//!< MaterialWire
        SaveXML_MemI(WireUnits);					//!< MaterialWireUnits
        SaveXML_MemI(FalloffOut);				//!< MaterialFalloffOut
        SaveXML_MemI(TransparencyType);			//!< MaterialTransparencyType
    // Dynamic properties
        SaveXML_MemF(Shininess);					//!< MaterialShininess									[Animatable]
        SaveXML_MemF(ShiningStrength);			//!< MaterialShiningStrength							[Animatable]
        SaveXML_MemF(SelfIllum);					//!< MaterialSelfIllum									[Animatable]
        SaveXML_MemF(Opacity);					//!< MaterialOpacity									[Animatable]
        SaveXML_MemF(OpaFalloff);				//!< MaterialOpacityFalloff								[Animatable]
        SaveXML_MemF(WireSize);					//!< MaterialWireSize									[Animatable]
        SaveXML_MemF(IOR);						//!< MaterialIOR										[Animatable]
    // Dynamic properties
        SaveXML_MemF(Bounce);					//!< Bounce												[Animatable]
        SaveXML_MemF(StaticFriction);			//!< Static Friction									[Animatable]
        SaveXML_MemF(SlidingFriction);			//!< Sliding Friction									[Animatable]
    // 1.15: enhanced self-illum
        SaveXML_MemI(SelfIllumOn);				//!< Is self-illum on ?
        SaveXML_MemF(SelfIllumValue);			//!< Self-illum value

    CMString tmp = "SelfIllumColor";
    SelfIllumColor.SaveXML(OutFile, Indent, tmp);
    tmp = "MtlAmbientColor";
    MtlAmbientColor.SaveXML(OutFile, Indent, tmp);
    tmp = "MtlDiffuseColor";
    MtlDiffuseColor.SaveXML(OutFile, Indent, tmp);
    tmp = "MtlSpecularColor";
    MtlSpecularColor.SaveXML(OutFile, Indent, tmp);
    tmp = "MtlFilterColor";
    MtlFilterColor.SaveXML(OutFile, Indent, tmp);
    fprintf(OutFile,"%s</D3F_Material>\n",pInt.c_str());
    Indent = pInt;
}
