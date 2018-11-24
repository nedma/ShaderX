#include "stdafx.h"
#include "d3format\d3f_camera.h"

CD3F_Camera::CD3F_Camera(char *ObjName,CEngine *OwnerEngine) : CD3F_Object(ObjName,OwnerEngine) {
}

CD3F_Camera::~CD3F_Camera(void) {
}

void CD3F_Camera::LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition) {
    SD3F_Camera s;
    memcpy(&s,&Buffer[CurrPosition],sizeof(s));
    CurrPosition+=sizeof(s);
    if (s.TAG!='_MAC') {
        char tmp[5];
        memcpy(tmp,&s.TAG,4);
        tmp[4] = 0;
        _WARN(NULL,"CD3F_Camera",ErrMgr->TFile,"Bad tag at %u found '%s' expecting 'CAM_' (DataSize is %u)",CurrPosition,tmp,sizeof(s));
    }
    FarCLip = s.FarCLip;
    FOV = s.FOV;
    NearClip = s.NearClip;
    Ortho = (s.Ortho!=0) ? true : false;
    TargetID = s.TargetID;
    CD3F_Object::LoadBIN(Buffer,Length,CurrPosition);
}

void CD3F_Camera::SaveBIN(FILE *OutFile) {
    SD3F_Camera s;
    s.TAG = '_MAC';
    s.FarCLip = FarCLip;
    s.FOV = FOV;
    s.NearClip = NearClip;
    s.Ortho = Ortho;
    s.TargetID = TargetID;
    fwrite(&s,sizeof(s),1,OutFile);
    CD3F_Object::SaveBIN(OutFile);
}

void CD3F_Camera::SaveXML(FILE *OutFile, CMString &Indent) {
    CMString pInt = Indent;
    Indent = Indent + "  ";
    fprintf(OutFile,"%s<D3F_CAMERA ORTHO='%u' FOV='%f' NEARCLIP='%f' FARCLIP='%f' TARGETID='%i'>\n", pInt.c_str(),
                Ortho, FOV, NearClip, FarCLip, TargetID);
    CD3F_Object::SaveXML(OutFile, Indent);
    fprintf(OutFile,"%s</D3F_CAMERA>\n",pInt.c_str());
    Indent = pInt;
}
