// ENG-MeshManager.cpp: implementation of the CMeshManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ENG-MeshManager.h"
#include "SC-HashTable2.h"
#include "ENG-Engine.h"
#include "MGR-DirectGraphicsManager.h"
#include "DG-DirectGraphicsDeviceInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeshManager::CMeshManager(char *ObjName, CEngine *OwnerEngine) : CResourceManager(ObjName,OwnerEngine) {
    int logE = _LOGB(this,D3_DV_GRP0_LEV0,"Creating");
    GuardIndices = new CIndicesInfo();
    GuardStream = new CHashTable2(TRUE,0);
    _LOGE(logE);
}

CMeshManager::~CMeshManager() {
    int logE = _LOGB(this,D3_DV_GRP0_LEV0,"Destroying");
    delete GuardIndices;
    delete GuardStream;
    _LOGE(logE);
}

//###############################################################
//DX8 Mapped functions. They use guard buffers
HRESULT CMeshManager::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9 *VBuf, UINT OffsetInBytes, UINT Stride) {
    CStreamInfo *SInfo = (CStreamInfo *)GuardStream->Get(StreamNumber);
    if (SInfo == NULL) {
        SInfo = new CStreamInfo;
        SInfo->StreamNumber = StreamNumber;
        GuardStream->Add(SInfo->StreamNumber, SInfo);
    }
    if ((VBuf == SInfo->VBuf) && (Stride == SInfo->Stride)) { //identical
        _LOG(this,D3_DV_GRP0_LEV4,"SetStreamSource - IDENTICAL - Stream %4u, VertexBuffer %p, Offset %4u, Stride %4u", StreamNumber, VBuf, OffsetInBytes, Stride);
        return D3D_OK;
    } else {    //need to set new ones
        _LOG(this,D3_DV_GRP0_LEV4,"SetStreamSource - NEWVALUES - Stream %4u, VertexBuffer %p, Offset %4u, Stride %4u", StreamNumber, VBuf, OffsetInBytes, Stride);
        HRESULT result = OwnerEngine->GetGraphicsManager()->Direct3DDevice->SetStreamSource(StreamNumber, VBuf, OffsetInBytes, Stride);
        if (result!=D3D_OK) {
            _WARN(this,ErrMgr->TNoSupport,"SetStreamSource - FAILED - Vertexbuffer %p at stream %i",VBuf, StreamNumber);
            SInfo->VBuf = NULL;
        } else {
            SInfo->VBuf = VBuf;
            SInfo->Stride = Stride;
            SInfo->OffsetInBytes = OffsetInBytes;
        }
        return result;
    }
}

HRESULT CMeshManager::SetIndices(IDirect3DIndexBuffer9 *IBuf) {
    if (IBuf == GuardIndices->Ibuf) { //identical
        _LOG(this,D3_DV_GRP0_LEV4,"SetIndices - IDENTICAL - real IndexBuffer %p", IBuf);
        return D3D_OK;
    } else {    //need to set new ones
        _LOG(this,D3_DV_GRP0_LEV4,"SetIndices - NEWVALUES - real IndexBuffer %p", IBuf);
        HRESULT result = OwnerEngine->GetGraphicsManager()->Direct3DDevice->SetIndices(IBuf);
        if (result!=D3D_OK) {
            _WARN(this,ErrMgr->TNoSupport,"SetIndices - FAILED - IndexBuffer %p",IBuf);
            GuardIndices->Ibuf = NULL;
        } else {
            GuardIndices->Ibuf = IBuf;
        }
        return result;
    }
}

void CMeshManager::DeviceLost(CRenderSettings &Settings) {
    int logE = _LOGB(this,D3_DV_GRP2_LEV0,"Sending device lost");
    //Then unset vertex and index buffers
    int StreamCount = OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.MaxStreams;
    for (int i=0; i<StreamCount; i++) {
        SetStreamSource(i,NULL,0,0);
    }
    SetIndices(NULL);
    CResourceManager::DeviceLost(Settings);
    _LOGE(logE,"Sended device lost");
}

CMeshManager::CStreamInfo::CStreamInfo() {
    StreamNumber = 0;
    VBuf = NULL;
    Stride = 0;
}

CMeshManager::CStreamInfo::~CStreamInfo() {
    if (VBuf!=NULL) {
        _WARN("Guard","CStreamInfo",ErrMgr->TMemory,"VertexBuffer %p still referenced at stream %i", VBuf, StreamNumber);
    }
}

CMeshManager::CIndicesInfo::CIndicesInfo() {
    Ibuf = NULL;
}

CMeshManager::CIndicesInfo::~CIndicesInfo() {
    if (Ibuf!=NULL) {
        _WARN("Guard","CIndicesInfo",ErrMgr->TMemory,"IndexBuffer %p still referenced", Ibuf);
    }
}
