#pragma once
#include "nommgr.h"
#include "expat.h"
#include "mmgr.h"

#include "baseclass.h"
#include "SC-MString.h"

class CRootClassArray;
class CStrHashTable;
class CXMLParser;

/**
    This class is one node in XML data
*/
class CXMLNode : public CRootClass {
friend class CXMLParser;
protected:
    CRootClassArray         *SubNodes;      //List of sub nodes of this node
    CStrHashTable           *Attributes;    //List of attribudes of this node
    CXMLNode                *Parent;

    CMString                Name;           //Name of tag
    char                    *Data;          //Data inside the tag (if any)
    UINT                    DataLength;     //Length of the data buffer
    bool                    DataIsCDATA;    //If data were defined as CData section
public:

    CXMLNode();
    virtual ~CXMLNode();


//-------- Parent
    CXMLNode *GetParent() const;
    void SetParent(const CXMLNode *NewParent);

    
//-------- Name
    CMString GetName() const;
    void SetName(char *NewName);

//-------- Data
    ///returns length of data buffer
    UINT GetDataLength() const;
    ///copies data buffer to the OutputData. Memory allocated for OutputData has to be
    ///big enough
    void GetData(char *OutputData) const;
    ///Copies (sets) data from parameters.
    void SetData(const char *NewData, const UINT NewLength);
    ///Concat new data to current stored data.
    void ConcatData(const char *AddData, const UINT AddLength);
    ///Returns true, if data is in CDATA section
    bool GetIsCDATA() const;
    ///sets IsCData boolean parameter
    void SetIsCDATA(const bool NewIsData);


//-------- Subnodes
    ///Adds sub node to the tree. Node memory is then managed by node tree!!!
    void AddSubNode(CXMLNode *Node);
    ///Deletes sub node from the tree. Entire subtree will be deleted!
    void DeleteSubNode(UINT NodeIndex);
    ///Erase (only remove, extract) sub node. Subnode will remain valid in memory
    void EraseSubNode(UINT NodeIndex);
    ///Returns pointer to the sub node.
    CXMLNode *GetSubNode(UINT NodeIndex) const;
    ///Returns count of the subnodes.
    UINT GetSubNodeCount() const;
    ///Finds subnode index by name starting _at_ given index
    ///returns 0xFFFFFFFF if node not found
    UINT FindSubNode(const char *NodeName, const UINT StartIndex) const;

//-------- Attributes
    ///Adds attribute to node. buffers are not touched, nor managed. Only copied.
    void AddAttribute(const char *Name, const char *Value);
    ///Deletes attribute from node.
    void DeleteAttribute(const char *Name);
    ///Stores attribute's value in parameter Value. It remains untouched, if value not found.
    ///returns false, if attribute not found
    bool GetAttributeValue(const char *Name, CMString &Value) const;
    bool GetAttributeValueF(const char *Name, float &Value) const;
    bool GetAttributeValueI(const char *Name, int &Value) const;
    bool GetAttributeValueU(const char *Name, UINT &Value) const;
    bool GetAttributeValueB(const char *Name, bool &Value) const;
    ///Returns count of the attributes.
    UINT GetAttributeCount() const;
    ///Returns first attribute
//-------------------iteration methods 
//NOTE: for description see HashTable's iteration methods
    UINT GetFirstAttribute() const;
    UINT GetNextAttribute(const UINT start) const;
    UINT GetPrevAttribute(const UINT start) const;    

    ///stores name and value of attribute at given index in parameters.
    void GetAttributeAtIndex(const UINT index, CMString &Name, CMString &Value) const;

//Debug
    void LogContents();
};

/**
    This class is used for parsing of XML data. It uses expat for windows library (as DLL)
*/
class CXMLParser : public CBaseClass {
protected:
    XML_Parser      Parser;
    CXMLNode        *CurrentNode;
    CXMLNode        *RootNode;

    static void _cdecl StartElemPrivate(void *userData, const XML_Char *name, const XML_Char **atts);
    static void _cdecl EndElemPrivate(void *userData, const XML_Char *name);
    static void _cdecl CharDataPrivate(void *userData, const XML_Char *s, int len);
public:
    CXMLParser(char *pObjName,CEngine *pOwnerEngine);
    virtual ~CXMLParser(void);

    CXMLNode *Parse(const char *XMLData, const UINT XMLDataLen);
    CXMLNode *Parse(int ResourceIndex);
    CXMLNode *Parse(const CMString &ResourceName);

    MAKE_CLSCREATE(CXMLParser);
    MAKE_CLSNAME("CXMLParser");
    MAKE_DEFAULTGARBAGE();
};
