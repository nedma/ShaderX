#include "stdafx.h"
#include "d3scene\eng-d3s_prslinearcontroller.h"
#include "sc-xmlparser.h"
#include "ENG-RenderSettings.h"
#include "ENG-Base3DObject.h"

CPRSLinearController::CPRSLinearController(char *ObjName,CEngine *OwnerEngine) : CBaseClass(ObjName, OwnerEngine) {
    _LOG(this, D3_DV_GRP2_LEV0, "Created");
    Keyframes = NULL;
    KeyframeCount = 0;
}

CPRSLinearController::~CPRSLinearController(void) {
    if (Keyframes!=NULL) MemMgr->FreeMem(Keyframes);
    _LOG(this, D3_DV_GRP2_LEV0, "Destroyed");
}

bool CPRSLinearController::LoadXML(CXMLNode *Node, CRenderSettings &Settings) {
    int logE = _LOGB(this, D3_DV_GRP2_LEV0, "Loading");
    bool result = true;

    CMString Field;
    int     ID;
    int     OwnerID;
    CMString Controller;
    CMString Value;
    int     KFCount;
    Node->GetAttributeValue("FIELD", Field);
    Node->GetAttributeValueI("ID", ID);
    Node->GetAttributeValueI("OWNERID", OwnerID);
    Node->GetAttributeValue("CONTROLLER", Controller);
    Node->GetAttributeValue("VALUE", Value);
    Node->GetAttributeValueI("KEYFRAMECOUNT", KFCount);

    if (strcmp(Value.c_str(), "PRS")!=0) {
        _WARN(this, ErrMgr->TNoSupport, "No support for controller '%s' and values '%s'", Controller.c_str(), Value.c_str());
        result = false;
    }

    Keyframes = (KeyFrame *)MemMgr->AllocMem(sizeof(KeyFrame) * KFCount,0);
    KeyframeCount = KFCount;

    for (UINT i=0; i<Node->GetSubNodeCount(); i++) {
        CXMLNode *KeyNode = Node->GetSubNode(i);
        if (KeyNode) {
            int NodeIndex = 0;
            KeyNode->GetAttributeValueI("INDEX", NodeIndex);
            CXMLNode *ValueNode = KeyNode->GetSubNode(KeyNode->FindSubNode("VALUE",0));
            if (ValueNode) {
                CXMLNode *PosNode = ValueNode->GetSubNode(ValueNode->FindSubNode("POSITION",0));
                if (PosNode) {
                    PosNode->GetAttributeValueF("X", Keyframes[NodeIndex].Position.x);
                    PosNode->GetAttributeValueF("Y", Keyframes[NodeIndex].Position.y);
                    PosNode->GetAttributeValueF("Z", Keyframes[NodeIndex].Position.z);
                } else {
                    _WARN(this, ErrMgr->TNoSupport, "Cannot find 'POSITION' tag");
                    result = false;
                    break;
                }
                CXMLNode *RotNode = ValueNode->GetSubNode(ValueNode->FindSubNode("ROTATION",0));
                if (RotNode) {
                    RotNode->GetAttributeValueF("X", Keyframes[NodeIndex].Rotation.x);
                    RotNode->GetAttributeValueF("Y", Keyframes[NodeIndex].Rotation.y);
                    RotNode->GetAttributeValueF("Z", Keyframes[NodeIndex].Rotation.z);
                    RotNode->GetAttributeValueF("W", Keyframes[NodeIndex].Rotation.w);
                } else {
                    _WARN(this, ErrMgr->TNoSupport, "Cannot find 'ROTATION' tag");
                    result = false;
                    break;
                }
                CXMLNode *ScaNode = ValueNode->GetSubNode(ValueNode->FindSubNode("SCALE",0));
                if (ScaNode) {
                    ScaNode->GetAttributeValueF("X", Keyframes[NodeIndex].Scale.x);
                    ScaNode->GetAttributeValueF("Y", Keyframes[NodeIndex].Scale.y);
                    ScaNode->GetAttributeValueF("Z", Keyframes[NodeIndex].Scale.z);
                } else {
                    _WARN(this, ErrMgr->TNoSupport, "Cannot find 'SCALE' tag");
                    result = false;
                    break;
                }
            } else {
                _WARN(this, ErrMgr->TNoSupport, "Cannot find 'VALUE' tag");
                result = false;
                break;
            }
        } else {
            _WARN(this, ErrMgr->TNoSupport, "Cannot find any 'KEYFRAME' tag");
            result = false;
            break;
        }
    }
    _LOGE(logE, "Load end");
    return result;
}

bool CPRSLinearController::Animate(CBase3DObject *Object, float Time, CRenderSettings &Settings) {
    if (KeyframeCount==0) return false;
    int StartKF = (int)floorf(Time);
    int EndKF = (int)ceilf(Time);
    float LerpParam = Time - (float)StartKF;

    StartKF = StartKF % (KeyframeCount-1);
    EndKF = EndKF % (KeyframeCount-1);

    if ((StartKF>=(int)KeyframeCount) || (EndKF>=(int)KeyframeCount)) {
        StartKF = KeyframeCount-1;
        EndKF = KeyframeCount-1;
        LerpParam = 0;
    } else if ((StartKF<0) || (EndKF<0)) {
        StartKF = 0;
        EndKF = 0;
        LerpParam = 0;
    }
    D3DXVec3Lerp(&Object->Translation, &Keyframes[StartKF].Position, &Keyframes[EndKF].Position, LerpParam);
    D3DXVec3Lerp(&Object->Scale, &Keyframes[StartKF].Scale, &Keyframes[EndKF].Scale, LerpParam);
    D3DXQuaternionSlerp(&Object->Rotation, &Keyframes[StartKF].Rotation, &Keyframes[EndKF].Rotation, LerpParam);
    return true;
}