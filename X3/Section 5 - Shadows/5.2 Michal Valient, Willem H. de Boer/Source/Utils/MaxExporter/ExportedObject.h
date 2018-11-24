#pragma once
#include "afx.h"
#include "exportedentity.h"

class PosRotScale : public CExportedEntity {
public:
    float Position[3];      //x,y,z position
    float Rotation[4];      //x,y,z,w rotation quarterion
    float Scale[3];         //x,y,z scale

    //Parts of XML export - save starting tag with attributes
    virtual void SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent);

    //Parts of XML export - save inner nodes (subnodes) and tag text
    virtual void SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent);
};

class CExportedObject : public CExportedEntity {
public:
    CString         Name;           //Name of the object
    int             ID;             //ID of this object
    int             ParentID;       //ID of the parent (or -1)
    int             MasterID;       //ID of master (if is instance of something)
    int             TargetID;       //ID of the target (if camera or spotlight)
    int             MeshID;         //ID of the mesh. Defaults to -1 and is not touched by CreateFromXXX method (must be set by exporter).
    PosRotScale     PRS;            //Position, rotation and scale
    int             LocalPRS;       //1 for local PRS, 0 for absolute PRS
    int             IsTarget;       //1 if object is target (of camera, light) 0 if not. Defaults to 0 and is not touched by CreateFromXXX method (must be set by exporter).
    int             IsInstance;     //1 if object is instance, 0 otherwise
    PosRotScale     LocalPivot;     //Local position pivot
    PosRotScale     WorldPivot;     //World position pivot

    CExportedObject(void);
    virtual ~CExportedObject(void);

    virtual void CreateFromFlexporter(const ObjectDescriptor& Obj);
    
    //Parts of XML export - save starting tag with attributes
    virtual void SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent);

    //Parts of XML export - save inner nodes (subnodes) and tag text
    virtual void SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent);
};
