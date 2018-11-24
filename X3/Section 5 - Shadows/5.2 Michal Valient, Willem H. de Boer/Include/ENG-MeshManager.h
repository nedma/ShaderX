// ENG-MeshManager.h: interface for the CMeshManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "nommgr.h"
#include "d3d9.h"
#include "mmgr.h"

#include "ENG-ResourceManager.h"



//############################################################################
//Forward declarations

class CMeshManager : public CResourceManager {
protected:
    //Guard structure for SetStreamSource
    class CStreamInfo : public CRootClass {
    public:
        UINT    StreamNumber;
        IDirect3DVertexBuffer9 *VBuf;
        UINT    OffsetInBytes;
        UINT    Stride;

        CStreamInfo();
        virtual ~CStreamInfo();
    };

    //Guard structure for SetIndices
    class CIndicesInfo : public CRootClass {
    public:
        IDirect3DIndexBuffer9 *Ibuf;

        CIndicesInfo();
        virtual ~CIndicesInfo();
    };

    CHashTable2 *GuardStream;
    CIndicesInfo *GuardIndices;

public:
    CMeshManager(char *ObjName, CEngine *OwnerEngine);
	virtual ~CMeshManager();

    //DX8 Mapped inline functions. They use guard buffers
    HRESULT SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9 *VBuf, UINT OffsetInBytes, UINT Stride);
    HRESULT SetIndices(IDirect3DIndexBuffer9 *IBuf);

    virtual void DeviceLost(CRenderSettings &Settings);

    MAKE_CLSCREATE(CMeshManager);
    MAKE_CLSNAME("CMeshManager");
    MAKE_DEFAULTGARBAGE()
};

