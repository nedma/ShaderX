/*
    Uses two NVIDIA Effect's browser methods for tangent space calculation.
*/
#pragma once
#include "eng-mesh.h"

#include "nommgr.h"
#include "D3dx9.h"
#include "mmgr.h"

class CMeshD3F : public CMesh {
    //My directX vertex bufffer struct.
    // Vertex format for Dimension^3 mesh.
    struct D3F_VERTEX {
        D3DXVECTOR3 Position;
	    D3DXVECTOR3 Normal;
	    D3DXVECTOR2 Texture;
	    D3DXVECTOR3 S;
    };
public:
    virtual bool Load(DWORD LoadIndex, CRenderSettings &RenderSettings);
    virtual bool Update(CRenderSettings &RenderSettings);

    HRESULT FindAndFixDegenerateVertexBasis(	D3F_VERTEX	* pVertices,
											DWORD		  dwNumVertices,
											void		* pIndices,
											DWORD		  dwNumIndices,
											bool bUnifyNormals,
											bool is32BitIndices);
    HRESULT CreateBasisVectors(LPDIRECT3DVERTEXBUFFER9 pVertexBuffer, LPDIRECT3DINDEXBUFFER9 pIndexBuffer);

    CMeshD3F(char *ObjName, CEngine *OwnerEngine);
    MAKE_CLSCREATE(CMeshD3F);
    MAKE_CLSNAME("CMeshD3F");
    MAKE_DEFAULTGARBAGE()
};
