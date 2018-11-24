#include "stdafx.h"
#include "d3format\d3f_Texture.h"

CD3F_Texture::CD3F_Texture(char *ObjName,CEngine *OwnerEngine) : CBaseClass(ObjName,OwnerEngine) {
}

CD3F_Texture::~CD3F_Texture(void) {
}

void CD3F_Texture::LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition) {
    SD3F_Texture s;
    memcpy(&s,&Buffer[CurrPosition],sizeof(s));
    CurrPosition+=sizeof(s);
    if (s.TAG!='RTXT') {
        char tmp[5];
        memcpy(tmp,&s.TAG,4);
        tmp[4] = 0;
        _WARN(NULL,"CD3F_Texture",ErrMgr->TFile,"Bad tag at %u found '%s' expecting 'TXTR' (DataSize is %u)",CurrPosition,tmp,sizeof(s));
    }
    FileName = s.FileName;
    ID = s.ID;
}

void CD3F_Texture::SaveBIN(FILE *OutFile) {
    SD3F_Texture s;
    s.TAG = 'RTXT';
    s.ID = ID;
    memset(s.FileName,' ',sizeof(s.FileName));
    strcpy(s.FileName,FileName.c_str());
    fwrite(&s,sizeof(s),1,OutFile);
}

void CD3F_Texture::SaveXML(FILE *OutFile, CMString &Indent) {
    CMString pInt = Indent;
    Indent = Indent + "  ";
    fprintf(OutFile,"%s<D3F_Texture ID='%i' FILENAME='%s'/>\n", pInt.c_str(),
        ID, FileName.c_str());
    Indent = pInt;
}
