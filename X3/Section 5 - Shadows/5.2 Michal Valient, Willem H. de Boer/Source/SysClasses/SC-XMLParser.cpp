#include "stdafx.h"
#include "sc-xmlparser.h"
#include "SC-DynamicArray.h"
#include "SC-StrHashTable.h"
#include "ENG-engine.h"
#include "FIL-FileManager.h"

void CXMLNode::LogContents() {
    int logE = _LOGB(NULL,NULL, D3_DV_GRP0_LEV0, "Node '%s' with %i Subnodes and %i attributes", Name.c_str(), SubNodes->GetSize(), Attributes->GetItemCount());
    if (DataLength) _LOG(NULL,NULL, D3_DV_GRP0_LEV0, "Data : '%s'", Data);
    if (Attributes->GetItemCount()>0) {
        UINT index;
        UINT index2 = Attributes->GetFirstItem();
        do {
            CMString tmp1, tmp2;
            index = index2;
            tmp2 = (char *)Attributes->GetValueAtIndex(index);
            Attributes->GetKeyAtIndex(index, tmp1);
            _LOG(NULL,NULL, D3_DV_GRP0_LEV0, "Attribute %s = '%s'", tmp1.c_str(), tmp2.c_str());
            index2 = Attributes->GetNextItem(index);
        } while (index2!=index);
        for (int i=0; i<SubNodes->GetSize(); i++) {
            CXMLNode *tmp = (CXMLNode *)SubNodes->GetAt(i);
            if (tmp!=NULL) {
                tmp->LogContents();
            }
        }
    }
    _LOGE(logE);
}

//------------------------------------
//-- CXMLNode
//------------------------------------
CXMLNode::CXMLNode() {
    SubNodes = new CRootClassArray(true);
    Attributes = new CStrHashTable(false,0);
    Data = NULL;
    DataLength = 0;
    DataIsCDATA = false;
    Parent = NULL;
}

CXMLNode::~CXMLNode() {
    delete SubNodes;
    if (Attributes->GetItemCount()>0) {
        UINT index;
        UINT index2 = Attributes->GetFirstItem();
        do {
            index = index2;
            MemMgr->FreeMem(Attributes->GetValueAtIndex(index));
            index2 = Attributes->GetNextItem(index);
        } while (index2!=index);
    }
    delete Attributes;
    if (Data!=NULL) MemMgr->FreeMem(Data);
}

CXMLNode *CXMLNode::GetParent() const {
    return Parent;
}

void CXMLNode::SetParent(const CXMLNode *NewParent) {
    Parent = (CXMLNode *)NewParent;
}


CMString CXMLNode::GetName() const {
    return Name;
}

void CXMLNode::SetName(char *NewName) {
    Name = NewName;
}

UINT CXMLNode::GetDataLength() const {
    return DataLength;
}

void CXMLNode::GetData(char *OutputData) const {
    memcpy(OutputData, Data, DataLength);
}

void CXMLNode::SetData(const char *NewData, const UINT NewLength) {
    Data = (char *)MemMgr->ReAllocMem(Data, NewLength+1, 0);
    memcpy(Data, NewData, NewLength);
    DataLength = NewLength;
    Data[DataLength] = 0;        //terminating with NULL character
}

void CXMLNode::ConcatData(const char *AddData, const UINT AddLength) {
    if ((Data == NULL) || (DataLength == 0)) {
        SetData(AddData, AddLength);
    } else {
        Data = (char *)MemMgr->ReAllocMem(Data, DataLength + AddLength + 2, 0);
        Data[DataLength] = ' ';
        memcpy(&Data[DataLength+1], AddData, AddLength);
        DataLength = DataLength + AddLength + 1;
        Data[DataLength] = 0;        //terminating with NULL character
    }
}

bool CXMLNode::GetIsCDATA() const {
    return DataIsCDATA;
}

void CXMLNode::SetIsCDATA(const bool NewIsData) {
    DataIsCDATA = NewIsData;
}

void CXMLNode::AddSubNode(CXMLNode *Node) {
    SubNodes->Add(Node);
}

void CXMLNode::DeleteSubNode(UINT NodeIndex) {
    SubNodes->DeleteAt(NodeIndex);
}

void CXMLNode::EraseSubNode(UINT NodeIndex) {
    SubNodes->EraseAt(NodeIndex);
}

CXMLNode *CXMLNode::GetSubNode(UINT NodeIndex) const {
    return (CXMLNode *)SubNodes->GetAt(NodeIndex);
}

UINT CXMLNode::GetSubNodeCount() const {
    return SubNodes->GetSize();
}

UINT CXMLNode::FindSubNode(const char *NodeName, const UINT StartIndex) const {
    if (StartIndex>=(UINT)SubNodes->GetSize()) return 0xFFFFFFFF;
    for (int i=StartIndex; i<SubNodes->GetSize(); i++) {
        CXMLNode *tmp = (CXMLNode *)SubNodes->GetAt(i);
        if ((tmp!=NULL) && (strcmp(tmp->Name.c_str(),NodeName) == 0)) {
            return i;
        }
    }
    return 0xFFFFFFFF;
}

void CXMLNode::AddAttribute(const char *Name, const char *Value) {
    UINT len = strlen(Value);
    char *AttValue = (char *)MemMgr->AllocMem(len+1,0);
    memcpy(AttValue, Value, len);
    AttValue[len] = 0;
    Attributes->Add(Name, AttValue);
}

void CXMLNode::DeleteAttribute(const char *Name) {
    void *Data = Attributes->Get(Name);
    if (Data!=NULL) MemMgr->FreeMem(Data);
    Attributes->Delete(Name);
}

bool CXMLNode::GetAttributeValue(const char *Name, CMString &Value) const {
    char *tmpValue = (char *)Attributes->Get(Name);
    if (tmpValue!=NULL) {
        Value = tmpValue;
        return true;
    } else {
        return false;
    }
}

bool CXMLNode::GetAttributeValueF(const char *Name, float &Value) const {
    CMString tmp;
    if (GetAttributeValue(Name, tmp)) {
        Value = (float)atof(tmp.c_str());
        return true;
    } else {
        return false;
    }
}

bool CXMLNode::GetAttributeValueI(const char *Name, int &Value) const {
    CMString tmp;
    if (GetAttributeValue(Name, tmp)) {
        Value = (int)atoi(tmp.c_str());
        return true;
    } else {
        return false;
    }
}

bool CXMLNode::GetAttributeValueU(const char *Name, UINT &Value) const {
    CMString tmp;
    if (GetAttributeValue(Name, tmp)) {
        Value = (UINT)_atoi64(tmp.c_str());
        return true;
    } else {
        return false;
    }
}

bool CXMLNode::GetAttributeValueB(const char *Name, bool &Value) const {
    CMString tmp;
    if (GetAttributeValue(Name, tmp)) {
        Value = (atoi(tmp.c_str())!=0) ? 1 : 0;
        return true;
    } else {
        return false;
    }
}

UINT CXMLNode::GetAttributeCount() const {
    return Attributes->GetItemCount();
}

UINT CXMLNode::GetFirstAttribute() const {
    return Attributes->GetFirstItem();
}

UINT CXMLNode::GetNextAttribute(const UINT start) const {
    return Attributes->GetNextItem(start);
}

UINT CXMLNode::GetPrevAttribute(const UINT start) const {
    return Attributes->GetPrevItem(start);
}

void CXMLNode::GetAttributeAtIndex(const UINT index, CMString &Name, CMString &Value) const {
    Attributes->GetKeyAtIndex(index, Name);
    Value = (char *)Attributes->GetValueAtIndex(index);
}


//------------------------------------
//-- CXMLParser
//------------------------------------


CXMLParser::CXMLParser(char *pObjName,CEngine *pOwnerEngine) : CBaseClass(pObjName, pOwnerEngine) {
    _LOG(this, D3_DV_GRP2_LEV0, "Created");
}

CXMLParser::~CXMLParser(void) {
    _LOG(this, D3_DV_GRP2_LEV0, "Destroyed");
}

CXMLNode *CXMLParser::Parse(const char *XMLData, const UINT XMLDataLen) {
    int logE = _LOGB(this, D3_DV_GRP2_LEV1, "Starting parse of buffer %p length %u", XMLData, XMLDataLen);

    Parser = XML_ParserCreate(NULL);
    XML_SetUserData(Parser, this);
    XML_SetStartElementHandler(Parser, (XML_StartElementHandler)StartElemPrivate);
    XML_SetEndElementHandler(Parser, (XML_EndElementHandler)EndElemPrivate);
    XML_SetCharacterDataHandler(Parser, (XML_CharacterDataHandler)CharDataPrivate);
    CurrentNode = NULL;
    RootNode = NULL;

    CXMLNode *Result = NULL;
    if (XML_Parse(Parser, XMLData, XMLDataLen, 1) == XML_STATUS_ERROR) {
        _WARN(this, ErrMgr->TSystem, "XML Parse ended with error : %s", XML_ErrorString(XML_GetErrorCode(Parser)));
        if (RootNode!=NULL) delete RootNode;
    } else {
        Result = RootNode;
        _LOGB(this, D3_DV_GRP2_LEV1, "Parsed with success");
    }
    RootNode = NULL;
    CurrentNode = NULL;
    XML_ParserFree(Parser);
    Parser = NULL;
    _LOGE(logE);
    return Result;
}

CXMLNode *CXMLParser::Parse(int ResourceIndex) {
    int logE = _LOGB(this, D3_DV_GRP2_LEV1, "Starting parse of resource (index %i)", ResourceIndex);
    CXMLNode *Result = NULL;
    if (this->OwnerEngine->GetFileManager()->Open(ResourceIndex)) {
        int ResourceSize = this->OwnerEngine->GetFileManager()->GetSize(ResourceIndex);
        if (ResourceSize>0) {
            char *ResBuffer = (char *)MemMgr->AllocMem(ResourceSize, 0);
            if (this->OwnerEngine->GetFileManager()->Load(ResourceIndex, ResBuffer)) {
                Result = Parse(ResBuffer, ResourceSize);
            }
            MemMgr->FreeMem(ResBuffer);
        }
    }
    _LOGE(logE);
    return Result;
}

CXMLNode *CXMLParser::Parse(const CMString &ResourceName) {
    int logE = _LOGB(this, D3_DV_GRP2_LEV1, "Starting parse of resource '%s'", ResourceName.c_str());
    CXMLNode *Result = NULL;
    int ResIndex = this->OwnerEngine->GetFileManager()->Find((CMString &)ResourceName);
    if (ResIndex>=0) Result = Parse(ResIndex);
    _LOGE(logE);
    return Result;
}


void _cdecl CXMLParser::StartElemPrivate(void *userData, const XML_Char *name, const XML_Char **atts) {
    CXMLParser *tmpParser = (CXMLParser *)userData;
    //_LOG(tmpParser, D3_DV_GRP2_LEV0, "Parsing element <%s>", (char *)name);
    CXMLNode *Node = new CXMLNode;
    Node->SetName((char *)name);
    Node->SetParent(tmpParser->CurrentNode);

    if (atts!=NULL) {
        //first, count the number of attributes
        int i=0;
        int AttributeCount = 0;
        while (atts[i]!=NULL) {
            AttributeCount++;
            i=i+2;
        
        }
        //set the initial size for hash table
        Node->Attributes->SetInitialSize(AttributeCount);
        i=0;
        while (atts[i]!=NULL) {
            UINT len = strlen(atts[i+1]);
            char *AttValue = (char *)MemMgr->AllocMem(len+1,0);
            memcpy(AttValue, atts[i+1], len);
            AttValue[len] = 0;
            Node->Attributes->Add(atts[i], AttValue);
            //_LOG(tmpParser, D3_DV_GRP2_LEV4, "    Adding attribute %s = '%s'", atts[i], atts[i+1]);
            i=i+2;
        }
    }

    if (tmpParser->CurrentNode == NULL) tmpParser->RootNode = Node;
    else {
        if (tmpParser->CurrentNode->SubNodes->GetSize()+1>tmpParser->CurrentNode->SubNodes->GetCapacity()) {
            tmpParser->CurrentNode->SubNodes->Grow(tmpParser->CurrentNode->SubNodes->GetSize());
        }
        tmpParser->CurrentNode->SubNodes->Add(Node);
    }
    tmpParser->CurrentNode = Node;
}

void _cdecl CXMLParser::EndElemPrivate(void *userData, const XML_Char *name) {
    CXMLParser *tmpParser = (CXMLParser *)userData;
    //_LOG(tmpParser, D3_DV_GRP2_LEV0, "Ending element <%s>", (char *)name);
    if (tmpParser->CurrentNode!=NULL) {
        tmpParser->CurrentNode->SubNodes->SetSize(tmpParser->CurrentNode->SubNodes->GetSize(),1);
        tmpParser->CurrentNode = tmpParser->CurrentNode->GetParent();
    }
}

void _cdecl CXMLParser::CharDataPrivate(void *userData, const XML_Char *s, int len) {
    CXMLParser *tmpParser = (CXMLParser *)userData;

    if (len>0) {
        //first we trim string from left
        char *tmp = (char *)s;
        int i;
        for (i=0; i<len; i++) 
            if ((!isspace(tmp[i])) && (isgraph(tmp[i]))) break;
        tmp = &tmp[i];  //tmp now starts at first non white space character
        len = len - i;
        if (len>0) {
            //now trim from right by decreasing length
            for (i=len-1; i>=0; i--) 
                if ((!isspace(tmp[i])) && (isgraph(tmp[i]))) break;
            len = i;
            if ((len>0) && (tmpParser->CurrentNode!=NULL)) {
                //_LOG(tmpParser, D3_DV_GRP2_LEV0, "Adding characters");
                tmpParser->CurrentNode->ConcatData(tmp, len);
            }
        }
    }
}
