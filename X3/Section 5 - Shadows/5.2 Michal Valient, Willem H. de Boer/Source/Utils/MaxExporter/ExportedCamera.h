#pragma once
#include "exportedobject.h"

class CExportedCamera : public CExportedObject {
public:
    int     OrthoCam;
    float   FOV;
    float   NearClip;
    float   FarClip;
    float   EnvNearRange;
    float   EnvFarRange;
    int     ManualClip;
    CString FOVType;
    CString CamType;

    CExportedCamera(void);
    virtual ~CExportedCamera(void);

    virtual void CreateFromFlexporter(const CameraDescriptor& Obj);

    virtual void SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent);
};
