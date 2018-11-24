#include "stdafx.h"
#include "D3Format\d3f_scene.h"
#include "D3Format\d3f_mesh.h"
#include "D3Format\d3f_Object.h"
#include "D3Format\d3f_Light.h"
#include "D3Format\d3f_Camera.h"
#include "d3format\d3f_Texture.h"
#include "d3format\d3f_Material.h"
#include "SC-DynamicArray.h"
#include "SC-HashTable2.h"

CD3F_Scene::CD3F_Scene(char *ObjName,CEngine *OwnerEngine) : CBaseClass(ObjName,OwnerEngine) {
    Meshes = new CRootClassArray(true);
    Objects = new CHashTable2(true, 0);
    Lights = new CHashTable2(true, 0);
    Cameras = new CHashTable2(true, 0);
    Textures = new CHashTable2(true, 0);
    Materials = new CHashTable2(true, 0);
}

CD3F_Scene::~CD3F_Scene(void) {
    delete Meshes;
    delete Objects;
    delete Lights;
    delete Cameras;
    delete Textures;
    delete Materials;
}

void CD3F_Scene::SaveXML(char *Filename) {
    CMString SceneFile = Filename;
    int logE = _LOGB(this,D3_DV_GRP0_LEV0,"Start of mesh XML export");
    //Mesh exporter part
    for (int i=0;i<Meshes->GetSize(); i++) {
        CD3F_Mesh *Mesh = (CD3F_Mesh *)Meshes->GetAt(i);
        Mesh->FileName = SceneFile.ExtractFile() + CMString(".") + Mesh->Name;
        CMString MFile = SceneFile.ExtractPath() + CMString("\\") + Mesh->FileName +  CMString(".xml");
        FILE *Of = fopen(MFile.c_str(),"wt");
        if (Of!=NULL) {
            fprintf(Of,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
            CMString Indent = "";
            _LOG(this,D3_DV_GRP0_LEV0,"Mesh '%s' - Saving to '%s'", Mesh->Name.c_str(),MFile.c_str());
            Mesh->SaveXML(Of,Indent);
            fclose(Of);
        } else {
            _LOG(this,D3_DV_GRP0_LEV0,"Mesh '%s' - Cannot save to '%s'", Mesh->Name.c_str(),MFile.c_str());
        }
    }
    _LOGE(logE,"End of mesh XML export");
    logE = _LOGB(this,D3_DV_GRP0_LEV0,"Start of scene XML export");
    SceneFile = SceneFile.ExtractPath() + CMString("\\") + SceneFile.ExtractFile() + CMString(".xml");
    FILE *Of = fopen(SceneFile.c_str(),"wt");
    if (Of!=NULL) {
        _LOG(this,D3_DV_GRP0_LEV0,"Scene - Saving to '%s'", SceneFile.c_str());
        fprintf(Of,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        fprintf(Of,"<D3F_SCENE>\n");
        fprintf(Of,"  <D3F_MESHLINKS COUNT='%u'>\n",Meshes->GetSize());
        for (i=0;i<Meshes->GetSize();i++) {
            CD3F_Mesh *Mesh = (CD3F_Mesh *)Meshes->GetAt(i);
            _LOG(this,D3_DV_GRP0_LEV0,"Mesh link '%s'", Mesh->Name.c_str());
            fprintf(Of,"    <D3F_MESHLINK MESHID='%u' MESHNAME='%s' MESHFILE='%s'/>\n",Mesh->Index, Mesh->Name.c_str(), Mesh->FileName.c_str());
        }
        fprintf(Of,"  </D3F_MESHLINKS>\n");
        fprintf(Of,"  <D3F_OBJECTS COUNT='%u'>\n",Objects->GetItemCount());
        CMString Indent = "    ";
        if (Objects->GetItemCount()>0) {
            UINT hasNext = Objects->GetFirstItem();
            while (1) {
                CD3F_Object *Obj = (CD3F_Object *)Objects->GetValueAtIndex(hasNext);
                _LOG(this,D3_DV_GRP0_LEV0,"Object '%s' - Saving to '%s'", Obj->Name.c_str(), SceneFile.c_str());
                Obj->SaveXML(Of,Indent);
                UINT hasNext2 = Objects->GetNextItem(hasNext);
                if (hasNext2 == hasNext) break;
                else hasNext = hasNext2;
            }
        }
        fprintf(Of,"  </D3F_OBJECTS>\n");
        fprintf(Of,"  <D3F_CAMERAS COUNT='%u'>\n",Cameras->GetItemCount());
        Indent = "    ";
        if (Cameras->GetItemCount()>0) {
            UINT hasNext = Cameras->GetFirstItem();
            while (1) {
                CD3F_Camera *Obj = (CD3F_Camera *)Cameras->GetValueAtIndex(hasNext);
                _LOG(this,D3_DV_GRP0_LEV0,"Camera '%s' - Saving to '%s'", Obj->Name.c_str(), SceneFile.c_str());
                Obj->SaveXML(Of,Indent);
                UINT hasNext2 = Cameras->GetNextItem(hasNext);
                if (hasNext2 == hasNext) break;
                else hasNext = hasNext2;
            }
        }
        fprintf(Of,"  </D3F_CAMERAS>\n");
        fprintf(Of,"  <D3F_LIGHTS COUNT='%u'>\n",Lights->GetItemCount());
        Indent = "    ";
        if (Lights->GetItemCount()>0) {
            UINT hasNext = Lights->GetFirstItem();
            while (1) {
                CD3F_Light *Obj = (CD3F_Light *)Lights->GetValueAtIndex(hasNext);
                _LOG(this,D3_DV_GRP0_LEV0,"Light '%s' - Saving to '%s'", Obj->Name.c_str(), SceneFile.c_str());
                Obj->SaveXML(Of,Indent);
                UINT hasNext2 = Lights->GetNextItem(hasNext);
                if (hasNext2 == hasNext) break;
                else hasNext = hasNext2;
            }
        }
        fprintf(Of,"  </D3F_LIGHTS>\n");

        fprintf(Of,"  <D3F_TEXTURES COUNT='%u'>\n",Textures->GetItemCount());
        Indent = "    ";
        if (Textures->GetItemCount()>0) {
            UINT hasNext = Textures->GetFirstItem();
            while (1) {
                CD3F_Texture *Obj = (CD3F_Texture *)Textures->GetValueAtIndex(hasNext);
                _LOG(this,D3_DV_GRP0_LEV0,"Texture '%s' - Saving to '%s'", Obj->FileName.c_str(), SceneFile.c_str());
                Obj->SaveXML(Of,Indent);
                UINT hasNext2 = Textures->GetNextItem(hasNext);
                if (hasNext2 == hasNext) break;
                else hasNext = hasNext2;
            }
        }
        fprintf(Of,"  </D3F_TEXTURES>\n");

        fprintf(Of,"  <D3F_MATERIALS COUNT='%u'>\n",Materials->GetItemCount());
        Indent = "    ";
        if (Materials->GetItemCount()>0) {
            UINT hasNext = Materials->GetFirstItem();
            while (1) {
                CD3F_Material *Obj = (CD3F_Material *)Materials->GetValueAtIndex(hasNext);
                _LOG(this,D3_DV_GRP0_LEV0,"Material '%s' - Saving to '%s'", Obj->Name.c_str(), SceneFile.c_str());
                Obj->SaveXML(Of,Indent);
                UINT hasNext2 = Materials->GetNextItem(hasNext);
                if (hasNext2 == hasNext) break;
                else hasNext = hasNext2;
            }
        }
        fprintf(Of,"  </D3F_MATERIALS>\n");

        fprintf(Of,"</D3F_SCENE>\n");
        fclose(Of);
    } else {
        _LOG(this,D3_DV_GRP0_LEV0,"Scene - Cannot save to '%s'", SceneFile.c_str());
    }
   _LOGE(logE,"End of scene export");
}

void CD3F_Scene::SaveBIN(char *Filename) {
    CMString SceneFile = Filename;
    int logE = _LOGB(this,D3_DV_GRP0_LEV0,"Start of mesh BIN export");
    //Mesh exporter part
    for (int i=0;i<Meshes->GetSize(); i++) {
        CD3F_Mesh *Mesh = (CD3F_Mesh *)Meshes->GetAt(i);
        Mesh->FileName = SceneFile.ExtractFile() + CMString(".") + Mesh->Name;
        CMString MFile = SceneFile.ExtractPath() + CMString("\\") + Mesh->FileName +  CMString(".d3mesh");
        FILE *Of = fopen(MFile.c_str(),"wb");
        if (Of!=NULL) {
            _LOG(this,D3_DV_GRP0_LEV0,"Mesh '%s' - Saving to '%s'", Mesh->Name.c_str(),MFile.c_str());
            Mesh->SaveBIN(Of);
            fclose(Of);
        } else {
            _LOG(this,D3_DV_GRP0_LEV0,"Mesh '%s' - Cannot save to '%s'", Mesh->Name.c_str(),MFile.c_str());
        }
    }
    _LOGE(logE,"End of mesh BIN export");
    logE = _LOGB(this,D3_DV_GRP0_LEV0,"Start of scene BIN export");
    SceneFile = SceneFile.ExtractPath() + CMString("\\") + SceneFile.ExtractFile() + CMString(".d3scene");
    FILE *Of = fopen(SceneFile.c_str(),"wb");
    if (Of!=NULL) {
        //Header
        SD3F_Scene s;
        memset(s.TAG,' ',sizeof(s.TAG));
        strcpy(s.TAG,"Dimension3 Scene by Em/Eastwood 2002");
        s.Version = 1;
        s.CameraCount = Cameras->GetItemCount();
        s.MeshCount = Meshes->GetSize();
        s.LightCount = Lights->GetItemCount();
        s.ObjCount = Objects->GetItemCount();
        s.TextureCount = Textures->GetItemCount();
        s.MaterialCount = Materials->GetItemCount();
        fwrite(&s,sizeof(s),1,Of);
        //Mesh links
        _LOG(this,D3_DV_GRP0_LEV0,"Scene - Saving to '%s'", SceneFile.c_str());
        for (i=0;i<Meshes->GetSize();i++) {
            CD3F_Mesh *Mesh = (CD3F_Mesh *)Meshes->GetAt(i);
            _LOG(this,D3_DV_GRP0_LEV0,"Mesh link '%s'", Mesh->Name.c_str());
            SD3F_MeshLink ms;
            ms.TAG = 'KNLM';
            memset(ms.MeshName,' ',sizeof(ms.MeshName));
            strcpy(ms.MeshName,Mesh->Name.c_str());
            memset(ms.MeshFile,' ',sizeof(ms.MeshFile));
            strcpy(ms.MeshFile,Mesh->FileName.c_str());
            ms.MeshID = Mesh->Index;
            fwrite(&ms,sizeof(ms),1,Of);
        }
        //Objects
        if (Objects->GetItemCount()>0) {
            UINT hasNext = Objects->GetFirstItem();
            while (1) {
                CD3F_Object *Obj = (CD3F_Object *)Objects->GetValueAtIndex(hasNext);
                _LOG(this,D3_DV_GRP0_LEV0,"Object '%s' - Saving to '%s'", Obj->Name.c_str(), SceneFile.c_str());
                Obj->SaveBIN(Of);
                UINT hasNext2 = Objects->GetNextItem(hasNext);
                if (hasNext2 == hasNext) break;
                else hasNext = hasNext2;
            }
        }
        //Cameras
        if (Cameras->GetItemCount()>0) {
            UINT hasNext = Cameras->GetFirstItem();
            while (1) {
                CD3F_Camera *Obj = (CD3F_Camera *)Cameras->GetValueAtIndex(hasNext);
                _LOG(this,D3_DV_GRP0_LEV0,"Camera '%s' - Saving to '%s'", Obj->Name.c_str(), SceneFile.c_str());
                Obj->SaveBIN(Of);
                UINT hasNext2 = Cameras->GetNextItem(hasNext);
                if (hasNext2 == hasNext) break;
                else hasNext = hasNext2;
            }
        }
        //Lights
        if (Lights->GetItemCount()>0) {
            UINT hasNext = Lights->GetFirstItem();
            while (1) {
                CD3F_Light *Obj = (CD3F_Light *)Lights->GetValueAtIndex(hasNext);
                _LOG(this,D3_DV_GRP0_LEV0,"Light '%s' - Saving to '%s'", Obj->Name.c_str(), SceneFile.c_str());
                Obj->SaveBIN(Of);
                UINT hasNext2 = Lights->GetNextItem(hasNext);
                if (hasNext2 == hasNext) break;
                else hasNext = hasNext2;
            }
        }
        //Textures
        if (Textures->GetItemCount()>0) {
            UINT hasNext = Textures->GetFirstItem();
            while (1) {
                CD3F_Texture *Obj = (CD3F_Texture *)Textures->GetValueAtIndex(hasNext);
                _LOG(this,D3_DV_GRP0_LEV0,"Texture '%s' - Saving to '%s'", Obj->FileName.c_str(), SceneFile.c_str());
                Obj->SaveBIN(Of);
                UINT hasNext2 = Textures->GetNextItem(hasNext);
                if (hasNext2 == hasNext) break;
                else hasNext = hasNext2;
            }
        }
        //Materials
        if (Materials->GetItemCount()>0) {
            UINT hasNext = Materials->GetFirstItem();
            while (1) {
                CD3F_Material *Obj = (CD3F_Material *)Materials->GetValueAtIndex(hasNext);
                _LOG(this,D3_DV_GRP0_LEV0,"Material '%s' - Saving to '%s'", Obj->Name.c_str(), SceneFile.c_str());
                Obj->SaveBIN(Of);
                UINT hasNext2 = Materials->GetNextItem(hasNext);
                if (hasNext2 == hasNext) break;
                else hasNext = hasNext2;
            }
        }
        fclose(Of);
    } else {
        _LOG(this,D3_DV_GRP0_LEV0,"Scene - Cannot save to '%s'", SceneFile.c_str());
    }
   _LOGE(logE,"End of scene export");
}

void CD3F_Scene::LoadBIN(char *Buffer, DWORD Length, DWORD &CurrPosition) {
    SD3F_Scene s;
    memcpy(&s,&Buffer[CurrPosition],sizeof(s));
    CurrPosition+=sizeof(s);
    CMString TAG = s.TAG;
    if (strcmp(TAG.c_str(),"Dimension3 Scene by Em/Eastwood 2002")!=0) {
        _WARN(NULL,"CD3F_Scene",ErrMgr->TFile,"Bad tag at %u found '%s' expecting 'Dimension3 Scene by Em/Eastwood 2002' (DataSize is %u)",CurrPosition,TAG.c_str(),sizeof(s));
    }
    //Mesh links
    for (int i=0;i<s.MeshCount;i++) {
        CD3F_MeshLink *MLink = new CD3F_MeshLink;
        SD3F_MeshLink ms;
        memcpy(&ms,&Buffer[CurrPosition],sizeof(ms));
        CurrPosition+=sizeof(ms);
        if (ms.TAG!='KNLM') {
            char tmp[5];
            memcpy(tmp,&s.TAG,4);
            tmp[4] = 0;
            _WARN(NULL,"CD3F_Scene",ErrMgr->TFile,"Bad tag at %u found '%s' expecting 'MLNK' (DataSize is %u)",CurrPosition,tmp,sizeof(s));
        }
        MLink->MeshID = ms.MeshID;
        MLink->MeshName = ms.MeshName;
        MLink->MeshFile = ms.MeshFile;
        Meshes->Add(MLink);
    }
    //Objects
    for (i=0;i<s.ObjCount;i++) {
        CD3F_Object *Obj = new CD3F_Object("Obj",(CEngine *)this);
        Obj->LoadBIN(Buffer, Length, CurrPosition);
        Objects->Add(Obj->ID,Obj);
    }
    //Cameras
    for (i=0;i<s.CameraCount;i++) {
        CD3F_Camera *Obj = new CD3F_Camera("Obj",(CEngine *)this);
        Obj->LoadBIN(Buffer, Length, CurrPosition);
        Cameras->Add(Obj->ID,Obj);
    }
    //Lights
    for (i=0;i<s.LightCount;i++) {
        CD3F_Light *Obj = new CD3F_Light("Obj",(CEngine *)this);
        Obj->LoadBIN(Buffer, Length, CurrPosition);
        Lights->Add(Obj->ID,Obj);
    }
    //Textures
    for (i=0;i<s.TextureCount;i++) {
        CD3F_Texture *Obj = new CD3F_Texture("Obj",(CEngine *)this);
        Obj->LoadBIN(Buffer, Length, CurrPosition);
        Textures->Add(Obj->ID,Obj);
    }
    //Materials
    for (i=0;i<s.MaterialCount;i++) {
        CD3F_Material *Obj = new CD3F_Material("Obj",(CEngine *)this);
        Obj->LoadBIN(Buffer, Length, CurrPosition);
        Materials->Add(Obj->ID,Obj);
    }
}
