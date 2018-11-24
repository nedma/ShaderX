#include "stdafx.h"
#include "d3format\d3f_mesh.h"
#include "SC-DynamicArray.h"

CD3F_Mesh::CD3F_Mesh(char *ObjName,CEngine *OwnerEngine) : CBaseClass(ObjName,OwnerEngine) {
    Vertexes = new CRootClassArray(true);
    Faces = new CRootClassArray(true); 
    SubMeshes = new CRootClassArray(true);
}

CD3F_Mesh::~CD3F_Mesh(void) {
    delete SubMeshes;
    delete Faces;
    delete Vertexes;
}

void CD3F_Mesh::LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition) {
    SD3F_Mesh s;
    memcpy(&s,&Buffer[CurrPosition],sizeof(s));
    CurrPosition+=sizeof(s);
    CMString TAG = s.TAG;
    if (strcmp(TAG.c_str(),"Dimension3 Mesh by Em/Eastwood 2002")!=0) {
        _WARN(NULL,"CD3F_Mesh",ErrMgr->TFile,"Bad tag at %u found '%s' expecting 'Dimension3 Mesh by Em/Eastwood 2002' (DataSize is %u)",CurrPosition,TAG.c_str(),sizeof(s));
    }
    Name = s.Name;
    BSphereRadius = s.BSphereRadius;
    BSphereCenter.LoadBIN(Buffer, Length, CurrPosition);
    for (DWORD vI=0;vI<s.VertexCount;vI++) {
        CD3F_Vertex *Vert = new CD3F_Vertex;
        Vert->LoadBIN(Buffer, Length, CurrPosition);
        Vertexes->Add(Vert);
    }
    for (DWORD vF=0;vF<s.FaceCount;vF++) {
        CD3F_Face *Face = new CD3F_Face;
        Face->LoadBIN(Buffer, Length, CurrPosition);
        Faces->Add(Face);
    }
    for (DWORD vS=0;vS<s.SubCount;vS++) {
        CD3F_Submesh *SubMesh = new CD3F_Submesh;
        SubMesh->LoadBIN(Buffer, Length, CurrPosition);
        SubMeshes->Add(SubMesh);
    }
}

void CD3F_Mesh::SaveBIN(FILE *OutFile) {
    SD3F_Mesh s;
    memset(s.TAG,' ',sizeof(s.TAG));
    strcpy(s.TAG,"Dimension3 Mesh by Em/Eastwood 2002");
    s.Version = 1;
    memset(s.Name,' ',sizeof(s.Name));
    strcpy(s.Name,Name.c_str());
    s.FaceCount = Faces->GetSize();
    s.SubCount = SubMeshes->GetSize();
    s.VertexCount = Vertexes->GetSize();
    s.BSphereRadius = BSphereRadius;
    fwrite(&s,sizeof(s),1,OutFile);
    BSphereCenter.SaveBIN(OutFile);
    for (int vI=0;vI<this->Vertexes->GetSize();vI++) {
        CD3F_Vertex *Vert = (CD3F_Vertex *)this->Vertexes->GetAt(vI);
        Vert->SaveBIN(OutFile);
    }
    for (int vF=0;vF<this->Faces->GetSize();vF++) {
        CD3F_Face *Face = (CD3F_Face *)this->Faces->GetAt(vF);
        Face->SaveBIN(OutFile);
    }
    for (int vS=0;vS<this->SubMeshes->GetSize();vS++) {
        CD3F_Submesh *SubMesh = (CD3F_Submesh *)this->SubMeshes->GetAt(vS);
        SubMesh->SaveBIN(OutFile);
    }
}

void CD3F_Mesh::SaveXML(FILE *OutFile, CMString &Indent) {
    CMString pInt = Indent;
    Indent = Indent + "    ";
    fprintf(OutFile,"%s<D3F_MESH NAME='%s'>\n",pInt.c_str(),this->Name.c_str());
    fprintf(OutFile,"%s  <BOUNDSPHERE RADIUS='%f'>\n",pInt.c_str(),this->BSphereRadius);
    CMString tmp = "CENTER";
    this->BSphereCenter.SaveXML(OutFile, Indent, tmp);
    fprintf(OutFile,"%s  </BOUNDSPHERE>\n",pInt.c_str());
    fprintf(OutFile,"%s  <VERTEXES COUNT='%u'>\n",pInt.c_str(),this->Vertexes->GetSize());
    for (int vI=0;vI<this->Vertexes->GetSize();vI++) {
        CD3F_Vertex *Vert = (CD3F_Vertex *)this->Vertexes->GetAt(vI);
        Vert->SaveXML(OutFile, Indent);
    }
    fprintf(OutFile,"%s  </VERTEXES>\n",pInt.c_str());
    fprintf(OutFile,"%s  <FACES COUNT='%u'>\n",pInt.c_str(),this->Faces->GetSize());
    for (int vF=0;vF<this->Faces->GetSize();vF++) {
        CD3F_Face *Face = (CD3F_Face *)this->Faces->GetAt(vF);
        Face->SaveXML(OutFile, Indent);
    }
    fprintf(OutFile,"%s  </FACES>\n",pInt.c_str());
    fprintf(OutFile,"%s  <SUBMESHES COUNT='%u'>\n",pInt.c_str(),this->SubMeshes->GetSize());
    for (int vS=0;vS<this->SubMeshes->GetSize();vS++) {
        CD3F_Submesh *SubMesh = (CD3F_Submesh *)this->SubMeshes->GetAt(vS);
        SubMesh->SaveXML(OutFile, Indent);
    }
    fprintf(OutFile,"%s  </SUBMESHES>\n",pInt.c_str());
    fprintf(OutFile,"%s</D3F_MESH>\n",pInt.c_str());
    Indent = pInt;
}

void CD3F_Submesh::LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition) {
    SD3F_Submesh s;
    memcpy(&s,&Buffer[CurrPosition],sizeof(s));
    CurrPosition+=sizeof(s);
    if (s.TAG!='MBUS') {
        char tmp[5];
        memcpy(tmp,&s.TAG,4);
        tmp[4] = 0;
        _WARN(NULL,"CD3F_Submesh",ErrMgr->TFile,"Bad tag at %u found '%s' expecting 'MBUS' (DataSize is %u)",CurrPosition,tmp,sizeof(s));
    }
    MaxVertIndex = s.MaxVertIndex;
    MinVertIndex = s.MinVertIndex;
    StartFaceIndex = s.StartFaceIndex;
    FaceCount = s.FaceCount;
    MaterialID = s.MaterialID;
}

void CD3F_Submesh::SaveBIN(FILE *OutFile) {
    SD3F_Submesh s;
    s.TAG = 'MBUS';
    s.MaxVertIndex = MaxVertIndex;
    s.MinVertIndex = MinVertIndex;
    s.StartFaceIndex = StartFaceIndex;
    s.FaceCount = FaceCount;
    s.MaterialID = MaterialID;
    fwrite(&s,sizeof(s),1,OutFile);
}

void CD3F_Submesh::SaveXML(FILE *OutFile, CMString &Indent) {
    CMString pInt = Indent;
    Indent = Indent + "  ";
    fprintf(OutFile,"%s<D3F_SUBMESH INDEX='%u' MINVERTEXINDEX='%u' MAXVERTEXINDEX='%u' STARTFACEINDEX='%u' FACECOUNT='%u' MATERIALID='%i'/>\n",pInt.c_str(),
        this->Index,this->MinVertIndex, this->MaxVertIndex, this->StartFaceIndex, this->FaceCount, this->MaterialID);
    Indent = pInt;
}

void CD3F_Face::LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition) {
    SD3F_Face s;
    memcpy(&s,&Buffer[CurrPosition],sizeof(s));
    CurrPosition+=sizeof(s);
    if (s.TAG!='ECAF') {
        char tmp[5];
        memcpy(tmp,&s.TAG,4);
        tmp[4] = 0;
        _WARN(NULL,"CD3F_Face",ErrMgr->TFile,"Bad tag at %u found '%s' expecting 'FACE' (DataSize is %u)",CurrPosition,tmp,sizeof(s));
    }
    this->VertexIndex[0] = s.VertexIndex[0];
    this->VertexIndex[1] = s.VertexIndex[1];
    this->VertexIndex[2] = s.VertexIndex[2];
}

void CD3F_Face::SaveBIN(FILE *OutFile) {
    SD3F_Face s;
    s.TAG = 'ECAF';
    s.VertexIndex[0] = (DWORD)VertexIndex[0];
    s.VertexIndex[1] = (DWORD)VertexIndex[1];
    s.VertexIndex[2] = (DWORD)VertexIndex[2];
    fwrite(&s,sizeof(s),1,OutFile);
}

void CD3F_Face::SaveXML(FILE *OutFile, CMString &Indent) {
    CMString pInt = Indent;
    Indent = Indent + "  ";
    fprintf(OutFile,"%s<D3F_FACE INDEX='%u' VERTEXINDEX1='%I64u' VERTEXINDEX2='%I64u' VERTEXINDEX3='%I64u'/>\n",pInt.c_str(),
        this->Index,this->VertexIndex[0], this->VertexIndex[1], this->VertexIndex[2]);
    Indent = pInt;
}

void CD3F_Vertex::LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition) {
    SD3F_Vertex s;
    memcpy(&s,&Buffer[CurrPosition],sizeof(s));
    CurrPosition+=sizeof(s);
    if (s.TAG!='TREV') {
        char tmp[5];
        memcpy(tmp,&s.TAG,4);
        tmp[4] = 0;
        _WARN(NULL,"CD3F_Vertex",ErrMgr->TFile,"Bad tag at %u found '%s' expecting 'VERT' (DataSize is %u)",CurrPosition,tmp,sizeof(s));
    }
    this->Pos.LoadBIN(Buffer, Length, CurrPosition);
    this->Norm.LoadBIN(Buffer, Length, CurrPosition);
    this->Map.LoadBIN(Buffer, Length, CurrPosition);
}

void CD3F_Vertex::SaveBIN(FILE *OutFile) {
    SD3F_Vertex s;
    s.TAG = 'TREV';
    fwrite(&s,sizeof(s),1,OutFile);
    this->Pos.SaveBIN(OutFile);
    this->Norm.SaveBIN(OutFile);
    this->Map.SaveBIN(OutFile);
}

void CD3F_Vertex::SaveXML(FILE *OutFile, CMString &Indent) {
    CMString pInt = Indent;
    Indent = Indent + "  ";
    fprintf(OutFile,"%s<D3F_VERTEX INDEX='%u'>\n",pInt.c_str(),this->Index);
    CMString tmp1("POS");
    this->Pos.SaveXML(OutFile, Indent,tmp1);
    CMString tmp2("NORMAL");
    this->Norm.SaveXML(OutFile, Indent,tmp2);
    CMString tmp3("MAPPINGCOORD");
    this->Map.SaveXML(OutFile, Indent,tmp3);
    fprintf(OutFile,"%s</D3F_VERTEX>\n",pInt.c_str());
    Indent = pInt;
}

void CD3F_MapCoord::LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition) {
    SD3F_MapCoord s;
    memcpy(&s,&Buffer[CurrPosition],sizeof(s));
    CurrPosition+=sizeof(s);
    if (s.TAG!='ROCM') {
        char tmp[5];
        memcpy(tmp,&s.TAG,4);
        tmp[4] = 0;
        _WARN(NULL,"CD3F_MapCoord",ErrMgr->TFile,"Bad tag at %u found '%s' expecting 'MCOR' (DataSize is %u)",CurrPosition,tmp,sizeof(s));
    }
    this->IsSet = (s.IsSet!=0) ? true : false;
    this->u = s.u;
    this->v = s.v;
}

void CD3F_MapCoord::SaveBIN(FILE *OutFile) {
    SD3F_MapCoord s;
    s.TAG = 'ROCM';
    s.IsSet = IsSet;
    s.u = u;
    s.v = v;
    fwrite(&s,sizeof(s),1,OutFile);
}

void CD3F_MapCoord::SaveXML(FILE *OutFile, CMString &Indent, CMString &TagName) {
    CMString pInt = Indent;
    Indent = Indent + "  ";
    fprintf(OutFile,"%s<%s ISSET='%u' COORDU='%f' COORDV='%f'/>\n",pInt.c_str(),TagName.c_str(),
        this->IsSet,this->u, this->v);
    Indent = pInt;
}

void CD3F_Coord::LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition) {
    SD3F_Coord s;
    memcpy(&s,&Buffer[CurrPosition],sizeof(s));
    CurrPosition+=sizeof(s);
    if (s.TAG!='ROOC') {
        char tmp[5];
        memcpy(tmp,&s.TAG,4);
        tmp[4] = 0;
        _WARN(NULL,"CD3F_Coord",ErrMgr->TFile,"Bad tag at %u found '%s' expecting 'COOR' (DataSize is %u)",CurrPosition,tmp,sizeof(s));
    }
    this->IsSet = (s.IsSet!=0) ? true : false;
    this->x = s.x;
    this->y = s.y;
    this->z = s.z;
}

void CD3F_Coord::SaveBIN(FILE *OutFile) {
    SD3F_Coord s;
    s.TAG = 'ROOC';
    s.IsSet = IsSet;
    s.x = x;
    s.y = y;
    s.z = z;
    fwrite(&s,sizeof(s),1,OutFile);
}

void CD3F_Coord::SaveXML(FILE *OutFile, CMString &Indent, CMString &TagName) {
    CMString pInt = Indent;
    Indent = Indent + "  ";
    fprintf(OutFile,"%s<%s ISSET='%u' COORDX='%f' COORDY='%f' COORDZ='%f'/>\n",pInt.c_str(),TagName.c_str(),
        this->IsSet,this->x, this->y, this->z);
    Indent = pInt;
}