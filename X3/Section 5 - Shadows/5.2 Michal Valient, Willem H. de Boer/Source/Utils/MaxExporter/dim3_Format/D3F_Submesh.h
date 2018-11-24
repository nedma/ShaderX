#pragma once
#include "baseclass.h"

class CD3F_Submesh : public CBaseClass {
public:
    DWORD       MinVertIndex;
    DWORD       MaxVertIndex;
    DWORD       StartFaceIndex;
    DWORD       FaceCount;

    CD3F_Submesh(char *ObjName,CEngine *OwnerEngine);
    virtual ~CD3F_Submesh(void);
};
