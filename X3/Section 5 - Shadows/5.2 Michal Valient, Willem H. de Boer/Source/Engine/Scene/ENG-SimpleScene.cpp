#include "stdafx.h"
#include "eng-simplescene.h"

#include "eng-rendergroup.h"
#include "SC-HashTable2.h"
#include "SC-DynamicArray.h"
#include "sc-xmlparser.h"
#include "ENG-Engine.h"
#include "ENG-RenderSettings.h"
#include "ENG-Texture2D.h"
#include "ENG-TextureManager.h"
#include "FIL-FileManager.h"
#include "ENG-MeshD3f.h"
#include "ENG-MeshPart.h"
#include "ENG-MeshManager.h"
#include "D3Scene\ENG-D3SObject.h"
#include "D3Scene\ENG-D3SCamera.h"
#include "D3Scene\ENG-D3SLight.h"
#include "d3scene\eng-d3starget.h"
#include "d3scene\eng-d3s_prslinearcontroller.h"
#include "Material\MAT-MaterialManager.h"
#include "material\mat-dx9material.h"
#include "DG-DirectGraphicsDeviceInfo.h"
#include "MGR-DirectGraphicsManager.h"

#include "eng-frustum.h"

CSimpleScene::CSimpleScene(char *ObjName,CEngine *OwnerEngine) : CBaseClass(ObjName, OwnerEngine) {
    int logE = _LOGB(this,D3_DV_GRP1_LEV0,"Creating scene");
    Objects = new CHashTable2(TRUE, 0);
    Cameras = new CHashTable2(FALSE, 0);
    Lights = new CHashTable2(FALSE, 0);
    Groups = new CHashTable2(TRUE, 0);

    CurrentCamera = NULL;
    _LOGE(logE,"Created");
}

CSimpleScene::~CSimpleScene(void) {
    int logE = _LOGB(this,D3_DV_GRP1_LEV0,"Destroying scene");

    //Then unset vertex and index buffers
    int StreamCount = OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.MaxStreams;
    for (int i=0; i<StreamCount; i++) {
        OwnerEngine->GetMeshManager()->SetStreamSource(i,NULL,0,0);
    }
    OwnerEngine->GetMeshManager()->SetIndices(NULL);

    //then unset textures
    int TextureCount = OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.MaxTextureBlendStages;
    for (int i=0; i<TextureCount; i++) {
        OwnerEngine->GetTextureManager()->SetTexture(i,NULL);
    }

    delete Groups;
    delete Objects;
    delete Cameras;
    delete Lights;
    _LOGE(logE,"Destroyed");
}

void CSimpleScene::AddGroupByID(DWORD GroupID, CRenderGroup &Group) {
    Groups->Add(GroupID,&Group);
    Group.SetGroupID(GroupID);
}

void CSimpleScene::AddObject(DWORD GroupID, CBase3DObject &Object) {
    int resID = Object.GetResourceID();
    Objects->Add(resID, &Object);
    CRenderGroup *Group = (CRenderGroup *)Groups->Get(GroupID);
    Group->AddObject(Object);
}

void CSimpleScene::AddCamera(CBase3DObject &Camera) {
    Objects->Add(Camera.GetResourceID(), &Camera);
    Cameras->Add(Camera.GetResourceID(), &Camera);
}

void CSimpleScene::AddLight(CBase3DObject &Light) {
    int resID = Light.GetResourceID();
    {
        Objects->Add(resID, &Light);
        Lights->Add(resID, &Light);
    }
}

void CSimpleScene::Render(CRenderSettings &Settings) {
    int logE = _LOGB(this,D3_DV_GRP1_LEV4,"Rendering scene");
    if (Objects->GetItemCount()>0) {
        ////Render target work
        //IDirect3DSurface9 *SceneRenderTarget = NULL;
        //IDirect3DSurface9 *SceneDepthStencil = NULL;
        //HRESULT resRT = Settings.Direct3DDevice->GetRenderTarget(0, &SceneRenderTarget);
        //if (FAILED(resRT)) {
        //    _WARN(this, ErrMgr->TNoSupport, "Failed to store scene render target");
        //    SceneRenderTarget = NULL;
        //}
        //HRESULT resDS = Settings.Direct3DDevice->GetDepthStencilSurface(&SceneDepthStencil);
        //if (FAILED(resDS)) {
        //    _WARN(this, ErrMgr->TNoSupport, "Failed to store scene depth-stencil surface");
        //    SceneDepthStencil = NULL;
        //}
        
        UINT hRes;
        //Prepare bounding geometry of entire scene
        hRes = Objects->GetFirstItem();
        while (1) {
            CBase3DObject *Obj = (CBase3DObject *)Objects->GetValueAtIndex(hRes);
            Obj->PrepareBoundingGeometry(Settings);
            UINT hRes2 = Objects->GetNextItem(hRes);
            if (hRes2 == hRes) break;
            else hRes = hRes2;
        }

        //Compute camera view
        this->CurrentCamera->Apply(Settings);
        Settings.ComputeCameraMatrix();

        //Frustum culling in work
        int logE2 = _LOGB(this,D3_DV_GRP1_LEV4,"Culling");
        CFrustum *Fr = new CFrustum("Frustum",OwnerEngine);
        Fr->CalculateFrustum(Settings.ViewMatrix, Settings.ProjectionMatrix);

        hRes = Groups->GetFirstItem();
        while (1) {
            CRenderGroup *Group = (CRenderGroup *)Groups->GetValueAtIndex(hRes);
            Group->ClearVisible();
            for (int j=0; j<Group->Objects->GetSize(); j++) {
                CBase3DObject * obj = (CBase3DObject *)Group->Objects->GetAt(j);
                if (Fr->SphereInFrustum(obj->WorldBSCenter, obj->WorldBSRadius)) {
                    Group->AddVisible((CBase3DObject &) *Group->Objects->GetAt(j));
                    _LOG(this,D3_DV_GRP1_LEV4,"Object %s visible",obj->GetObjName());
                } else {
                    _LOG(this,D3_DV_GRP1_LEV4,"Object %s not visible",obj->GetObjName());
                }
            }
            UINT hRes2 = Groups->GetNextItem(hRes);
            if (hRes2 == hRes) break;
            else hRes = hRes2;
        }
        delete Fr;
        _LOGE(logE2, "Culled");

        //prepare geometry of lights (the frustum matrix)
        if (Lights->GetItemCount()>0) {
            hRes = Lights->GetFirstItem();
            while (1) {
                CD3SLight *Obj = (CD3SLight *)Lights->GetValueAtIndex(hRes);
                Obj->PrepareGeometry(Settings);
                Obj->LitObjects->RemoveAll();
                UINT hRes2 = Lights->GetNextItem(hRes);
                if (hRes2 == hRes) break;
                else hRes = hRes2;
            }

            //Compute lights for objects (and vice versa)
            hRes = Objects->GetFirstItem();
            while (1) {
                CBase3DObject *Obj = (CBase3DObject *)Objects->GetValueAtIndex(hRes);
                Obj->Lights->RemoveAll();
                UINT hRes2 = Lights->GetFirstItem();
                while (1) {
                    CD3SLight *Lght = (CD3SLight *)Lights->GetValueAtIndex(hRes2);
                    if (Lght->TestObjectLit(Obj)) {
                        Obj->Lights->Add(Lght);
                        Lght->AddLitObject(Obj);
                    }
                    UINT hResX4 = Lights->GetNextItem(hRes2);
                    if (hResX4 == hRes2) break;
                    else hRes2 = hResX4;
                }
                UINT hResX2 = Objects->GetNextItem(hRes);
                if (hResX2 == hRes) break;
                else hRes = hResX2;
            }

            hRes = Lights->GetFirstItem();
            while (1) {
                CD3SLight *Light = (CD3SLight *)Lights->GetValueAtIndex(hRes);
                Light->RenderShadowBuffer(Settings);
                UINT hRes2 = Lights->GetNextItem(hRes);
                if (hRes2 == hRes) break;
                else hRes = hRes2;
            }
        }

        //Compute camera view
        this->CurrentCamera->Apply(Settings);
        Settings.ComputeCameraMatrix();

        hRes = Groups->GetFirstItem();
        while (1) {
            CRenderGroup *Group = (CRenderGroup *)Groups->GetValueAtIndex(hRes);
            DWORD VisibleCount = Group->GetVisibleCount();
            for (DWORD j=0; j<VisibleCount; j++) {
                CBase3DObject *obj = Group->GetVisible(j);
                if (!obj->GetCacheInCount()) obj->CacheIn(Settings);
                obj->PrepareGeometry(Settings);
            }
            UINT hRes2 = Groups->GetNextItem(hRes);
            if (hRes2 == hRes) break;
            else hRes = hRes2;
        }
        ////Render target work
        //if (SceneRenderTarget!=NULL) {
        //    resRT = Settings.Direct3DDevice->SetRenderTarget(0, SceneRenderTarget);
        //    if (FAILED(resRT)) {
        //        _WARN(this, ErrMgr->TNoSupport, "Failed to set scene render target");
        //    }
        //    SceneRenderTarget->Release();
        //}
        //if (SceneDepthStencil!=NULL) {
        //    resDS = Settings.Direct3DDevice->SetDepthStencilSurface(SceneDepthStencil);
        //    if (FAILED(resDS)) {
        //        _WARN(this, ErrMgr->TNoSupport, "Failed to set scene depth-stencil surface");
        //    }
        //    SceneDepthStencil->Release();
        //}

        Settings.Direct3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0 );

        hRes = Groups->GetFirstItem();
        while (1) {
            CRenderGroup *Group = (CRenderGroup *)Groups->GetValueAtIndex(hRes);
            Group->Render(Settings);
            UINT hRes2 = Groups->GetNextItem(hRes);
            if (hRes2 == hRes) 
                break;
            else 
                hRes = hRes2;
        }

    }
    _LOGE(logE,"Rendered");
}

void CSimpleScene::DeviceLost(CRenderSettings &Settings) {
    int logE = _LOGB(this,D3_DV_GRP1_LEV0,"Sending Lost device");
    UINT hRes = Objects->GetFirstItem();
    while (1) {
        CBase3DObject *Obj = (CBase3DObject *)Objects->GetValueAtIndex(hRes);
        Obj->DeviceLost(Settings);    
        UINT hRes2 = Objects->GetNextItem(hRes);
        if (hRes2 == hRes) break;
        else hRes = hRes2;
    }
    _LOGE(logE,"Sended");
}

void CSimpleScene::Restore(CRenderSettings &Settings) {
    int logE = _LOGB(this,D3_DV_GRP1_LEV0,"Sending Restore");
    UINT hRes = Objects->GetFirstItem();
    while (1) {
        CBase3DObject *Obj = (CBase3DObject *)Objects->GetValueAtIndex(hRes);
        Obj->Restore(Settings);    
        UINT hRes2 = Objects->GetNextItem(hRes);
        if (hRes2 == hRes) break;
        else hRes = hRes2;
    }
    _LOGE(logE,"Sended");
}


bool CSimpleScene::LoadXML(CXMLNode *Node, CRenderSettings &Settings) {
    CXMLNode *nodeMeshes = NULL;
    CXMLNode *nodeTextures = NULL;
    CXMLNode *nodeMaterials = NULL;
    CXMLNode *nodeObjects = NULL;
    CXMLNode *nodeLights = NULL;
    CXMLNode *nodeCameras = NULL;
    CXMLNode *nodeControllers = NULL;
    for (UINT i=0; i<Node->GetSubNodeCount(); i++) {
        CXMLNode *tmpNode = Node->GetSubNode(i);
        if (strcmp(tmpNode->GetName().c_str(), "D3F_MESHLINKS") == 0) {
            nodeMeshes = tmpNode;
        } else if (strcmp(tmpNode->GetName().c_str(), "D3F_OBJECTS") == 0) {
            nodeObjects = tmpNode;
        } else if (strcmp(tmpNode->GetName().c_str(), "D3F_LIGHTS") == 0) {
            nodeLights = tmpNode;
        } else if (strcmp(tmpNode->GetName().c_str(), "D3F_CAMERAS") == 0) {
            nodeCameras = tmpNode;
        } else if (strcmp(tmpNode->GetName().c_str(), "D3F_MATERIALS") == 0) {
            nodeMaterials = tmpNode;
        } else if (strcmp(tmpNode->GetName().c_str(), "D3F_TEXTURES") == 0) {
            nodeTextures = tmpNode;
        } else if (strcmp(tmpNode->GetName().c_str(), "D3F_CONTROLLERS") == 0) {
            nodeControllers = tmpNode;
        }
    }
    CFileManager *fil = OwnerEngine->GetFileManager();
    if (nodeMeshes) {
        //First load all meshes
        for (UINT i=0; i<nodeMeshes->GetSubNodeCount(); i++) {
            CXMLNode *tmpNode = nodeMeshes->GetSubNode(i);
            if (strcmp(tmpNode->GetName().c_str(), "D3F_MESHLINK") == 0) {
                CMString tmpMeshName = "UnknownMesh";
                tmpNode->GetAttributeValue("MESHNAME", tmpMeshName);
                int tmpMeshID = 0;
                tmpNode->GetAttributeValueI("ID", tmpMeshID);
                CMString tmpMeshFile = "UnknownFile";
                tmpNode->GetAttributeValue("MESHFILE", tmpMeshFile);

                CMeshD3F *Mesh = new CMeshD3F(tmpMeshName.c_str(),this->OwnerEngine);
                int ResFile = fil->Find(tmpMeshFile);
                Mesh->Load(ResFile,Settings);
                OwnerEngine->GetMeshManager()->Add(tmpMeshID, *Mesh);
            }
        }
    }
    if (nodeTextures) {
        //Then load all textures
        for (UINT i=0; i<nodeTextures->GetSubNodeCount(); i++) {
            CXMLNode *tmpNode = nodeTextures->GetSubNode(i);
            if (strcmp(tmpNode->GetName().c_str(), "D3F_TEXTURE") == 0) {
                CMString tmpTexName = "UnknownTexture";
                tmpNode->GetAttributeValue("NAME", tmpTexName);
                int tmpTexID = 0;
                tmpNode->GetAttributeValueI("ID", tmpTexID);

                CTexture2D *tTex = new CTexture2D(tmpTexName.c_str(),OwnerEngine);
                tTex->SetResourceID(tmpTexID);
                int resindex = fil->Find(tmpTexName);
                tTex->Load(resindex,Settings);
                OwnerEngine->GetTextureManager()->Add(tmpTexID,*tTex);
            }
        }
    }
    CHashTable2 htable(false, 0);
    if (nodeMaterials) {
        //Create hashtable of material's nodes
        for (UINT i=0; i<nodeMaterials->GetSubNodeCount(); i++) {
            CXMLNode *tmpNode = nodeMaterials->GetSubNode(i);
            if (strcmp(tmpNode->GetName().c_str(), "D3F_MATERIAL") == 0) {
                UINT tmpObjID = 0;
                tmpNode->GetAttributeValueU("ID", tmpObjID);
                htable.Add(tmpObjID, tmpNode);
            }
        }
    }
    ///Create one material with ID D3_GRI_Mat_PixelPhong
    CDX9Material *MPhong = (CDX9Material *)OwnerEngine->GetMaterialManager()->Query(D3_GRI_Mat_PixelPhong);
    if (MPhong == NULL) {
        _FATAL(this, ErrMgr->TSystem, "No default material !!!");
    }

    if (nodeObjects) {
        //Then we load objects (and targets)
        for (UINT i=0; i<nodeObjects->GetSubNodeCount(); i++) {
            CXMLNode *tmpNode = nodeObjects->GetSubNode(i);
            if (strcmp(tmpNode->GetName().c_str(), "D3F_OBJECT") == 0) {
                CMString tmpObjName = "UnknownObject";
                tmpNode->GetAttributeValue("NAME", tmpObjName);
                int tmpObjID = 0;
                tmpNode->GetAttributeValueI("ID", tmpObjID);
                bool tmpIsTarget = false;
                tmpNode->GetAttributeValueB("ISTARGET", tmpIsTarget);
                if (!tmpIsTarget) {
                    CD3SObject *Object = new CD3SObject(tmpObjName.c_str(),this->OwnerEngine);
                    Object->SetResourceID(tmpObjID);
                    Object->LoadXML(tmpNode, Settings);
                    //Then assign materials to object / mesh parts
                    Object->AssignMesh(Object->MeshID);
                    CMesh *Msh = (CMesh *)OwnerEngine->GetMeshManager()->Find(Object->MeshID);
                    for (int i=0; i<Msh->GetSubsetCount(); i++) {
                        CMeshPart *MPart = Msh->GetSubset(i);
                        CXMLNode *tmpMatNode = (CXMLNode *)htable.Get(MPart->GetAppData());
                        CDX9Material *pMat = NULL;
                        MPhong->Clone(&pMat);
                        pMat->LoadXML(tmpMatNode, Settings);
                        Object->AssignMaterial(i,pMat->GetResourceID());
                    }
                    AddObject(0,*Object);
                } else {
                    CD3STarget *Object = new CD3STarget(tmpObjName.c_str(),this->OwnerEngine);
                    Object->SetResourceID(tmpObjID);
                    Object->LoadXML(tmpNode, Settings);
                    AddObject(0,*Object);
                }
            }
        }
    }
    if (nodeLights) {
        //Then load lights
        for (UINT i=0; i<nodeLights->GetSubNodeCount(); i++) {
            CXMLNode *tmpNode = nodeLights->GetSubNode(i);
            if (strcmp(tmpNode->GetName().c_str(), "D3F_LIGHT") == 0) {
                CMString tmpObjName = "UnknownLight";
                tmpNode->GetAttributeValue("NAME", tmpObjName);
                int tmpObjID = 0;
                tmpNode->GetAttributeValueI("ID", tmpObjID);
                CD3SLight *Object = new CD3SLight(tmpObjName.c_str(),this->OwnerEngine);
                Object->SetResourceID(tmpObjID);
                Object->LoadXML(tmpNode, Settings);
                AddLight(*Object);
                Object->Target = (CBase3DObject *)this->Objects->Get(Object->TargetID);
            }
        }
    }
    if (nodeCameras) {
        //Then cameras
        for (UINT i=0; i<nodeCameras->GetSubNodeCount(); i++) {
            CXMLNode *tmpNode = nodeCameras->GetSubNode(i);
            if (strcmp(tmpNode->GetName().c_str(), "D3F_CAMERA") == 0) {
                CMString tmpObjName = "UnknownCamera";
                tmpNode->GetAttributeValue("NAME", tmpObjName);
                int tmpObjID = 0;
                tmpNode->GetAttributeValueI("ID", tmpObjID);
                CD3SCamera *Object = new CD3SCamera(tmpObjName.c_str(),this->OwnerEngine);
                Object->SetResourceID(tmpObjID);
                Object->LoadXML(tmpNode, Settings);
                CurrentCamera = Object;
                AddCamera(*Object);
                Object->Target = (CBase3DObject *)this->Objects->Get(Object->TargetID);
            }
        }
    }
    //Then resolve parents
    UINT hRes = this->Objects->GetFirstItem();
    while (1) {
        CD3SObject *Object = (CD3SObject *)this->Objects->GetValueAtIndex(hRes);
        CD3SObject *Parent = (CD3SObject *)this->Objects->Get(Object->ParentID);
        Object->Parent = Parent;
        UINT hRes2 = this->Objects->GetNextItem(hRes);
        if (hRes2 == hRes) break;
        else hRes = hRes2;
    }

    if (nodeControllers) {
        //Then controllers
        for (UINT i=0; i<nodeControllers->GetSubNodeCount(); i++) {
            CXMLNode *tmpNode = nodeControllers->GetSubNode(i);
            if (strcmp(tmpNode->GetName().c_str(), "D3F_CONTROLLER") == 0) {
                int OwnerID = -1;
                tmpNode->GetAttributeValueI("OWNERID", OwnerID);
                CBase3DObject *Owner = (CBase3DObject *)this->Objects->Get(OwnerID);
                if (Owner) {
                    //CMString tmpName2 = Owner->GetObjName();
                    CMString tmpName = "Controller";
                    CPRSLinearController *Object = new CPRSLinearController(tmpName.c_str(),this->OwnerEngine);
                    if (Object->LoadXML(tmpNode, Settings)) {
                        Owner->PRSController = Object;
                    } else {
                        delete Object;
                    }
                }
            }
        }
    }

    //Unload unused material
    OwnerEngine->GetMaterialManager()->Delete(MPhong->GetResourceID());
    if (nodeTextures) {
        //Unload unused textures
        for (UINT i=0; i<nodeTextures->GetSubNodeCount(); i++) {
            CXMLNode *tmpNode = nodeTextures->GetSubNode(i);
            if (strcmp(tmpNode->GetName().c_str(), "D3F_TEXTURE") == 0) {
                int tmpTexID = 0;
                tmpNode->GetAttributeValueI("ID", tmpTexID);
                OwnerEngine->GetTextureManager()->Delete(tmpTexID);
            }
        }
    }
    if (nodeMeshes) {
        //Unload unused meshes
        for (UINT i=0; i<nodeMeshes->GetSubNodeCount(); i++) {
            CXMLNode *tmpNode = nodeMeshes->GetSubNode(i);
            if (strcmp(tmpNode->GetName().c_str(), "D3F_MESHLINK") == 0) {
                int tmpMeshID = 0;
                tmpNode->GetAttributeValueI("ID", tmpMeshID);
                OwnerEngine->GetMeshManager()->Delete(tmpMeshID);
            }
        }
    }
    return true;
}

bool CSimpleScene::Load(int LoadIndex, CRenderSettings &Settings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV2,"Loading Dimension3 XML Scene from resource %i",LoadIndex);
    bool result = true;
    CFileManager *fil = OwnerEngine->GetFileManager();
    int ResSize = fil->GetSize(LoadIndex);
    if (ResSize<0) {
        _WARN(this,ErrMgr->TFile,"Resource not found in library");
        result = false;
    } else {
        //------------------------------------
        //Stage 0 - Unload (possibly) used memory
        this->Lights->Clear(true);
        this->Cameras->Clear(true);
        this->Objects->Clear(true);
        //------------------------------------
        //Stage 1 - Load from media
        CXMLParser *Parser = new CXMLParser("SceneReader", OwnerEngine);
        CXMLNode *Node = Parser->Parse(LoadIndex);
        delete Parser;
        if (Node!=NULL) {   //loaded
            LoadXML(Node, Settings);
            delete Node;
        }
    }
    if (result) {
        _LOGE(logE,"Ending with success");
    } else {
        _LOGE(logE,"Ending with failure");
    }
    return result;
}

/*
bool CSimpleScene::Load(int LoadIndex, CRenderSettings &Settings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV2,"Loading Dimension3 BIN Scene from resource %i",LoadIndex);
    bool result = TRUE;
    CFileManager *fil = OwnerEngine->GetFileManager();
    int ResSize = fil->GetSize(LoadIndex);
    if (ResSize<0) {
        _WARN(this,ErrMgr->TFile,"Resource not found in library");
        result = FALSE;
    } else {
        //------------------------------------
        //Stage 0 - Unload (possibly) used memory
        this->Lights->Clear(true);
        this->Cameras->Clear(true);
        this->Objects->Clear(true);
        //------------------------------------
        //Stage 1 - Load from media
        int logEE = _LOGB(this,D3_DV_GRP2_LEV2,"Reading from disk",LoadIndex);
        char *Buffer = (char *)MemMgr->AllocMem(ResSize,0);
        CD3F_Scene *Scene = NULL;
        if (fil->Open(LoadIndex)) {
            if (fil->Load(LoadIndex, Buffer)>0) {    //load it
                DWORD Position = 0;
                Scene = new CD3F_Scene("D3F_Scene",this->OwnerEngine);
                Scene->LoadBIN(Buffer,ResSize,Position);
            } else {    
                _WARN(this,ErrMgr->TFile,"Resource could not be loaded");
                result = FALSE;
            }
            if (!fil->Close(LoadIndex)) {
                _WARN(this,ErrMgr->TFile,"Resource could not be closed");
                result = FALSE;
            }
        } else {
            _WARN(this,ErrMgr->TFile,"Resource could not be opened");
            result = FALSE;
        }
        MemMgr->FreeMem(Buffer);
        _LOGE(logEE);
        if (Scene!=NULL) {   //loaded
            for (int i=0; i<Scene->Meshes->GetSize(); i++) {
                CD3F_MeshLink *Link = (CD3F_MeshLink *)Scene->Meshes->GetAt(i);
                CMeshD3F *Mesh = new CMeshD3F(Link->MeshName.c_str(),this->OwnerEngine);
                CMString FileName = Link->MeshFile + CMString(".d3mesh");
                int ResFile = fil->Find(FileName);
                Mesh->Load(ResFile,Settings);
                OwnerEngine->GetMeshManager()->Add(Link->MeshID, *Mesh);
            }
            ///Create one material with ID D3_GRI_Mat_PixelPhong
            CBaseMaterial *MPhong = (CBaseMaterial *)OwnerEngine->GetMaterialManager()->Query(D3_GRI_Mat_PixelPhong);
            if (MPhong == NULL) {
                MPhong = new CMaterialPixelPhong("DefaultPhong",this->OwnerEngine);
                MPhong->SetResourceID(D3_GRI_Mat_PixelPhong);
                OwnerEngine->GetMaterialManager()->Add(MPhong->GetResourceID(),*MPhong);
            }

            UINT hRes = Scene->Objects->GetFirstItem();
            while (1) {
                CD3F_Object *SceneObj = (CD3F_Object *)Scene->Objects->GetValueAtIndex(hRes);
                if (!SceneObj->IsTarget) {
                    CD3SObject *Object = new CD3SObject(SceneObj->Name.c_str(),this->OwnerEngine);
                    Object->Translation.x = (float)SceneObj->LocalPivot.Pos.x;
                    Object->Translation.y = (float)SceneObj->LocalPivot.Pos.y;
                    Object->Translation.z = (float)SceneObj->LocalPivot.Pos.z;
                    Object->Rotation.x = (float)SceneObj->LocalPivot.Rot.x;
                    Object->Rotation.y = (float)SceneObj->LocalPivot.Rot.y;
                    Object->Rotation.z = (float)SceneObj->LocalPivot.Rot.z;
                    Object->Rotation.w = (float)SceneObj->LocalPivot.Rot.w;
                    Object->Scale.x = (float)SceneObj->LocalPivot.Scale.x;
                    Object->Scale.y = (float)SceneObj->LocalPivot.Scale.y;
                    Object->Scale.z = (float)SceneObj->LocalPivot.Scale.z;
                    Object->AssignMesh(SceneObj->MeshID);
                    Object->SetResourceID(SceneObj->ID);
                    //assign material
                    CMesh *Msh = (CMesh *)OwnerEngine->GetMeshManager()->Find(SceneObj->MeshID);
                    for (int i=0; i<Msh->GetSubsetCount(); i++) {
                        CMeshPart *MPart = Msh->GetSubset(i);
                        CD3F_Material *SMat = (CD3F_Material *)Scene->Materials->Get(MPart->GetAppData());
                        
                        CDX8MaterialContext *Context = (CDX8MaterialContext *)MPhong->CreateContext(Object);
                        int PropID;

                        //Ambient part of material
                        float AmbientMultiplier;
                        AmbientMultiplier = (FLOAT)SMat->m.AmbientCoeff;
                        Context->SetProperty(Context->QueryProperty("AmbientMultiplier"), &AmbientMultiplier);

                        D3DXVECTOR4 AmbientColor;
                        AmbientColor.x = (FLOAT)SMat->MtlAmbientColor.x;
                        AmbientColor.y = (FLOAT)SMat->MtlAmbientColor.y;
                        AmbientColor.z = (FLOAT)SMat->MtlAmbientColor.z;
                        AmbientColor.w = 1;
                        Context->SetProperty(Context->QueryProperty("AmbientColor"), &AmbientColor);

                        PropID = Context->QueryProperty("AmbientTexture");
                        if (PropID>=0) {
                            CBaseTexture *AmbientTexture = NULL;
                            if (SMat->m.AmbientMapID>=0) {
                                AmbientTexture = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(SMat->m.AmbientMapID);
                                if (AmbientTexture==NULL) {
                                    CD3F_Texture *Tex = (CD3F_Texture *)Scene->Textures->Get(SMat->m.AmbientMapID);
                                    CTexture2D *tTex = new CTexture2D(Tex->FileName.c_str(),OwnerEngine);
                                    tTex->SetResourceID(Tex->ID);
                                    int resindex = OwnerEngine->GetFileManager()->Find(Tex->FileName);
                                    tTex->Load(resindex,Settings);
                                    AmbientTexture = tTex;
                                    OwnerEngine->GetTextureManager()->Add(Tex->ID,*AmbientTexture);
                                }
                            }
                            Context->SetProperty(PropID, AmbientTexture);
                        }

                        //Self illumination (emisive part)
                        bool SelfIllumOn = (SMat->m.SelfIllumOn == TRUE) ? true : false;
                        Context->SetProperty(Context->QueryProperty("SelfIllumOn"), &SelfIllumOn);

                        D3DXVECTOR4 SelfIllumColor;
                        SelfIllumColor.x = (FLOAT)SMat->SelfIllumColor.x;
                        SelfIllumColor.y = (FLOAT)SMat->SelfIllumColor.y;
                        SelfIllumColor.z = (FLOAT)SMat->SelfIllumColor.z;
                        SelfIllumColor.w = 1;
                        Context->SetProperty(Context->QueryProperty("SelfIllumColor"), &SelfIllumColor);

                        float SelfIllumValue;
                        SelfIllumValue = (FLOAT)SMat->m.SelfIllumValue;
                        Context->SetProperty(Context->QueryProperty("SelfIllumValue"), &SelfIllumValue);

                        //Specular

                        float SpecularMultiplier = (FLOAT)SMat->m.SpecularCoeff;
                        Context->SetProperty(Context->QueryProperty("SpecularMultiplier"), &SpecularMultiplier);

                        D3DXVECTOR4 SpecularColor;
                        SpecularColor.x = (FLOAT)SMat->MtlSpecularColor.x;
                        SpecularColor.y = (FLOAT)SMat->MtlSpecularColor.y;
                        SpecularColor.z = (FLOAT)SMat->MtlSpecularColor.z;
                        SpecularColor.w = 1;
                        Context->SetProperty(Context->QueryProperty("SpecularColor"), &SpecularColor);

                        float SpecularShininess = (float)SMat->m.Shininess;
                        Context->SetProperty(Context->QueryProperty("SpecularShininess"), &SpecularShininess);

                        PropID = Context->QueryProperty("SpecularTexture");
                        if (PropID>=0) {
                            CBaseTexture *SpecularTexture = NULL;
                            if (SMat->m.SpecularMapID>=0) {
                                SpecularTexture = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(SMat->m.SpecularMapID);
                                if (SpecularTexture==NULL) {
                                    CD3F_Texture *Tex = (CD3F_Texture *)Scene->Textures->Get(SMat->m.SpecularMapID);
                                    CTexture2D *SpecTex = new CTexture2D(Tex->FileName.c_str(),OwnerEngine);
                                    SpecTex->SetResourceID(Tex->ID);
                                    int resindex = OwnerEngine->GetFileManager()->Find(Tex->FileName);
                                    SpecTex->Load(resindex,Settings);
                                    SpecularTexture = SpecTex;
                                    OwnerEngine->GetTextureManager()->Add(Tex->ID,*SpecularTexture);
                                }
                            }
                            Context->SetProperty(PropID, SpecularTexture);
                        }

                        PropID = Context->QueryProperty("NormalTexture");
                        if (PropID>=0) {
                            CBaseTexture *NormalTexture = NULL;
                            if (SMat->m.BumpMapID>=0) {
                                NormalTexture = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(SMat->m.BumpMapID);
                                if (NormalTexture == NULL) {
                                    CD3F_Texture *Tex = (CD3F_Texture *)Scene->Textures->Get(SMat->m.BumpMapID);
                                    CTexture2D *NormTex = new CTexture2D(Tex->FileName.c_str(),OwnerEngine);
                                    NormTex->SetResourceID(Tex->ID);
                                    int resindex = OwnerEngine->GetFileManager()->Find(Tex->FileName);
                                    NormTex->Load(resindex,Settings);
                                    //NormTex->ConvertToNormalMap(Settings);
                                    NormalTexture = NormTex;
                                    OwnerEngine->GetTextureManager()->Add(Tex->ID,*NormalTexture);
                                }
                            }
                            Context->SetProperty(PropID, NormalTexture);
                        }

                        //Diffuse component
                        float DiffuseMultiplier = (FLOAT)SMat->m.DiffuseCoeff;
                        Context->SetProperty(Context->QueryProperty("DiffuseMultiplier"), &DiffuseMultiplier);

                        D3DXVECTOR4 DiffuseColor;
                        DiffuseColor.x = (FLOAT)SMat->MtlDiffuseColor.x;
                        DiffuseColor.y = (FLOAT)SMat->MtlDiffuseColor.y;
                        DiffuseColor.z = (FLOAT)SMat->MtlDiffuseColor.z;
                        DiffuseColor.w = 1;
                        Context->SetProperty(Context->QueryProperty("DiffuseColor"), &DiffuseColor);

                        PropID = Context->QueryProperty("DiffuseTexture");
                        if (PropID>=0) {
                            CBaseTexture *DiffuseTexture = NULL;
                            if (SMat->m.DiffuseMapID>=0) {
                                DiffuseTexture = (CBaseTexture *)OwnerEngine->GetTextureManager()->Query(SMat->m.DiffuseMapID);
                                if (DiffuseTexture==NULL) {
                                    CD3F_Texture *Tex = (CD3F_Texture *)Scene->Textures->Get(SMat->m.DiffuseMapID);
                                    CTexture2D *DiffTex = new CTexture2D(Tex->FileName.c_str(),OwnerEngine);
                                    DiffTex->SetResourceID(Tex->ID);
                                    int resindex = OwnerEngine->GetFileManager()->Find(Tex->FileName);
                                    DiffTex->Load(resindex,Settings);
                                    DiffuseTexture = DiffTex;
                                    OwnerEngine->GetTextureManager()->Add(Tex->ID,*DiffuseTexture);
                                }
                            }
                            Context->SetProperty(PropID, DiffuseTexture);
                        }
                        Object->AssignMaterial(i,MPhong->GetResourceID(),Context);
                    }
                    this->AddObject(0,*Object);
                } else {    //special processing for targets
                    CD3STarget *Target = new CD3STarget(SceneObj->Name.c_str(),this->OwnerEngine);
                    Target->SetResourceID(SceneObj->ID);
                    Target->Translation.x = (float)SceneObj->LocalPivot.Pos.x;
                    Target->Translation.y = (float)SceneObj->LocalPivot.Pos.y;
                    Target->Translation.z = (float)SceneObj->LocalPivot.Pos.z;
                    Target->Rotation.x = (float)SceneObj->LocalPivot.Rot.x;
                    Target->Rotation.y = (float)SceneObj->LocalPivot.Rot.y;
                    Target->Rotation.z = (float)SceneObj->LocalPivot.Rot.z;
                    Target->Rotation.w = (float)SceneObj->LocalPivot.Rot.w;
                    Target->Scale.x = (float)SceneObj->LocalPivot.Scale.x;
                    Target->Scale.y = (float)SceneObj->LocalPivot.Scale.y;
                    Target->Scale.z = (float)SceneObj->LocalPivot.Scale.z;
                    this->AddObject(0,*Target);
                }
                UINT hRes2 = Scene->Objects->GetNextItem(hRes);
                if (hRes2 == hRes) break;
                else hRes = hRes2;
            }

            hRes = Scene->Cameras->GetFirstItem();
            while (1) {
                CD3F_Camera *SceneObj = (CD3F_Camera *)Scene->Cameras->GetValueAtIndex(hRes);
                CD3SCamera *Camera = new CD3SCamera(SceneObj->Name.c_str(),this->OwnerEngine);
                Camera->SetResourceID(SceneObj->ID);
                Camera->Translation.x = (float)SceneObj->LocalPivot.Pos.x;
                Camera->Translation.y = (float)SceneObj->LocalPivot.Pos.y;
                Camera->Translation.z = (float)SceneObj->LocalPivot.Pos.z;
                Camera->Rotation.x = (float)SceneObj->LocalPivot.Rot.x;
                Camera->Rotation.y = (float)SceneObj->LocalPivot.Rot.y;
                Camera->Rotation.z = (float)SceneObj->LocalPivot.Rot.z;
                Camera->Rotation.w = (float)SceneObj->LocalPivot.Rot.w;
                Camera->Scale.x = (float)SceneObj->LocalPivot.Scale.x;
                Camera->Scale.y = (float)SceneObj->LocalPivot.Scale.y;
                Camera->Scale.z = (float)SceneObj->LocalPivot.Scale.z;

                Camera->Ortho = SceneObj->Ortho;
                Camera->FOV = SceneObj->FOV;
                Camera->NearClip = SceneObj->NearClip;
                Camera->FarCLip = SceneObj->FarCLip;
                Camera->Target = (CBase3DObject *)this->Objects->Get(SceneObj->TargetID);
                this->AddCamera(*Camera);
                CurrentCamera = Camera;
                UINT hRes2 = Scene->Cameras->GetNextItem(hRes);
                if (hRes2 == hRes) break;
                else hRes = hRes2;
            }

            hRes = Scene->Lights->GetFirstItem();
            while (1) {
                CD3F_Light *SceneObj = (CD3F_Light *)Scene->Lights->GetValueAtIndex(hRes);
                CD3SLight *Light = new CD3SLight(SceneObj->Name.c_str(),this->OwnerEngine);
                Light->SetResourceID(SceneObj->ID);
                Light->Translation.x = (float)SceneObj->LocalPivot.Pos.x;
                Light->Translation.y = (float)SceneObj->LocalPivot.Pos.y;
                Light->Translation.z = (float)SceneObj->LocalPivot.Pos.z;
                Light->Rotation.x = (float)SceneObj->LocalPivot.Rot.x;
                Light->Rotation.y = (float)SceneObj->LocalPivot.Rot.y;
                Light->Rotation.z = (float)SceneObj->LocalPivot.Rot.z;
                Light->Rotation.w = (float)SceneObj->LocalPivot.Rot.w;
                Light->Scale.x = (float)SceneObj->LocalPivot.Scale.x;
                Light->Scale.y = (float)SceneObj->LocalPivot.Scale.y;
                Light->Scale.z = (float)SceneObj->LocalPivot.Scale.z;

                Light->Type = SceneObj->Type;
                Light->Hotspot = SceneObj->Hotspot;
                Light->Intensity = SceneObj->Intensity;
                Light->Contrast = SceneObj->Contrast;
                Light->UseAttenuationNear = SceneObj->UseAttenuationNear;
                Light->UseAttenuationFar = SceneObj->UseAttenuationFar;
                Light->AttenuationNearStart = SceneObj->AttenuationNearStart;
                Light->AttenuationNearEnd = SceneObj->AttenuationNearEnd;
                Light->AttenuationFarStart = SceneObj->AttenuationFarStart;
                Light->AttenuationFarEnd = SceneObj->AttenuationFarEnd;
                if ((Light->AttenuationNearStart == 0) || (Light->AttenuationNearEnd == 0)) {
                    double tmp = max(Light->AttenuationNearStart, Light->AttenuationNearEnd);
                    if (tmp == 0) tmp = 0.01f;
                    Light->AttenuationNearStart = tmp;
                    Light->AttenuationNearEnd = tmp;
                }
                if ((Light->AttenuationFarStart == 0) || (Light->AttenuationFarEnd == 0)) {
                    double tmp = max(Light->AttenuationFarStart, Light->AttenuationFarEnd);
                    if (tmp == 0) tmp = 10000.0f;
                    Light->AttenuationFarStart = tmp;
                    Light->AttenuationFarEnd = tmp;
                }

                Light->DecayType = SceneObj->DecayType;
                Light->LightColor.r = (float)SceneObj->LightColor.x;
                Light->LightColor.g = (float)SceneObj->LightColor.y;
                Light->LightColor.b = (float)SceneObj->LightColor.z;
                Light->Target = (CBase3DObject *)this->Objects->Get(SceneObj->TargetID);
                this->AddLight(*Light);
                UINT hRes2 = Scene->Lights->GetNextItem(hRes);
                if (hRes2 == hRes) break;
                else hRes = hRes2;
            }

            //In another loop resolve parents.
            hRes = Scene->Objects->GetFirstItem();
            while (1) {
                CD3F_Object *SceneObj = (CD3F_Object *)Scene->Objects->GetValueAtIndex(hRes);
                if ((!SceneObj->IsTarget) && (SceneObj->ParentID>0)) {
                    CD3SObject *Object = (CD3SObject *)this->Objects->Get(SceneObj->ID);
                    CD3SObject *Parent = (CD3SObject *)this->Objects->Get(SceneObj->ParentID);
                    Object->Parent = Parent;
                } else {    //special processing for targets
                }
                UINT hRes2 = Scene->Objects->GetNextItem(hRes);
                if (hRes2 == hRes) break;
                else hRes = hRes2;
            }
            //Delete allocated phong material
            OwnerEngine->GetMaterialManager()->Delete(MPhong->GetResourceID());
        }

        //In the end of all release loaded meshes - only assigned will remain active
        for (int i=0; i<Scene->Meshes->GetSize(); i++) {
            CD3F_MeshLink *Link = (CD3F_MeshLink *)Scene->Meshes->GetAt(i);
            OwnerEngine->GetMeshManager()->Delete(Link->MeshID);
        }
        delete Scene;
    }
    if (result) {
        _LOGE(logE,"Ending with success");
    } else {
        _LOGE(logE,"Ending with failure");
    }
    return result;
}
*/