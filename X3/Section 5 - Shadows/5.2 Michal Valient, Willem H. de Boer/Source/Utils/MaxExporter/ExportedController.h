#pragma once
#include "stdafx.h"
#include "exportedentity.h"

class CControllerLinear : public CExportedEntity {
public:
    virtual void SaveToXML(CStdioFile *File, CString *Tag, CString *Indent);
};

class CControllerTCB : public CExportedEntity {
public:
    float   Tension;
    float   Continuity;
    float   Bias;
    float   EaseTo;
    float   EaseFrom;

    virtual void SaveToXML(CStdioFile *File, CString *Tag, CString *Indent);
};

class CControllerBezier : public CExportedEntity {
public:
    float   TangentIn[3];
    float   TangentOut[3];

    virtual void SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent);
};

class CValueFloat : public CExportedEntity {
public:
    float   Value;

    virtual void SaveToXML(CStdioFile *File, CString *Tag, CString *Indent);
};

class CValuePoint : public CExportedEntity {
public:
    float   Value[3];

    virtual void SaveToXML(CStdioFile *File, CString *Tag, CString *Indent);
};

class CValueQuaterion : public CExportedEntity {
public:
    float   Value[4];

    virtual void SaveToXML(CStdioFile *File, CString *Tag, CString *Indent);
};

class CValueScale : public CExportedEntity {
public:
    float   Scale[3];
    float   Quat[4];

    virtual void SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent);
};

class CValuePR : public CExportedEntity {
public:
    float   Position[3];
    float   Rotation[4];

    virtual void SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent);
};

class CValuePRS : public CExportedEntity {
public:
    float   Position[3];
    float   Rotation[4];
    float   Scale[3];

    virtual void SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent);
};

class CValueVertexCloud : public CExportedEntity {
public:
    int     Count;          //number of vertexes
    float   *Values;        //array of (x,y,z) values. Size = Count * 3;

    CValueVertexCloud() {Values = NULL; }
    virtual ~CValueVertexCloud() { if (Values) free(Values); }

    virtual void SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent);
    virtual void SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent);
};

class CExportedKeyFrame : public CExportedEntity {
public:
    int                 KeyFrameIndex;      //Index of the keyframe
    int                 Time;               //Time of the keyframe
    CExportedEntity    *Controller;         //Controller
    CExportedEntity    *Value;              //Value for controller

    CExportedKeyFrame() { Controller = Value = NULL; }
    virtual ~CExportedKeyFrame() {
        if (Controller) delete Controller;
        if (Value) delete Value;
    }

    virtual void SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent);
    virtual void SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent);
};

class CExportedController : public CExportedEntity {
public:
    CString             Field;              //Which field if affected with controller
    int                 ID;                 //ID of the controller
    int                 OwnerID;            //Owner object of the controller
    CString             Type;               //Keyframes, Sampling, Morphing
    CString             Controller;         //Name of the controller class
    CString             Value;              //Name of the value class

    CObArray            Keyframes;          //Keyframes

    CExportedController(void);
    virtual ~CExportedController(void);

    virtual void SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent);
    virtual void SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent);

    void CreateFromFlexporter(const ControllerDescriptor& Ctl);
};
