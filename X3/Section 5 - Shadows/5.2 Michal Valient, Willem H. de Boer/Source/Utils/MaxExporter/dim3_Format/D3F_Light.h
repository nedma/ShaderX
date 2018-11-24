#pragma once
#include "D3Format\d3f_Object.h"

class CD3F_Light : public CD3F_Object {
    typedef struct {
        DWORD   TAG;
        int             Type;       //convert from Flex types
        double          Hotspot;
        double          Intensity;
        double          Contrast;
        bool            UseAttenuationNear;
        bool            UseAttenuationFar;
        double          AttenuationNearStart;
        double          AttenuationNearEnd;
        double          AttenuationFarStart;
        double          AttenuationFarEnd;
        int             DecayType;  //convert from Flex types
        int             TargetID;
    } SD3F_Light;
public:
    static const int TTypeOmni = 1;
    static const int TTypeSpot = 2;
    static const int TTypeDirect = 3;

    static const int TDecayNone = 1;
    static const int TDecayInverse = 2;
    static const int TDecayInverseSquare = 3;

    int             Type;       //convert from Flex types
    double          Hotspot;
    double          Intensity;
    double          Contrast;
    bool            UseAttenuationNear;
    bool            UseAttenuationFar;
    double          AttenuationNearStart;
    double          AttenuationNearEnd;
    double          AttenuationFarStart;
    double          AttenuationFarEnd;
    int             DecayType;  //convert from Flex types
    int             TargetID;

    CD3F_Coord      LightColor;

    CD3F_Light(char *ObjName,CEngine *OwnerEngine);
    virtual ~CD3F_Light(void);

    virtual void SaveXML(FILE *OutFile, CMString &Indent);
    virtual void SaveBIN(FILE *OutFile);
    
    virtual void LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition);

    MAKE_DEFAULTGARBAGE();
    MAKE_CLSNAME("CD3F_Light");
    MAKE_CLSCREATE(CD3F_Light);
};
