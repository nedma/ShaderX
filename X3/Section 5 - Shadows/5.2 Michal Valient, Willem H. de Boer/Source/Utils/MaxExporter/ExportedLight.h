#pragma once
#include "exportedobject.h"

class CExportedLight : public CExportedObject {
public:
	CString             LightType;
	float               Color[3];
	float               Intensity;
	float               Contrast;
	float               DiffuseSoft;
	int                 LightUsed;
	int                 AffectDiffuse;
	int                 AffectSpecular;
	int                 UseAttenNear;
	int                 UseAttenFar;
	float               NearAttenStart;
	float               NearAttenEnd;
	float               FarAttenStart;
	float               FarAttenEnd;
	CString             DecayType;
	float               HotSpot;
	float               Falloff;
	float               Aspect;
	CString             SpotShape;
	int                 ShadowType;
	int                 AbsMapBias;
	float               RayBias;
	float               MapBias;
	float               MapRange;
	int                 MapSize;
	int                 CastShadows;
	float               ShadowColor[3];
	int                 LightAffectsShadow;
	float               ShadowDensity;
	int                 ProjMapID;
	int                 ShadowProjMapID;

    CExportedLight(void);
    virtual ~CExportedLight(void);

    virtual void CreateFromFlexporter(const LightDescriptor& Obj);

    virtual void SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent);

    //Parts of XML export - save inner nodes (subnodes) and tag text
    virtual void SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent);
};
