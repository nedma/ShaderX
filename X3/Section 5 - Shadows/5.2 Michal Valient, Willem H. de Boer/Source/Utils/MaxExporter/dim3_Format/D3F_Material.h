#pragma once
#include "d3f_object.h"
#include "SC-MString.h"

#define SetM(Dest, Src, Att) {Dest->m.Att = Src.m##Att;}
#define SetCoord(Dest, Src, Att) {Dest->Att.Set(Src.m##Att.r,Src.m##Att.g,Src.m##Att.b);}

class CD3F_Material : public CBaseClass {
    typedef struct {
        DWORD       TAG;
        int         ID;
        char        Name[512];
    // Textures
        int         AmbientMapID;				//!< Ambient texture map (seems not to exist anymore in MAX 3)
        int         DiffuseMapID;				//!< Diffuse texture map
        int         SpecularMapID;				//!< Specular texture map
        int         ShininessMapID;			//!< Shininess texture map
        int         ShiningStrengthMapID;		//!< Shining Strength texture map
        int         SelfIllumMapID;			//!< Self Illum texture map
        int         OpacityMapID;				//!< Opacity texture map
        int         FilterMapID;				//!< Filter texture map
        int         BumpMapID;					//!< Bump texture map
        int         ReflexionMapID;			//!< Reflexion texture map
        int         RefractionMapID;			//!< Refraction texture map
        int         DisplacementMapID;			//!< Displacement texture map
    // Amounts
        float       AmbientCoeff;				//!< Ambient texture %									[Animatable]
        float       DiffuseCoeff;				//!< Diffuse texture %									[Animatable]
        float       SpecularCoeff;				//!< Specular texture %									[Animatable]
        float       ShininessCoeff;			//!< Shininess texture %								[Animatable]
        float       ShiningStrengthCoeff;		//!< Shining Strength texture %							[Animatable]
        float       SelfIllumCoeff;			//!< Self Illum texture %								[Animatable]
        float       OpacityCoeff;				//!< Opacity texture %									[Animatable]
        float       FilterCoeff;				//!< Filter texture %									[Animatable]
        float       BumpCoeff;					//!< Bump texture %										[Animatable]
        float       ReflexionCoeff;			//!< Reflexion texture %								[Animatable]
        float       RefractionCoeff;			//!< Refraction texture %								[Animatable]
        float       DisplacementCoeff;			//!< Displacement texture %								[Animatable]
    // Static properties
        long        Shading;					//!< Material Shading
        UINT        Soften;					//!< MaterialSoften
        UINT        FaceMap;					//!< MaterialFaceMap
        UINT        TwoSided;					//!< MaterialTwoSided
        UINT        Wire;						//!< MaterialWire
        UINT        WireUnits;					//!< MaterialWireUnits
        UINT        FalloffOut;				//!< MaterialFalloffOut
        long        TransparencyType;			//!< MaterialTransparencyType
    // Dynamic properties
        float       Shininess;					//!< MaterialShininess									[Animatable]
        float       ShiningStrength;			//!< MaterialShiningStrength							[Animatable]
        float       SelfIllum;					//!< MaterialSelfIllum									[Animatable]
        float       Opacity;					//!< MaterialOpacity									[Animatable]
        float       OpaFalloff;				//!< MaterialOpacityFalloff								[Animatable]
        float       WireSize;					//!< MaterialWireSize									[Animatable]
        float       IOR;						//!< MaterialIOR										[Animatable]
    // Dynamic properties
        float       Bounce;					//!< Bounce												[Animatable]
        float       StaticFriction;			//!< Static Friction									[Animatable]
        float       SlidingFriction;			//!< Sliding Friction									[Animatable]
    // 1.15: enhanced self-illum
        UINT        SelfIllumOn;				//!< Is self-illum on ?
        float       SelfIllumValue;			//!< Self-illum value
    } SD3F_Material;
public:
    int         ID;
    CMString    Name;
    SD3F_Material m;                    //properties

// Colors
    CD3F_Coord  SelfIllumColor;			//!< Self-illum color
    CD3F_Coord  MtlAmbientColor;			//!< Ambient color										[Animatable]
    CD3F_Coord  MtlDiffuseColor;			//!< Diffuse color										[Animatable]
    CD3F_Coord  MtlSpecularColor;			//!< Specular color										[Animatable]
    CD3F_Coord  MtlFilterColor;			//!< Filter color										[Animatable]

    CD3F_Material(char *ObjName,CEngine *OwnerEngine);
    virtual ~CD3F_Material(void);

    virtual void SaveXML(FILE *OutFile, CMString &Indent);
    virtual void SaveBIN(FILE *OutFile);

    virtual void LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition);

    MAKE_DEFAULTGARBAGE();
    MAKE_CLSNAME("CD3F_Material");
    MAKE_CLSCREATE(CD3F_Material);
};
