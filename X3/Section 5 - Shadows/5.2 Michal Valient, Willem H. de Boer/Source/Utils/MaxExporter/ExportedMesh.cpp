#include "stdafx.h"
#include "exportedmesh.h"

void Log(LPCTSTR text) {
    FILE *f = fopen("C:\\ExportMesh,log", "a+t");
    fprintf(f, "%s\n", text);
    fflush(f);
    fclose(f);
}


CExportedMesh::CExportedMesh(void) {
    memset(&Header,0,sizeof(Header));
    Name = NULL;
    Vertex = NULL;
    Face = NULL;
    SubMesh = NULL;
}

CExportedMesh::~CExportedMesh(void) {
    if (Name!=NULL) free(Name);
    if (Vertex!=NULL) free(Vertex);
    if (Face!=NULL) free(Face);
    if (SubMesh!=NULL) free(SubMesh);
}

bool CExportedMesh::CreateFromFlexporter(const MeshDescriptor& mesh) {
    //clean
    memset(&Header,0,sizeof(Header));
    if (Name!=NULL) free(Name);
    if (Vertex!=NULL) free(Vertex);
    if (Face!=NULL) free(Face);
    if (SubMesh!=NULL) free(SubMesh);
    Name = NULL;
    Vertex = NULL;
    Face = NULL;
    SubMesh = NULL;

    Header.NameLength = strlen((char *)mesh.mName) + 1;
    Name = (char *)malloc(Header.NameLength);
    memcpy(Name,(char *)mesh.mName, Header.NameLength);
    if (mesh.mCleanMesh==NULL) {
        return false;
    } else {
        //Bound sphere stuff
        Header.BSphereRadius = mesh.mOriginalMesh.mBSRadius;
        Header.BSphereCenter[0] = mesh.mOriginalMesh.mBSCenter.x;
        Header.BSphereCenter[1] = mesh.mOriginalMesh.mBSCenter.y;
        Header.BSphereCenter[2] = mesh.mOriginalMesh.mBSCenter.z;

        CPtrArray   *Vertexes = new CPtrArray();
        CPtrArray   *Faces = new CPtrArray();
        CPtrArray   *SubMeshes = new CPtrArray();

        Vertexes->SetSize(0, 256);
        Faces->SetSize(0, 256);

        //SubMeshes->SetSize(mesh.mCleanMesh->Topology.NbSubmeshes, 1);

        MBGeometry *Geom = &mesh.mCleanMesh->Geometry;
        DWORD *Vert = (DWORD *)mesh.mCleanMesh->Topology.VRefs;
        for (DWORD i=0; i<mesh.mCleanMesh->Topology.NbSubmeshes; i++) {
            MBSubmesh *subMesh = &mesh.mCleanMesh->Topology.SubmeshProperties[i];
            //Export submesh
            ExportedMeshSubmesh *ExpSub = new ExportedMeshSubmesh;
            memset(ExpSub, 0, sizeof(ExportedMeshSubmesh));
            ExpSub->MaterialID = subMesh->MatID;

            //export faces
            //Faces->SetSize(Faces->GetCount() + subMesh->NbFaces, 1);
            //Vertexes->SetSize(Vertexes->GetCount() + subMesh->NbFaces * 3, 3);
            for (DWORD j=0; j<subMesh->NbFaces; j++) {
                ExportedMeshFace *ExpFace = new ExportedMeshFace;
                memset(ExpFace, 0, sizeof(ExportedMeshFace));
                //export verticles
                for (DWORD k=0; k<3; k++) {
                    ExportedMeshVertex *ExpVert = new ExportedMeshVertex;
                    memset(ExpVert, 0, sizeof(ExportedMeshVertex));
                    if (mesh.mCleanMesh->Geometry.VertsRefs!=NULL) {
                        ExpVert->Position[0] = Geom->Verts[3*Geom->VertsRefs[Vert[k]]+0];
                        ExpVert->Position[1] = Geom->Verts[3*Geom->VertsRefs[Vert[k]]+1];
                        ExpVert->Position[2] = Geom->Verts[3*Geom->VertsRefs[Vert[k]]+2];
                    }
                    if (mesh.mCleanMesh->Geometry.Normals!=NULL) {
                        ExpVert->Normal[0] = Geom->Normals[3*Vert[k]+0];
                        ExpVert->Normal[1] = Geom->Normals[3*Vert[k]+1];
                        ExpVert->Normal[2] = Geom->Normals[3*Vert[k]+2];
                    }
                    if (mesh.mCleanMesh->Geometry.TVertsRefs!=NULL) {
                        if (mesh.mCleanMesh->Geometry.UseW) {
                            ExpVert->Map[0] = Geom->TVerts[3*Geom->TVertsRefs[Vert[k]]+0];
                            ExpVert->Map[1] = Geom->TVerts[3*Geom->TVertsRefs[Vert[k]]+1];
                        } else {
                            ExpVert->Map[0] = Geom->TVerts[2*Geom->TVertsRefs[Vert[k]]+0];
                            ExpVert->Map[1] = Geom->TVerts[2*Geom->TVertsRefs[Vert[k]]+1];
                        }
                    }
                    //Find the same vertex in list
                    int ExpVertIndex = -1;
                    for (int vI=0; vI<Vertexes->GetSize(); vI++) {
                        ExportedMeshVertex *TmpVert = (ExportedMeshVertex *)Vertexes->GetAt(vI);
                        if (memcmp(TmpVert, ExpVert, sizeof(ExportedMeshVertex)) == 0) {
                            ExpVertIndex = vI;
                            break;
                        }
                    }
                    if (ExpVertIndex>=0) {  //found stored verticle
                        delete ExpVert;
                    } else {    //not found stored verticle
                        ExpVertIndex = Vertexes->Add(ExpVert);
                    }
                    ExpFace->Vertex[k] = ExpVertIndex;
                    //Submesh indexing hints
                    if (ExpSub->MinVertIndex>(UINT)ExpVertIndex) ExpSub->MinVertIndex = (UINT)ExpVertIndex;
                    if (ExpSub->MaxVertIndex<(UINT)ExpVertIndex) ExpSub->MaxVertIndex = (UINT)ExpVertIndex;
                }
                int FaceIndex = Faces->Add(ExpFace);
                if (j==0) ExpSub->StartFaceIndex = FaceIndex;
                ExpSub->FaceCount++;
                Vert+=3;
            }
            SubMeshes->Add(ExpSub);
        }
        //Part2 - prepare buffers for final save
        Header.VertexCount = Vertexes->GetCount();
        Vertex = (ExportedMeshVertex *)malloc(sizeof(ExportedMeshVertex) * Vertexes->GetCount());
        for (int i=0; i<Vertexes->GetCount(); i++) {
            ExportedMeshVertex *tmp = (ExportedMeshVertex *)Vertexes->GetAt(i);
            memcpy(&Vertex[i], tmp, sizeof(ExportedMeshVertex));
            delete tmp;
        }
        delete Vertexes;

        Header.FaceCount = Faces->GetCount();
        Face = (ExportedMeshFace *)malloc(sizeof(ExportedMeshFace) * Faces->GetCount());
        for (int i=0; i<Faces->GetCount(); i++) {
            ExportedMeshFace *tmp = (ExportedMeshFace *)Faces->GetAt(i);
            memcpy(&Face[i], tmp, sizeof(ExportedMeshFace));
            delete tmp;
        }
        delete Faces;
 
        Header.SubMeshCount = SubMeshes->GetCount();
        SubMesh = (ExportedMeshSubmesh *)malloc(sizeof(ExportedMeshSubmesh) * SubMeshes->GetCount());
        for (int i=0; i<SubMeshes->GetCount(); i++) {
            ExportedMeshSubmesh *tmp = (ExportedMeshSubmesh *)SubMeshes->GetAt(i);
            memcpy(&SubMesh[i], tmp, sizeof(ExportedMeshSubmesh));
            delete tmp;
        }
        delete SubMeshes;
        return true;
    }
    return false;
}

bool CExportedMesh::Save(CString *FileName) {
        this->FileName = *FileName;
        Header.Version = MeshVersion;
        memset(Header.Header, 0, 32);
        strcpy(Header.Header, MeshHeader);
        CFile OutFile((LPCTSTR)*FileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
        OutFile.Write(&Header, sizeof(Header));
        OutFile.Write(Name, Header.NameLength);
        OutFile.Write(Vertex, Header.VertexCount * sizeof(ExportedMeshVertex));
        OutFile.Write(Face, Header.FaceCount * sizeof(ExportedMeshFace));
        OutFile.Write(SubMesh, Header.SubMeshCount * sizeof(ExportedMeshSubmesh));
        OutFile.Close();
        return true;
}