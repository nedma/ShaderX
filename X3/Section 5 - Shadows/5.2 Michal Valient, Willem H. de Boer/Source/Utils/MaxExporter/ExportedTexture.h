#pragma once
#include "exportedentity.h"

class CExportedTexture : public CExportedEntity {
public:
    int      ID;
    CString  Name;

    // Diffuse map cropping values
    float    TextureCrop_OffsetUV[2];
    float    TextureCrop_ScaleUV[2];
    float    TextureMatrix_4x3[4][3];

    CExportedTexture(void);
    virtual ~CExportedTexture(void);

    virtual void CreateFromFlexporter(const TextureDescriptor& Obj);

    //Parts of XML export - save starting tag with attributes
    virtual void SaveToXML_StartTAG(CStdioFile *File, CString *Tag, CString *Indent);

    //Parts of XML export - save inner nodes (subnodes) and tag text
    virtual void SaveToXML_InnerBody(CStdioFile *File, CString *Tag, CString *Indent);
};
