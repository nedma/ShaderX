//
//This is exporter to dimension3 scene format
#include "Stdafx.h"
#include "VoidExporter.h"
#include "exportedmesh.h"
#include "exportedobject.h"
#include "exportedlight.h"
#include "exportedcamera.h"
#include "exportedmaterial.h"
#include "exportedtexture.h"
#include "exportedcontroller.h"

static Dimension3Scene		gDimension3Scene;		// Global exporter instance
static ExportSettings	gSettings;			// Global export settings

// FLEXPORTER Identification Callbacks

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Flexporter callback.
 *	Gives a brief exporter description. That string is displayed inside MAX, in the Options Panel.
 *	\relates	Dimension3Scene
 *	\fn			ExporterDescription()
 *	\return		a description string
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* ExporterDescription() { 
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return "Dimension3 Exporter V2";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Flexporter callback.
 *	Returns the format's extension. That string is displayed inside MAX, in the Options Panel.
 *	\relates	Dimension3Scene
 *	\fn			FormatExtension()
 *	\return		an extension string
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* FormatExtension() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return "";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Flexporter callback.
 *	Returns the one and only exporter instance.
 *	\relates	Dimension3Scene
 *	\fn			GetExporter()
 *	\return		pointer to the global exporter instance.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ExportFormat* GetExporter() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return &gDimension3Scene;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Flexporter callback.
 *	Returns the default exportation settings for this format. This is the right place to initialize the default settings for your format.
 *	\relates	Dimension3Scene
 *	\fn			GetDefaultSettings()
 *	\return		pointer to the global ExportSettings instance.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ExportSettings* GetDefaultSettings() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return &gSettings;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Flexporter callback.
 *	Returns the enabled/disabled status for all settings. This is the right place to disable options your own format doesn't support.
 *	\relates	Dimension3Scene
 *	\fn			GetEnabledSettings()
 *	\return		pointer to a global Enabled instance.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Enabled* GetEnabledSettings() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    static Enabled Settings;
	return &Settings;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Flexporter callback.
 *	Returns the FLEXPORTER SDK Version.
 *	\relates	Dimension3Scene
 *	\fn			Version()
 *	\return		FLEXPORTER_VERSION
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Version() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return FLEXPORTER_VERSION;
}









///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A void exporter plug-in for Flexporter..
 *	\class		Dimension3Scene
 *	\author		Pierre Terdiman
 *	\version	1.0
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Constructor.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Dimension3Scene::Dimension3Scene() {
    AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);

    Meshes = new CMapWordToOb();
    Objects = new CMapWordToOb();
    Cameras = new CMapWordToOb();
    Lights = new CMapWordToOb();
    Textures = new CMapWordToOb();
    Materials = new CMapWordToOb();
    Controllers = new CMapWordToOb();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Destructor.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Dimension3Scene::~Dimension3Scene() {
    POSITION pos = Meshes->GetStartPosition();
    while (pos!=NULL) {
        WORD Key;
        CObject *Obj;
        Meshes->GetNextAssoc(pos, Key, Obj);
        if (Obj!=NULL) delete Obj;
    }
    delete Meshes;

    pos = Objects->GetStartPosition();
    while (pos!=NULL) {
        WORD Key;
        CObject *Obj;
        Objects->GetNextAssoc(pos, Key, Obj);
        if (Obj!=NULL) delete Obj;
    }
    delete Objects;

    pos = Cameras->GetStartPosition();
    while (pos!=NULL) {
        WORD Key;
        CObject *Obj;
        Cameras->GetNextAssoc(pos, Key, Obj);
        if (Obj!=NULL) delete Obj;
    }
    delete Cameras;

    pos = Lights->GetStartPosition();
    while (pos!=NULL) {
        WORD Key;
        CObject *Obj;
        Lights->GetNextAssoc(pos, Key, Obj);
        if (Obj!=NULL) delete Obj;
    }
    delete Lights;

    pos = Textures->GetStartPosition();
    while (pos!=NULL) {
        WORD Key;
        CObject *Obj;
        Textures->GetNextAssoc(pos, Key, Obj);
        if (Obj!=NULL) delete Obj;
    }
    delete Textures;

    pos = Materials->GetStartPosition();
    while (pos!=NULL) {
        WORD Key;
        CObject *Obj;
        Materials->GetNextAssoc(pos, Key, Obj);
        if (Obj!=NULL) delete Obj;
    }
    delete Materials;

    pos = Controllers->GetStartPosition();
    while (pos!=NULL) {
        WORD Key;
        CObject *Obj;
        Controllers->GetNextAssoc(pos, Key, Obj);
        if (Obj!=NULL) delete Obj;
    }
    delete Controllers;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Plug-in initialization method.
 *	This method is called once before each export. When this method is called, the mSettings and mFilename members of the base format are valid.
 *	\param		motion		[in] true for pure motion files.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Dimension3Scene::Init(bool motion) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Scene export method.
 *	This method is called once to export the scene global information.
 *	\param		maininfo	[in] main info structure
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Dimension3Scene::SetSceneInfo(const MainDescriptor& maininfo) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    memcpy(&SceneInformations, &maininfo, sizeof(MainDescriptor));
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Controller export method.
 *	This method is called once for each exported controller.
 *	\param		controller		[in] a structure filled with current controller information.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Dimension3Scene::ExportController(const ControllerDescriptor& controller) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CExportedController *ExpObj = new CExportedController;
    ExpObj->CreateFromFlexporter(controller);
    Controllers->SetAt(controller.mObjectID, ExpObj);
    CString Tmp;
    Tmp.Format("Adding Controller %s; ID %i", controller.mField, ExpObj->ID);
    AddLogString((LPCTSTR)Tmp);
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Helper export method.
 *	This method is called once for each exported helper.
 *	\param		helper		[in] a structure filled with current helper information.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Dimension3Scene::ExportHelper(const HelperDescriptor& helper) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Light export method.
 *	This method is called once for each exported light.
 *	\param		light		[in] a structure filled with current light information.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Dimension3Scene::ExportLight(const LightDescriptor& light) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CExportedLight *ExpObj = new CExportedLight;
    ExpObj->CreateFromFlexporter(light);
    Lights->SetAt(light.mObjectID, ExpObj);
    CString Tmp;
    Tmp.Format("Adding Light %s; ID %i", light.mName, ExpObj->ID);
    AddLogString((LPCTSTR)Tmp);
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Material export method.
 *	This method is called once for each exported material.
 *	\param		material		[in] a structure filled with current material information.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Dimension3Scene::ExportMaterial(const MaterialDescriptor& material) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CExportedMaterial *ExpObj = new CExportedMaterial;
    ExpObj->CreateFromFlexporter(material);
    Materials->SetAt(material.mObjectID, ExpObj);
    CString Tmp;
    Tmp.Format("Adding Material %s; ID %i", material.mName, ExpObj->ID);
    AddLogString((LPCTSTR)Tmp);
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Camera export method.
 *	This method is called once for each exported camera.
 *	\param		camera		[in] a structure filled with current camera information.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Dimension3Scene::ExportCamera(const CameraDescriptor& camera) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CExportedCamera *ExpObj = new CExportedCamera;
    ExpObj->CreateFromFlexporter(camera);
    Cameras->SetAt(camera.mObjectID, ExpObj);
    CString Tmp;
    Tmp.Format("Adding Camera %s; ID %i", camera.mName, ExpObj->ID);
    AddLogString((LPCTSTR)Tmp);
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Mesh export method.
 *	This method is called once for each exported mesh.
 *	\param		mesh		[in] a structure filled with current mesh information.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Dimension3Scene::ExportMesh(const MeshDescriptor& mesh) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (mesh.mIsTarget) {
        CString Tmp;
        Tmp.Format("Object %s is target - not saving BIN version", mesh.mName);
        AddLogString((LPCTSTR)Tmp);
    } else if (mesh.mIsInstance) {
        CString Tmp;
        Tmp.Format("Object %s is instance - not saving BIN version", mesh.mName);
        AddLogString((LPCTSTR)Tmp);
    } else {
        CExportedMesh *ExpMesh = new CExportedMesh;
        ExpMesh->CreateFromFlexporter(mesh);
        CString FileName;
        FileName.Format("%s%s.d3mesh", mFilename, ExpMesh->Name);
        ExpMesh->Save(&FileName);
        FileName.Format("Saving Binary mesh to %s%s.d3mesh\n", mFilename, ExpMesh->Name);
        AddLogString((LPCTSTR)FileName);

        if (mesh.mObjectID>0xFFFF) {
            CString tmp;
            tmp.Format("ERROR : Mesh '%s' has ID bigger than WORD %u > 0xFFFF\n",mesh.mName, mesh.mObjectID);
            AddLogString((LPCTSTR)FileName);
        }
        Meshes->SetAt(mesh.mObjectID, ExpMesh);
    }

    CExportedObject *ExpObj = new CExportedObject;
    ExpObj->CreateFromFlexporter(mesh);
    ExpObj->IsTarget = mesh.mIsTarget;
    if (mesh.mIsInstance) {
        CObject *Master;
        Objects->Lookup(mesh.mMasterID, (CObject *)Master);
        ExpObj->MeshID = ((CExportedObject *)Master)->MeshID;
    } else {
        ExpObj->MeshID = mesh.mObjectID;
    }
    Objects->SetAt(mesh.mObjectID, ExpObj);
    CString Tmp;
    Tmp.Format("Adding Object %s; ID %i; MeshID %i", mesh.mName, ExpObj->ID, ExpObj->MeshID);
    AddLogString((LPCTSTR)Tmp);
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Motion export method.
 *	This method is called once for each exported motion.
 *	\param		motion		[in] a structure filled with current motion information.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Dimension3Scene::ExportMotion(const MotionDescriptor& motion) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Shape export method.
 *	This method is called once for each exported shape.
 *	\param		shape		[in] a structure filled with current shape information.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Dimension3Scene::ExportShape(const ShapeDescriptor& shape) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Texture export method.
 *	This method is called once for each exported texture.
 *	\param		texture		[in] a structure filled with current texture information.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Dimension3Scene::ExportTexture(const TextureDescriptor& texture) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CExportedTexture *ExpObj = new CExportedTexture;
    ExpObj->CreateFromFlexporter(texture);
    Textures->SetAt(texture.mObjectID, ExpObj);
    CString Tmp;
    Tmp.Format("Adding Texture %s; ID %i", texture.mName, ExpObj->ID);
    AddLogString((LPCTSTR)Tmp);
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	End of export notification.
 *	This method is called once all nodes have been exported. This is a convenient place to free all used ram, etc.
 *	\param		stats		[in] a structure filled with some export statistics.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Dimension3Scene::EndExport(const StatsDescriptor& stats) {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CString tmp;
    CString Indent = "    ";
    tmp.Format("%sxml",mFilename);
    CStdioFile File((LPCTSTR)tmp, CFile::modeCreate | CFile::modeWrite | CFile::typeText);

    File.WriteString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    tmp.Format("<D3F_SCENE\n");
    File.WriteString((LPCTSTR)tmp);

    SaveAttrib(&File, Indent, "Version", SceneVersion);
    SaveAttrib(&File, Indent, "StartTime", SceneInformations.mTime.mStartTime);
    SaveAttrib(&File, Indent, "EndTime", SceneInformations.mTime.mEndTime);
    SaveAttrib(&File, Indent, "TicksPerFrame", SceneInformations.mTime.mDeltaTime);
    SaveAttrib(&File, Indent, "FrameRate", SceneInformations.mTime.mFrameRate);
    SaveAttrib(&File, Indent, "CurrentTime", SceneInformations.mTime.mCurrentTime);
    SaveAttrib(&File, Indent, "FrameCount", SceneInformations.mTime.mNbFrames);

    SaveAttrib(&File, Indent, "ExpectedNodes", SceneInformations.mNbNodes);
    SaveAttrib(&File, Indent, "ExpectedCharacters", SceneInformations.mNbCharacters);
    SaveAttrib(&File, Indent, "GlobalScale", SceneInformations.mGlobalScale);

    tmp.Format(">\n");
    File.WriteString((LPCTSTR)tmp);

    SaveAttribXYZ(&File, Indent, "BackgroundColor", (float *)&SceneInformations.mBackColor);
    SaveAttribXYZ(&File, Indent, "AmbientColor", (float *)&SceneInformations.mAmbColor);
    tmp.Format("    <INFO><![CDATA[%s]]></INFO>\n", SceneInformations.mSceneInfos);
    File.WriteString((LPCTSTR)tmp);

    File.WriteString("\n\n");

    tmp.Format("    <D3F_MESHLINKS COUNT=\"%i\">\n", Meshes->GetCount());
    File.WriteString((LPCTSTR)tmp);
    POSITION pos = Meshes->GetStartPosition();
    while (pos!=NULL) {
        WORD Key;
        CObject *Obj;
        Meshes->GetNextAssoc(pos, Key, Obj);
        if (Obj!=NULL) {
            CExportedMesh *Mesh = (CExportedMesh *)Obj;
            CString tmpMeshFile = Mesh->FileName.Mid(Mesh->FileName.ReverseFind('\\')+1);
            tmp.Format("        <D3F_MESHLINK ID=\"%i\" MESHNAME=\"%s\" MESHFILE=\"%s\"/>\n", Key, Mesh->Name, (LPCTSTR)tmpMeshFile);
            File.WriteString((LPCTSTR)tmp);
        }
    }
    tmp.Format("    </D3F_MESHLINKS>\n");
    File.WriteString((LPCTSTR)tmp);

    Indent = "        ";

    File.WriteString("\n\n");

    tmp.Format("    <D3F_OBJECTS COUNT=\"%i\">\n", Objects->GetCount());
    File.WriteString((LPCTSTR)tmp);
    pos = Objects->GetStartPosition();
    while (pos!=NULL) {
        WORD Key;
        CObject *Obj;
        Objects->GetNextAssoc(pos, Key, Obj);
        if (Obj!=NULL) {
            CExportedEntity *Object = (CExportedEntity *)Obj;
            CString Tag = "D3F_OBJECT";
            Object->SaveToXML(&File, &Tag, &Indent);
        }
    }
    tmp.Format("    </D3F_OBJECTS>\n");
    File.WriteString((LPCTSTR)tmp);

    File.WriteString("\n\n");

    tmp.Format("    <D3F_LIGHTS COUNT=\"%i\">\n", Lights->GetCount());
    File.WriteString((LPCTSTR)tmp);
    pos = Lights->GetStartPosition();
    while (pos!=NULL) {
        WORD Key;
        CObject *Obj;
        Lights->GetNextAssoc(pos, Key, Obj);
        if (Obj!=NULL) {
            CExportedEntity *Object = (CExportedEntity *)Obj;
            CString Tag = "D3F_LIGHT";
            Object->SaveToXML(&File, &Tag, &Indent);
        }
    }
    tmp.Format("    </D3F_LIGHTS>\n");
    File.WriteString((LPCTSTR)tmp);

    File.WriteString("\n\n");

    tmp.Format("    <D3F_CAMERAS COUNT=\"%i\">\n", Cameras->GetCount());
    File.WriteString((LPCTSTR)tmp);
    pos = Cameras->GetStartPosition();
    while (pos!=NULL) {
        WORD Key;
        CObject *Obj;
        Cameras->GetNextAssoc(pos, Key, Obj);
        if (Obj!=NULL) {
            CExportedEntity *Object = (CExportedEntity *)Obj;
            CString Tag = "D3F_CAMERA";
            Object->SaveToXML(&File, &Tag, &Indent);
        }
    }
    tmp.Format("    </D3F_CAMERAS>\n");
    File.WriteString((LPCTSTR)tmp);

    File.WriteString("\n\n");

    tmp.Format("    <D3F_MATERIALS COUNT=\"%i\">\n", Materials->GetCount());
    File.WriteString((LPCTSTR)tmp);
    pos = Materials->GetStartPosition();
    while (pos!=NULL) {
        WORD Key;
        CObject *Obj;
        Materials->GetNextAssoc(pos, Key, Obj);
        if (Obj!=NULL) {
            CExportedEntity *Object = (CExportedEntity *)Obj;
            CString Tag = "D3F_MATERIAL";
            Object->SaveToXML(&File, &Tag, &Indent);
        }
    }
    tmp.Format("    </D3F_MATERIALS>\n");
    File.WriteString((LPCTSTR)tmp);

    File.WriteString("\n\n");

    tmp.Format("    <D3F_TEXTURES COUNT=\"%i\">\n", Textures->GetCount());
    File.WriteString((LPCTSTR)tmp);
    pos = Textures->GetStartPosition();
    while (pos!=NULL) {
        WORD Key;
        CObject *Obj;
        Textures->GetNextAssoc(pos, Key, Obj);
        if (Obj!=NULL) {
            CExportedEntity *Object = (CExportedEntity *)Obj;
            CString Tag = "D3F_TEXTURE";
            Object->SaveToXML(&File, &Tag, &Indent);
        }
    }
    tmp.Format("    </D3F_TEXTURES>\n");
    File.WriteString((LPCTSTR)tmp);


    tmp.Format("    <D3F_CONTROLLERS COUNT=\"%i\">\n", Controllers->GetCount());
    File.WriteString((LPCTSTR)tmp);
    pos = Controllers->GetStartPosition();
    while (pos!=NULL) {
        WORD Key;
        CObject *Obj;
        Controllers->GetNextAssoc(pos, Key, Obj);
        if (Obj!=NULL) {
            CExportedEntity *Object = (CExportedEntity *)Obj;
            CString Tag = "D3F_CONTROLLER";
            Object->SaveToXML(&File, &Tag, &Indent);
        }
    }
    tmp.Format("    </D3F_CONTROLLERS>\n");
    File.WriteString((LPCTSTR)tmp);

    tmp.Format("</D3F_SCENE>\n");
    File.WriteString((LPCTSTR)tmp);
    return true;
}

// About box
void Dimension3Scene::About() {
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IceCore::MessageBox(null, "Dimension3 Scene exporter V2", "About...", MB_OK);
}
