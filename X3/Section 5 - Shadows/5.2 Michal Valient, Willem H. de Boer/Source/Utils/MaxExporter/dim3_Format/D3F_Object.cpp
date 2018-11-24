#include "stdafx.h"
#include "d3format\d3f_object.h"

CD3F_Object::CD3F_Object(char *ObjName,CEngine *OwnerEngine) : CBaseClass(ObjName,OwnerEngine) {
    ParentID = Index = ID = MeshID = -1;
    MasterID = -1;
    IsInstance = false;
}

CD3F_Object::~CD3F_Object(void) {
}

void CD3F_Object::LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition) {
    SD3F_Object s;
    memcpy(&s,&Buffer[CurrPosition],sizeof(s));
    CurrPosition+=sizeof(s);
    if (s.TAG!='_JBO') {
        char tmp[5];
        memcpy(tmp,&s.TAG,4);
        tmp[4] = 0;
        _WARN(NULL,"CD3F_Object",ErrMgr->TFile,"Bad tag at %u found '%s' expecting 'OBJ_' (DataSize is %u)",CurrPosition,tmp,sizeof(s));
    }
    Name = s.Name;
    ID = s.ID;
    IsInstance = (s.IsInstance!=0) ? true : false;
    IsTarget = (s.IsTarget!=0) ? true : false;
    MasterID = s.MasterID;
    MeshID = s.MeshID;
    ParentID = s.ParentID;
    LocalPivot.LoadBIN(Buffer,Length,CurrPosition);
    WorldPivot.LoadBIN(Buffer,Length,CurrPosition);
}

void CD3F_Object::SaveBIN(FILE *OutFile) {
    SD3F_Object s;
    s.TAG = '_JBO';
    memset(s.Name,' ',sizeof(s.Name));
    strcpy(s.Name,Name.c_str());
    s.ID = ID;
    s.IsInstance = IsInstance;
    s.IsTarget = IsTarget;
    s.MasterID = MasterID;
    s.MeshID = MeshID;
    s.ParentID = ParentID;
    fwrite(&s,sizeof(s),1,OutFile);
    LocalPivot.SaveBIN(OutFile);
    WorldPivot.SaveBIN(OutFile);
}

void CD3F_Object::SaveXML(FILE *OutFile, CMString &Indent) {
    CMString pInt = Indent;
    Indent = Indent + "  ";
    fprintf(OutFile,"%s<D3F_OBJECT NAME='%s' ID='%i' PARENTID='%i' ISINSTANCE='%i' MASTERID='%i' MESHID='%i' ISTARGET='%i'>\n",
        pInt.c_str(),Name.c_str(),ID, ParentID,IsInstance,MasterID,MeshID,IsTarget);
    CMString tmp1("LOCALPIVOT");
    LocalPivot.SaveXML(OutFile,Indent,tmp1);
    CMString tmp2("WORLDPIVOT");
    WorldPivot.SaveXML(OutFile,Indent,tmp2);
    fprintf(OutFile,"%s</D3F_OBJECT>\n",pInt.c_str());
    Indent = pInt;
}

void CD3F_PosRotScale::LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition) {
    SD3F_PosRotScale s;
    memcpy(&s,&Buffer[CurrPosition],sizeof(s));
    CurrPosition+=sizeof(s);
    if (s.TAG!='CSRP') {
        char tmp[5];
        memcpy(tmp,&s.TAG,4);
        tmp[4] = 0;
        _WARN(NULL,"CD3F_PosRotScale",ErrMgr->TFile,"Bad tag at %u found '%s' expecting 'PRSC' (DataSize is %u)",CurrPosition,tmp,sizeof(s));
    }
    this->Pos.LoadBIN(Buffer,Length,CurrPosition);
    this->Rot.LoadBIN(Buffer,Length,CurrPosition);
    this->Scale.LoadBIN(Buffer,Length,CurrPosition);
}

void CD3F_PosRotScale::SaveBIN(FILE *OutFile) {
    SD3F_PosRotScale s;
    s.TAG = 'CSRP';
    fwrite(&s,sizeof(s),1,OutFile);
    this->Pos.SaveBIN(OutFile);
    this->Rot.SaveBIN(OutFile);
    this->Scale.SaveBIN(OutFile);
}

void CD3F_PosRotScale::SaveXML(FILE *OutFile, CMString &Indent, CMString &TagName) {
    CMString pInt = Indent;
    Indent = Indent + "  ";
    fprintf(OutFile,"%s<%s>\n",pInt.c_str(),TagName.c_str());
    CMString tmp("POSITION");
    this->Pos.SaveXML(OutFile, Indent,tmp);
    tmp = "ROTATION";
    this->Rot.SaveXML(OutFile, Indent,tmp);
    tmp = "SCALE";
    this->Scale.SaveXML(OutFile, Indent,tmp);
    fprintf(OutFile,"%s</%s>\n",pInt.c_str(),TagName.c_str());
    Indent = pInt;
}

void CD3F_Quat::LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition) {
    SD3F_Quat s;
    memcpy(&s,&Buffer[CurrPosition],sizeof(s));
    CurrPosition+=sizeof(s);
    if (s.TAG!='TAUQ') {
        char tmp[5];
        memcpy(tmp,&s.TAG,4);
        tmp[4] = 0;
        _WARN(NULL,"CD3F_Quat",ErrMgr->TFile,"Bad tag at %u found '%s' expecting 'QUAT' (DataSize is %u)",CurrPosition,tmp,sizeof(s));
    }
    this->isSet = (s.IsSet!=0) ? true : false;
    this->x = s.x;
    this->y = s.y;
    this->z = s.z;
    this->w = s.w;
}

void CD3F_Quat::SaveBIN(FILE *OutFile) {
    SD3F_Quat s;
    s.TAG = 'TAUQ';
    s.IsSet = isSet;
    s.x = x;
    s.y = y;
    s.z = z;
    s.w = w;
    fwrite(&s,sizeof(s),1,OutFile);
}

void CD3F_Quat::SaveXML(FILE *OutFile, CMString &Indent, CMString &TagName) {
    CMString pInt = Indent;
    Indent = Indent + "  ";
    fprintf(OutFile,"%s<%s ISSET='%u' X='%f' Y='%f' Z='%f' W='%f'/>\n",pInt.c_str(),TagName.c_str(),
        this->isSet,this->x, this->y, this->z, this->w);
    Indent = pInt;
}

