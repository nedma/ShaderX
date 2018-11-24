#include "exportedcontroller.h"


void CControllerLinear::SaveToXML(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        tmp.Format("%s<%s/>\n", (LPCTSTR)IndentTag, (LPCTSTR)*Tag);
        File->WriteString((LPCTSTR)tmp);
    }
}

void CControllerTCB::SaveToXML(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        tmp.Format("%s<%s  TENSION=\"%g\"  CONTINUITY=\"%g\"  BIAS=\"%g\"  EASETO=\"%g\"  EASEFROM=\"%g\" />\n", (LPCTSTR)IndentTag, (LPCTSTR)*Tag, Tension, Continuity, Bias, EaseTo, EaseFrom);
        File->WriteString((LPCTSTR)tmp);
    }
}

void CControllerBezier::SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        IndentAttribs = IndentAttribs + "    ";
    }
    SaveAttribXYZ(File, IndentAttribs, "TANGENT_IN", TangentIn);
    SaveAttribXYZ(File, IndentAttribs, "TANGENT_OUT", TangentOut);
}

void CValueFloat::SaveToXML(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        tmp.Format("%s<%s  X=\"%g\" />\n", (LPCTSTR)IndentTag, (LPCTSTR)*Tag, Value);
        File->WriteString((LPCTSTR)tmp);
    }
}

void CValuePoint::SaveToXML(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    SaveAttribXYZ(File, IndentTag, (char *)Tag->GetString(), Value);
}

void CValueQuaterion::SaveToXML(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    SaveAttribXYZW(File, IndentTag, (char *)Tag->GetString(), Value);
}


void CValueScale::SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        IndentAttribs = IndentAttribs + "    ";
    }
    SaveAttribXYZ(File, IndentAttribs, "SCALE", Scale);
    SaveAttribXYZW(File, IndentAttribs, "QUAT", Quat);
}

void CValuePR::SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        IndentAttribs = IndentAttribs + "    ";
    }
    SaveAttribXYZ(File, IndentAttribs, "POSITION", Position);
    SaveAttribXYZW(File, IndentAttribs, "ROTATION", Rotation);
}

void CValuePRS::SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        IndentAttribs = IndentAttribs + "    ";
    }
    SaveAttribXYZ(File, IndentAttribs, "POSITION", Position);
    SaveAttribXYZW(File, IndentAttribs, "ROTATION", Rotation);
    SaveAttribXYZ(File, IndentAttribs, "SCALE", Scale);
}

void CValueVertexCloud::SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        tmp.Format("%s<%s\n", (LPCTSTR)IndentTag, (LPCTSTR)*Tag);
        File->WriteString((LPCTSTR)tmp);
        IndentAttribs = IndentAttribs + "    ";
    }
    SaveAttrib(File, IndentAttribs, "VERTEXCOUNT", Count);
    if (Tag!=NULL) {
        tmp.Format("%s>\n", (LPCTSTR)IndentTag);
        File->WriteString((LPCTSTR)tmp);
    }
}

void CValueVertexCloud::SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        IndentAttribs = IndentAttribs + "    ";
    }
    for (int i=0; i<Count; i++) {
        tmp.Format("%s<VERTEX  INDEX=\"%i\"  X=\"%g\"  Y=\"%g\"  Z=\"%g\" />\n", (LPCTSTR)IndentAttribs, i, Values[3*i], Values[3*i+1], Values[3*i+2]);
        File->WriteString((LPCTSTR)tmp);
    }
}

//----- Keyframe
void CExportedKeyFrame::SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        tmp.Format("%s<%s  INDEX = \"%i\"  TIME = \"%i\" >\n", (LPCTSTR)IndentTag, (LPCTSTR)*Tag, KeyFrameIndex, Time);
        File->WriteString((LPCTSTR)tmp);
        IndentAttribs = IndentAttribs + "    ";
    }
}

void CExportedKeyFrame::SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        IndentAttribs = IndentAttribs + "    ";
    }
    tmp = "CONTROLLER";
    Controller->SaveToXML(File, &tmp, &IndentAttribs);
    tmp = "VALUE";
    Value->SaveToXML(File, &tmp, &IndentAttribs);
}

//----- Exporter

CExportedController::CExportedController(void) {
}

CExportedController::~CExportedController(void) {
    for (int i=0; i<Keyframes.GetSize(); i++) {
        CExportedKeyFrame *Frame = (CExportedKeyFrame *)Keyframes.GetAt(i);
        if (Frame!=NULL) {
            delete Frame;
        }
    }
}

void CExportedController::SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        tmp.Format("%s<%s\n", (LPCTSTR)IndentTag, (LPCTSTR)*Tag);
        File->WriteString((LPCTSTR)tmp);
        IndentAttribs = IndentAttribs + "    ";
    }
    SaveAttrib(File, IndentAttribs, "Field", Field);
    SaveAttrib(File, IndentAttribs, "ID", ID);
    SaveAttrib(File, IndentAttribs, "OwnerID", OwnerID);
    SaveAttrib(File, IndentAttribs, "Controller", Controller);
    SaveAttrib(File, IndentAttribs, "Value", Value);
    SaveAttrib(File, IndentAttribs, "KeyframeCount", Keyframes.GetSize());
    if (Tag!=NULL) {
        tmp.Format("%s>\n", (LPCTSTR)IndentTag);
        File->WriteString((LPCTSTR)tmp);
    }
}

void CExportedController::SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent) {
    CString IndentTag = *Indent;
    CString IndentAttribs = *Indent;
    CString tmp;
    if (Tag!=NULL) {
        IndentAttribs = IndentAttribs + "    ";
    }
    tmp = "KEYFRAME";
    for (int i=0; i<Keyframes.GetSize(); i++) {
        CExportedKeyFrame *Frame = (CExportedKeyFrame *)Keyframes.GetAt(i);
        if (Frame!=NULL) {
            Frame->SaveToXML(File, &tmp, &IndentAttribs);
        }
    }
}

void CExportedController::CreateFromFlexporter(const ControllerDescriptor& Ctl) {
    Field = Ctl.mField;
    ID = Ctl.mObjectID;
    OwnerID = Ctl.mOwnerID;
    switch(Ctl.mData->mType) {
        case CTRL_FLOAT : Value = "FLOAT"; break;
		case CTRL_VECTOR : Value = "VECTOR"; break;
		case CTRL_QUAT : Value = "QUATERION"; break;
		case CTRL_SCALE : Value = "SCALE"; break;
		case CTRL_PR : Value = "PR"; break;
		case CTRL_PRS : Value = "PRS"; break;
		case CTRL_VERTEXCLOUD : Value = "VERTEXCLOUD"; break;
        default: Value = "UNKNOWN";
    };

    Controller = "UNKNOWN";

    if (Ctl.mData->mMode == CTRL_SAMPLES) {
        Type = "SAMPLES";
        Controller = "LINEAR";
        SampleData* mdata = (SampleData*)Ctl.mData;
        //int FrameCount = 1;
        int FrameCount = mdata->mNbSamples;
        this->Keyframes.SetSize(0, 10);
        float *Samples = (float *)mdata->mSamples;
        for (int i=0; i<FrameCount; i++) {
            CExportedKeyFrame *KeyFrame = new CExportedKeyFrame;
            KeyFrame->Controller = new CControllerLinear;
            KeyFrame->KeyFrameIndex = i;
            KeyFrame->Time = -1;
            switch(mdata->mType) {
                case CTRL_FLOAT : {
                    KeyFrame->Value = new CValueFloat;
                    ((CValueFloat *)KeyFrame->Value)->Value = Samples[i];
                } break;
                case CTRL_VECTOR : {
                    KeyFrame->Value = new CValuePoint;
                    memcpy(((CValuePoint *)KeyFrame->Value)->Value, &Samples[3 * i], 3 * sizeof(float));
                } break;
		        case CTRL_QUAT : {
                    KeyFrame->Value = new CValueQuaterion;
                    memcpy(((CValueQuaterion *)KeyFrame->Value)->Value, &Samples[4 * i], 4 * sizeof(float));
                } break;
		        case CTRL_SCALE : {
                    KeyFrame->Value = new CValueScale;
                    memcpy(((CValueScale *)KeyFrame->Value)->Scale, &Samples[7 * i], 3 * sizeof(float));
                    memcpy(((CValueScale *)KeyFrame->Value)->Quat, &Samples[7 * i + 3], 4 * sizeof(float));
                } break;
		        case CTRL_PR : {
                    KeyFrame->Value = new CValuePR;
                    memcpy(((CValuePR *)KeyFrame->Value)->Position, &Samples[7 * i], sizeof(float));
                    memcpy(((CValuePR *)KeyFrame->Value)->Rotation, &Samples[7 * i + 3], sizeof(float));
                } break;
                case CTRL_PRS : {
                    KeyFrame->Value = new CValuePRS;
                    memcpy(((CValuePRS *)KeyFrame->Value)->Position, &Samples[10 * i], 3 * sizeof(float));
                    memcpy(((CValuePRS *)KeyFrame->Value)->Rotation, &Samples[10 * i + 3], 4 *sizeof(float));
                    memcpy(((CValuePRS *)KeyFrame->Value)->Scale, &Samples[10 * i + 7], 3 * sizeof(float));
                } break;
		        case CTRL_VERTEXCLOUD :  {
                    MorphData* mdata = (MorphData*)Ctl.mData;
                    KeyFrame->Value = new CValueVertexCloud;
                    ((CValueVertexCloud *)KeyFrame->Value)->Count = mdata->mNbVertices;
                    ((CValueVertexCloud *)KeyFrame->Value)->Values = (float *)calloc(mdata->mNbVertices, 3 * sizeof(float));
                    memcpy(((CValueVertexCloud *)KeyFrame->Value)->Values, &Samples[i * mdata->mNbVertices * 3], sizeof(float) * mdata->mNbVertices * 3);
                } break;
                default : KeyFrame->Value = NULL;
            };
            this->Keyframes.Add(KeyFrame);
        }
    } else if (Ctl.mData->mMode == CTRL_KEYFRAMES) {
        Type = "KEYFRAMES";
        KeyframeData* kdata = (KeyframeData*)Ctl.mData;
        int FrameCount = kdata->mNbKeyframes;
        float *KeyData = (float *)kdata->mKeyframes;
        for (int i=0; i<FrameCount; i++) {
            CExportedKeyFrame *KeyFrame = new CExportedKeyFrame;
            KeyFrame->KeyFrameIndex = i;
            KeyFrame->Time = (int)*((int *)KeyData);
            KeyData = KeyData + 2;
            if (kdata->mScheme == SCHEME_TCB) {
                KeyFrame->Controller = new CControllerTCB;
                Controller = "TCB";
                ((CControllerTCB *)KeyFrame->Controller)->Tension = KeyData[0];
                ((CControllerTCB *)KeyFrame->Controller)->Continuity = KeyData[1];
                ((CControllerTCB *)KeyFrame->Controller)->Bias = KeyData[2];
                ((CControllerTCB *)KeyFrame->Controller)->EaseTo = KeyData[3];
                ((CControllerTCB *)KeyFrame->Controller)->EaseFrom = KeyData[4];
                KeyData = KeyData + 5;
            } else if (kdata->mScheme == SCHEME_LINEAR) {
                Controller = "LINEAR";
                KeyFrame->Controller = new CControllerLinear;
            } else if (kdata->mScheme == SCHEME_BEZIER) {
                KeyFrame->Controller = new CControllerBezier;
                Controller = "BEZIER";
                if (kdata->mType == CTRL_FLOAT) {
                    ((CControllerBezier *)KeyFrame->Controller)->TangentIn[0] = KeyData[0];
                    ((CControllerBezier *)KeyFrame->Controller)->TangentOut[0] = KeyData[1];
                    KeyData = KeyData + 2;
                } else {
                    memcpy(((CControllerBezier *)KeyFrame->Controller)->TangentIn, &KeyData[0], 3 * sizeof(float));
                    memcpy(((CControllerBezier *)KeyFrame->Controller)->TangentOut, &KeyData[3], 3 * sizeof(float));
                    KeyData = KeyData + 6;
                }
            } else {
                KeyFrame->Controller = NULL;
                CString Msg;
                Msg.Format("Unknown keyframe interpolator %i (ControllerID:%i OwnerID:%i Field:%s",
                    kdata->mScheme, Ctl.mObjectID, Ctl.mOwnerID, Ctl.mField);
                IceCore::MessageBox(null, (LPCTSTR)Msg, "WARNING", MB_OK);
            }
            switch(kdata->mType) {
                case CTRL_FLOAT : {
                    KeyFrame->Value = new CValueFloat;
                    ((CValueFloat *)KeyFrame->Value)->Value = KeyData[0];
                    KeyData++;
                } break;
                case CTRL_VECTOR : {
                    KeyFrame->Value = new CValuePoint;
                    memcpy(((CValuePoint *)KeyFrame->Value)->Value, KeyData, 3 * sizeof(float));
                    KeyData = KeyData + 3;
                } break;
		        case CTRL_QUAT : {
                    KeyFrame->Value = new CValueQuaterion;
                    memcpy(((CValueQuaterion *)KeyFrame->Value)->Value, KeyData, 4 * sizeof(float));
                    KeyData = KeyData + 4;
                } break;
                case CTRL_SCALE : {
                    KeyFrame->Value = new CValueScale;
                    memcpy(((CValueScale *)KeyFrame->Value)->Scale, &KeyData[0], 3 * sizeof(float));
                    memcpy(((CValueScale *)KeyFrame->Value)->Quat, &KeyData[3], 4 * sizeof(float));
                    KeyData = KeyData + 7;
                } break;
                default : {
                    KeyFrame->Value = NULL;
                    CString Msg;
                    Msg.Format("Unexpected keyframe value '%s' (%i) (ControllerID:%i OwnerID:%i Field:%s",
                        (LPCTSTR)Value, kdata->mScheme, Ctl.mObjectID, Ctl.mOwnerID, Ctl.mField);
                    IceCore::MessageBox(null, (LPCTSTR)Msg, "WARNING", MB_OK);
                }
            }
            this->Keyframes.Add(KeyFrame);
        }
    } else {
        Type = "UNKNOWN";
    }
}
