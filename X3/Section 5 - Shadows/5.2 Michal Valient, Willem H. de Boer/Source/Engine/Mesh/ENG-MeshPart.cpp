#include "stdafx.h"
#include "eng-meshpart.h"

CMeshPart::CMeshPart(void) {
    VertexLockCount = 0;
    VertexFVF = 0;
    VertexPool = (D3DPOOL)0;
    VertexUsage = 0;
    VertexBuffer = NULL;
    IndexLockCount = 0;
    IndexBuffer = NULL;
    IndexFormat = (D3DFORMAT)0;
    IndexPool = (D3DPOOL)0;
    IndexUsage = 0;
    IndexLength = 0;
    AppData = 0;
    _LOG("MeshPart",NULL,D3_DV_GRP3_LEV0,"Created");
}

CMeshPart::~CMeshPart(void) {
    IndexDelete();
    VertexDelete();
}

//Index buffer
char *CMeshPart::IndexLock(UINT OffsetToLock, UINT SizeToLock, DWORD Flags) {
    char *result = NULL;
    HRESULT res = this->IndexBuffer->Lock(OffsetToLock, SizeToLock,(VOID **)&result, Flags);
    if (res!=D3D_OK) {
        _ERROR("CMeshPart", "IB", ErrMgr->TSystem, "Cannot lock index buffer. HRESULT is %i",res);
        return NULL;
    }
    IndexLockCount++;
    _LOG("CMeshPart",NULL,D3_DV_GRP3_LEV4,"Locking index buffer, lock count is %u",IndexLockCount);
    return result;
}

void CMeshPart::IndexUnlock(bool ForceAll) {
    if (IndexLockCount>0) {
        HRESULT res = this->IndexBuffer->Unlock();
        if (res!=D3D_OK) {
            _ERROR("CMeshPart", "IB", ErrMgr->TSystem, "Cannot unlock index buffer. HRESULT is %i",res);
            return;
        }
        IndexLockCount--;
        _LOG("CMeshPart",NULL,D3_DV_GRP3_LEV4,"Unlocking index buffer, lock count is %u",IndexLockCount);
        if ((ForceAll) && (IndexLockCount>0)) IndexUnlock(ForceAll);
    } else {
        _LOG("CMeshPart",NULL,D3_DV_GRP3_LEV4,"Index buffer is not locked");
    }
}

bool CMeshPart::IndexBufferCreate(IDirect3DDevice9 *Device, UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool) {
    IDirect3DIndexBuffer9 *IBuf = NULL;
    HRESULT res = Device->CreateIndexBuffer(Length, Usage, Format, Pool, &IBuf, NULL);
    if (res!=D3D_OK) {
        _ERROR("CMeshPart", "IB", ErrMgr->TSystem, "Cannot create index buffer. HRESULT is %i",res);
        return FALSE;
    }
    IndexBuffer = IBuf;
    IndexLength = Length;
    IndexFormat = Format;
    IndexUsage = Usage;
    IndexPool = Pool;
    IndexLockCount = 0;
    _LOG("CMeshPart",NULL,D3_DV_GRP3_LEV4,"Created index buffer");
    return TRUE;
}

void CMeshPart::IndexDelete() {
    if (IndexBuffer) {
        IndexUnlock(TRUE);
        ULONG res = IndexBuffer->Release();
        IndexLockCount = 0;
        IndexBuffer = NULL;
        IndexFormat = (D3DFORMAT)0;
        IndexPool = (D3DPOOL)0;
        IndexUsage = 0;
        IndexLength = 0;
        _LOG("CMeshPart",NULL,D3_DV_GRP3_LEV0,"Deleting index buffer, new reference count is %u",res);
    }
}

void CMeshPart::IndexUseExisting(CMeshPart &Part) {
    IndexDelete();
    IndexBuffer = Part.IndexBuffer;
    IndexLockCount = 0;
    IndexLength = Part.IndexLength;
    IndexPool = Part.IndexPool;
    IndexUsage = Part.IndexUsage;
    IndexFormat = Part.IndexFormat;
    if (IndexBuffer) IndexBuffer->AddRef();
}

//Vertex buffer
char *CMeshPart::VertexLock(UINT OffsetToLock, UINT SizeToLock, DWORD Flags) {
    char *result = NULL;
    HRESULT res = this->VertexBuffer->Lock(OffsetToLock, SizeToLock,(VOID **)&result, Flags);
    if (res!=D3D_OK) {
        _ERROR("CMeshPart", "VB", ErrMgr->TSystem, "Cannot lock vertex buffer. HRESULT is %i",res);
        return NULL;
    }
    VertexLockCount++;
    _LOG("CMeshPart",NULL,D3_DV_GRP3_LEV4,"Locking vertex buffer, lock count is %u",VertexLockCount);
    return result;
}

void CMeshPart::VertexUnlock(bool ForceAll) {
    if (VertexLockCount>0) {
        HRESULT res = this->VertexBuffer->Unlock();
        if (res!=D3D_OK) {
            _ERROR("CMeshPart", "VB", ErrMgr->TSystem, "Cannot unlock vertex buffer. HRESULT is %i",res);
            return;
        }
        VertexLockCount--;
        _LOG("CMeshPart",NULL,D3_DV_GRP3_LEV4,"Unlocking vertex buffer, lock count is %u",VertexLockCount);
        if ((ForceAll) && (VertexLockCount>0)) VertexUnlock(ForceAll);
    } else {
        _LOG("CMeshPart",NULL,D3_DV_GRP3_LEV4,"Vertex buffer is not locked");
    }
}

bool CMeshPart::VertexBufferCreate(IDirect3DDevice9 *Device, UINT Length, DWORD Usage, UINT FVF, D3DPOOL Pool) {
    IDirect3DVertexBuffer9 *VBuf = NULL;
    HRESULT res = Device->CreateVertexBuffer(Length, Usage, FVF, Pool, &VBuf, NULL);
    if (res!=D3D_OK) {
        _ERROR("CMeshPart", "VB", ErrMgr->TSystem, "Cannot create vertex buffer. HRESULT is %i",res);
        return FALSE;
    }
    VertexBuffer = VBuf;
    VertexLength = Length;
    VertexFVF = FVF;
    VertexPool = Pool;
    VertexUsage = Usage;
    VertexLockCount = 0;
    _LOG("CMeshPart",NULL,D3_DV_GRP3_LEV4,"Created vertex buffer");
    return TRUE;
}

void CMeshPart::VertexDelete() {
    if (VertexBuffer) {
        VertexUnlock(TRUE);
        ULONG res = VertexBuffer->Release();
        VertexLockCount = 0;
        VertexFVF = 0;
        VertexPool = (D3DPOOL)0;
        VertexUsage = 0;
        VertexBuffer = NULL;
        _LOG("CMeshPart",NULL,D3_DV_GRP3_LEV0,"Deleting vertex buffer, new reference count is %u",res);
    }
}

void CMeshPart::VertexUseExisting(CMeshPart &Part) {
    VertexDelete();
    VertexBuffer = Part.VertexBuffer;
    VertexLockCount = 0;
    VertexFVF = Part.VertexFVF;
    VertexPool = Part.VertexPool;
    VertexUsage = Part.VertexUsage;
    if (VertexBuffer) VertexBuffer->AddRef();
}


