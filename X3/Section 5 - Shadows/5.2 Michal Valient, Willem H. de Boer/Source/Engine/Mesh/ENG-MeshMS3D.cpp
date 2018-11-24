#include "stdafx.h"
#include "eng-meshms3d.h"
#include "FIL-FileManager.h"
#include "MGR-DirectGraphicsManager.h"
#include "ENG-EngineDefaults.h"
#include "ENG-Engine.h"
#include "SC-HashTable2.h"
#include "eng-MeshPart.h"
#include "DG-DirectGraphicsDeviceInfo.h"

int CMeshMS3D::IsThereVertex(CImportMilkShape3D_Vertex *Vertex, CRootClassArray *Vertexes) {
    for (int i=0; i<Vertexes->GetSize(); i++) {
        CImportMilkShape3D_Vertex *V1 = (CImportMilkShape3D_Vertex *)Vertexes->GetAt(i);
        if (Vertex->Equals(V1)) return i;
    }
    return -1;
}

CMeshMS3D::CMeshMS3D(char *sObjName, CEngine *pOwnerEngine) : CMesh(sObjName, pOwnerEngine) {
}

bool CMeshMS3D::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
    this->LoadIndex = LoadIndex;
    int logE = _LOGB(this,D3_DV_GRP2_LEV2,"Loading MilkShape3D Object from resource %i",LoadIndex);
    bool result = TRUE;
    CFileManager *fil = OwnerEngine->GetFileManager();
    int ResSize = fil->GetSize(LoadIndex);
    if (ResSize<0) {
        _LOGE(logE,"Ending with failure - resource not found");
        result = FALSE;
    } else {
        //------------------------------------
        //Stage 0 - Unload (possibly) used memory
        this->ReleaseBuffers(TRUE);

	    sMS3D_Header    Header;
	    WORD            VertexCount;
        sMS3D_Vertex    *Vertexes = NULL;
        WORD            FaceCount;
        sMS3D_Face      *Faces = NULL;
        WORD            GroupCount;
        sMS3D_Group     *Groups = NULL;
        WORD            MaterialCount;
        sMS3D_Material  *Materials = NULL;

        //------------------------------------
        //Stage 1 - Load from media
        int res = 1;
        //header
        res = fil->Open(LoadIndex);
        res = fil->Read(LoadIndex, &Header, sizeof(sMS3D_Header));
        //vertexcount
        res = fil->Read(LoadIndex, &VertexCount, sizeof(VertexCount));
        if (VertexCount>0) {
            //vertexes
            Vertexes = (sMS3D_Vertex*)MemMgr->AllocMem((int)VertexCount * sizeof(sMS3D_Vertex),0);
            res = fil->Read(LoadIndex, Vertexes, (int)VertexCount * sizeof(sMS3D_Vertex));
        }
        //face count
        res = fil->Read(LoadIndex, &FaceCount, sizeof(FaceCount));
        if (FaceCount>0) {
            //faces
            Faces = (sMS3D_Face*)MemMgr->AllocMem((int)FaceCount * sizeof(sMS3D_Face),0);
            res = fil->Read(LoadIndex, Faces, (int)FaceCount * sizeof(sMS3D_Face));
        }
        //group count
        res = fil->Read(LoadIndex, &GroupCount, sizeof(GroupCount));
        if (GroupCount>0) {
            //groups
            Groups = (sMS3D_Group*)MemMgr->AllocMem((int)GroupCount * sizeof(sMS3D_Group),0);
            for (int i=0; i<GroupCount; i++) {
                res = fil->Read(LoadIndex, &Groups[i].flags, sizeof(Groups[i].flags));
                res = fil->Read(LoadIndex, &Groups[i].name, sizeof(Groups[i].name));
                res = fil->Read(LoadIndex, &Groups[i].numtriangles, sizeof(Groups[i].numtriangles));
                //group indices...
                Groups[i].triangleIndices = (WORD*)MemMgr->AllocMem((int)Groups[i].numtriangles * sizeof(WORD),0);
                res = fil->Read(LoadIndex, Groups[i].triangleIndices, (int)Groups[i].numtriangles * sizeof(WORD));
                res = fil->Read(LoadIndex, &Groups[i].materialIndex, sizeof(Groups[i].materialIndex));
            }
        }
        //material count
        res = fil->Read(LoadIndex, &MaterialCount, sizeof(MaterialCount));
        if (MaterialCount>0) {
            //materials
            Materials = (sMS3D_Material*)MemMgr->AllocMem((int)MaterialCount * sizeof(sMS3D_Material),0);
            res = fil->Read(LoadIndex, Materials, (int)MaterialCount * sizeof(sMS3D_Material));
        }
        res = fil->Close(LoadIndex);

        //------------------------------------
        //Stage 2 - create our structures & erase unused ones.
        this->Vertexes = new CRootClassArray(true);
        this->Materials = new CHashTable2(true, 0);

        _LOG(this,D3_DV_GRP2_LEV4,"Convert of data");
        _LOG(this,D3_DV_GRP2_LEV4,"Group count %i",GroupCount);
        for (int i=0; i<GroupCount; i++) {
            _LOG(this,D3_DV_GRP2_LEV4,"Start with group %i material index %i",i,Groups[i].materialIndex);
            CImportMilkShape3D_Material *Mat = (CImportMilkShape3D_Material *)this->Materials->Get(Groups[i].materialIndex);
            if (Mat == NULL) {  //Not in hash yet
                _LOG(this,D3_DV_GRP2_LEV4,"Material not found, creating new one");
                Mat = new CImportMilkShape3D_Material;
                Mat->MaterialID = Groups[i].materialIndex;
                this->Materials->Add(Groups[i].materialIndex,Mat);
            }
            //Add group to Material
            CImportMilkShape3D_Group *Grp = new CImportMilkShape3D_Group;
            Grp->GroupID = i;
            Mat->Groups->Add(Grp);
            _LOG(this,D3_DV_GRP2_LEV4,"Added group %i to material %i",i,Mat->MaterialID);
            //Add faces to group
            _LOG(this,D3_DV_GRP2_LEV4,"Start adding faces (count %i)",Groups[i].numtriangles);
            for (int j=0; j<Groups[i].numtriangles; j++) {
                _LOG(this,D3_DV_GRP2_LEV4,"Face %i",j);
                sMS3D_Face *Fc = &Faces[Groups[i].triangleIndices[j]];
                CImportMilkShape3D_Face *Face = new CImportMilkShape3D_Face;
                for (int k=0; k<3;k++) {
                    _LOG(this,D3_DV_GRP2_LEV4,"Face vertex %i",k);
                    CImportMilkShape3D_Vertex *V = new CImportMilkShape3D_Vertex;
                    _LOG(this,D3_DV_GRP2_LEV4,"Position %f,%f,%f",Vertexes[Fc->vertexIndices[k]].vertex[0],Vertexes[Fc->vertexIndices[k]].vertex[1], Vertexes[Fc->vertexIndices[k]].vertex[2]);
                    _LOG(this,D3_DV_GRP2_LEV4,"Normal   %f,%f,%f",Fc->vertexNormals[k][0],Fc->vertexNormals[k][1],Fc->vertexNormals[k][2]);
                    _LOG(this,D3_DV_GRP2_LEV4,"TexCoord %f,%f",Fc->s[k],Fc->t[k]);
                    memcpy(V->Position, Vertexes[Fc->vertexIndices[k]].vertex, sizeof(V->Position));
                    memcpy(V->Normal, Fc->vertexNormals[k], sizeof(V->Normal));
                    V->TexCoord[0] = Fc->s[k];
                    V->TexCoord[1] = Fc->t[k];
                    int VIndex = IsThereVertex(V,this->Vertexes);
                    if (VIndex<0) {
                        VIndex = this->Vertexes->Add(V);
                        _LOG(this,D3_DV_GRP2_LEV4,"Vertex not found, creating at index %i",VIndex);
                    } else {
                        _LOG(this,D3_DV_GRP2_LEV4,"Vertex stored index %i",VIndex);
                        delete V;
                    }
                    Face->Vertexes[k] = VIndex;
                }
                Grp->Faces->Add(Face);
            }
            MemMgr->FreeMem(Groups[i].triangleIndices);
        }
        if (Groups!=NULL) MemMgr->FreeMem(Groups);

        //------------------------------------
        //Stage 3 - free temporary data
        if (Vertexes!=NULL) MemMgr->FreeMem(Vertexes); 
        if (Materials!=NULL) MemMgr->FreeMem(Materials); 
        if (Faces!=NULL) MemMgr->FreeMem(Faces);

        if (res<0) {
            _LOGE(logE,"Ending with failure - resource not loaded");
            result = FALSE;
        } else {
            _LOGE(logE,"Resource loaded");
            result = TRUE;
        }
    }

    logE = _LOGB(this,D3_DV_GRP2_LEV2,"Assigning MS3D resource %i to mesh",this->LoadIndex);
    if (this->Vertexes == NULL || this->Materials ==NULL || this->Vertexes->GetSize()<=0 || this->Materials->GetItemCount()<=0) {
        _LOG(this,D3_DV_GRP2_LEV0,"ImportMshape3D - Incorrect input in data (mesh was not loaded?)");
        result = FALSE;
    } else {
        //first we need to know, if our hardware is capable enough to hold these verticles.
        if ((DWORD)this->Vertexes->GetSize() > this->OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.MaxPrimitiveCount ||
            (DWORD)this->Vertexes->GetSize() > this->OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.MaxVertexIndex) {
            _LOG(this,D3_DV_GRP2_LEV0,"ImportMshape3D - Model has %u verticles, but device supports only %u primitives and %u vertexes",
                this->Vertexes->GetSize(),
                this->OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.MaxPrimitiveCount,
                this->OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.MaxVertexIndex);
            result = FALSE;
        } else {    //the good way
            //If needed, delete vertex and index buffers
            this->ReleaseBuffers(TRUE);
            //Create one huge vertex buffer and fill it (might need optimalization later)
            CMeshPart *Buf = (CMeshPart *)new CMeshPart;
            Buf->VertexBufferCreate(this->OwnerEngine->GetGraphicsManager()->Direct3DDevice,
                                    this->Vertexes->GetSize() * sizeof(MS3D_VERTEX), 
                                    OwnerEngine->GetEngineDefaults()->VBuf_Usage, 
                                    MS3D_FVF, OwnerEngine->GetEngineDefaults()->VBuf_Pool);
            MS3D_VERTEX *buf = (MS3D_VERTEX *)Buf->VertexLock(0,this->Vertexes->GetSize() * sizeof(MS3D_VERTEX),0);
            for (int i=0; i<this->Vertexes->GetSize(); i++) {
                CImportMilkShape3D_Vertex *VVert = (CImportMilkShape3D_Vertex *)this->Vertexes->GetAt(i);
                buf[i].Position.x = VVert->Position[0];
                buf[i].Position.y = VVert->Position[1];
                buf[i].Position.z = VVert->Position[2];
                buf[i].Normal.x = VVert->Normal[0];
                buf[i].Normal.y = VVert->Normal[1];
                buf[i].Normal.z = VVert->Normal[2];
                buf[i].U = VVert->TexCoord[0];
                buf[i].V = VVert->TexCoord[1];
            }
            Buf->VertexUnlock();
            //Create subsets in mesh for each group / material combination.
            int isNextMaterial = this->Materials->GetFirstItem();
            CMeshPart *VBuf = Buf;
            while (isNextMaterial>=0) {
                CImportMilkShape3D_Material *Mat = (CImportMilkShape3D_Material *)this->Materials->GetValueAtIndex(isNextMaterial);
                for (int j=0; j<Mat->Groups->GetSize(); j++) {
                    if (VBuf==NULL) {
                        VBuf = (CMeshPart *)new CMeshPart;
                        VBuf->VertexUseExisting(*Buf);
                    }
                    DWORD VBMinIndex = 0;
                    DWORD VBMaxIndex = 0;
                    CImportMilkShape3D_Group *Grp = (CImportMilkShape3D_Group *)Mat->Groups->GetAt(j);
                    //(UINT Length, DWORD Usage = D3DUSAGE_SOFTWAREPROCESSING, D3DFORMAT Format = D3DFMT_INDEX16, D3DPOOL Pool = D3DPOOL_MANAGED)
                    if (Grp->Faces->GetSize() * 3>0xFFFF) {
                        VBuf->IndexBufferCreate(this->OwnerEngine->GetGraphicsManager()->Direct3DDevice,
                                                Grp->Faces->GetSize() * 3 * sizeof(DWORD),
                                                OwnerEngine->GetEngineDefaults()->IBuf_Usage,
                                                D3DFMT_INDEX32, OwnerEngine->GetEngineDefaults()->IBuf_Pool);
                        DWORD *buf = (DWORD *)VBuf->IndexLock(0,Grp->Faces->GetSize() * 3 * sizeof(DWORD),0);
                        for (int k=0; k< Grp->Faces->GetSize(); k++) {
                            CImportMilkShape3D_Face *Face = (CImportMilkShape3D_Face *)Grp->Faces->GetAt(k);
                            buf[3*k + 0] = (DWORD)Face->Vertexes[0];
                            buf[3*k + 1] = (DWORD)Face->Vertexes[1];
                            buf[3*k + 2] = (DWORD)Face->Vertexes[2];
                            VBMinIndex = min(VBMinIndex, min((DWORD)Face->Vertexes[0], min((DWORD)Face->Vertexes[1],(DWORD)Face->Vertexes[2])));
                            VBMaxIndex = max(VBMaxIndex, max((DWORD)Face->Vertexes[0], max((DWORD)Face->Vertexes[1],(DWORD)Face->Vertexes[2])));
                        }
                        VBuf->IndexUnlock();
                    } else {
                        VBuf->IndexBufferCreate(this->OwnerEngine->GetGraphicsManager()->Direct3DDevice,
                                                Grp->Faces->GetSize() * 3 * sizeof(DWORD),
                                                OwnerEngine->GetEngineDefaults()->IBuf_Usage,
                                                D3DFMT_INDEX16, OwnerEngine->GetEngineDefaults()->IBuf_Pool);
                        WORD *buf = (WORD *)VBuf->IndexLock(0,Grp->Faces->GetSize() * 3 * sizeof(WORD),0);
                        for (int k=0; k< Grp->Faces->GetSize(); k++) {
                            CImportMilkShape3D_Face *Face = (CImportMilkShape3D_Face *)Grp->Faces->GetAt(k);
                            buf[3*k + 0] = (WORD)Face->Vertexes[0];
                            buf[3*k + 1] = (WORD)Face->Vertexes[1];
                            buf[3*k + 2] = (WORD)Face->Vertexes[2];
                            VBMinIndex = min(VBMinIndex, min((WORD)Face->Vertexes[0], min((WORD)Face->Vertexes[1],(WORD)Face->Vertexes[2])));
                            VBMaxIndex = max(VBMaxIndex, max((WORD)Face->Vertexes[0], max((WORD)Face->Vertexes[1],(WORD)Face->Vertexes[2])));
                        }
                        VBuf->IndexUnlock();
                    }
                    VBuf->SetStrideSize(sizeof(MS3D_VERTEX));
                    VBuf->SetBaseVertexIndex(0);
                    VBuf->SetPrimitiveType(D3DPT_TRIANGLELIST);
                    VBuf->SetMinIndex(VBMinIndex);
                    VBuf->SetNumVerticles(VBMaxIndex - VBMinIndex + 1);
                    VBuf->SetStartIndex(0);
                    VBuf->SetPrimitiveCount(Grp->Faces->GetSize());
                    this->AddSubset(*VBuf);
                    VBuf = NULL;
                }
                isNextMaterial = this->Materials->GetNextItem(isNextMaterial);
            }
            this->SetFilled(true);
            this->SetRestorable(true);
        }
    }
    _LOGE(logE,"End of assigment");

    logE = _LOGB(this,D3_DV_GRP2_LEV2,"Unloading from memory (resource %i)",LoadIndex);
    if (this->Vertexes!=NULL) delete this->Vertexes;
    if (this->Materials!=NULL) delete this->Materials;
    this->Vertexes = NULL;
    this->Materials = NULL;
    _LOGE(logE,"Resource unloaded");

    return result;
}

bool CMeshMS3D::Update(CRenderSettings &RenderSettings) {
    return TRUE;
}
