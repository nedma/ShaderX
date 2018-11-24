#include "stdafx.h"
#include "eng-meshd3f.h"
#include "FIL-FileManager.h"
#include "ENG-Engine.h"
#include "MGR-DirectGraphicsManager.h"
#include "DG-DirectGraphicsDeviceInfo.h"
#include "ENG-EngineDefaults.h"
#include "sc-DynamicArray.h"
#include "eng-MeshPart.h"


/****
Copied from the exporter
***/

#pragma pack(push, oldpack)
#pragma pack(1)

static const char   MeshHeader[] = "Dimension3 Mesh File";
static const float  MeshVersion = 2.0f;

typedef struct {
    UINT    MaterialID;
    UINT    MinVertIndex;
    UINT    MaxVertIndex;
    UINT    StartFaceIndex;
    UINT    FaceCount;
} ExportedMeshSubmesh;

typedef struct {
    UINT    Vertex[3];
} ExportedMeshFace;

typedef struct {
    float       Position[3];
    float       Normal[3];
    float       Map[2];
} ExportedMeshVertex;

typedef struct {
    char        Header[32];             //Header magic string "Dimension3 Mesh File" padded to 32 with zeroes
    float       Version;                //Version of mesh
    float       BSphereRadius;          //Center of bounding sphere
    float       BSphereCenter[3];       //x,y,z coordinates of bounding sphere
    UINT        NameLength;             //length of zero terminated name buffer (including zero at the end! strlen(Name) = NameLength-1)
    UINT        VertexCount;            //Number of saved verticles
    UINT        FaceCount;              //Number of saved faces
    UINT        SubMeshCount;           //Number of saved submeshes
} ExportedMeshHeader;


#pragma pack(pop, oldpack)

CMeshD3F::CMeshD3F(char *sObjName, CEngine *pOwnerEngine) : CMesh(sObjName, pOwnerEngine) {
}

bool CMeshD3F::Load(DWORD LoadIndex, CRenderSettings &RenderSettings) {
    this->LoadIndex = LoadIndex;
    int logE = _LOGB(this,D3_DV_GRP2_LEV2,"Loading Dimension3 BIN Mesh from resource %i",LoadIndex);
    bool result = TRUE;
    CFileManager *fil = OwnerEngine->GetFileManager();
    int ResSize = fil->GetSize(LoadIndex);
    if (ResSize<0) {
        _WARN(this,ErrMgr->TFile,"Resource not found in library");
        result = FALSE;
    } else {
        //------------------------------------
        //Stage 0 - Unload (possibly) used memory
        this->ReleaseBuffers(TRUE);

        //------------------------------------
        //Stage 1 - Load from media
        int logEE = _LOGB(this,D3_DV_GRP2_LEV2,"Reading from disk",LoadIndex);
        ExportedMeshHeader MeshHeader;
        memset(&MeshHeader, 0, sizeof(MeshHeader));
        char *MeshName = NULL;
        ExportedMeshVertex *MeshVertexes = NULL;
        ExportedMeshFace *MeshFaces = NULL;
        ExportedMeshSubmesh *MeshSubsets = NULL;

        if (fil->Open(LoadIndex)) {
            fil->Read(LoadIndex, &MeshHeader, sizeof(ExportedMeshHeader));
            MeshName = (char *)MemMgr->AllocMem(MeshHeader.NameLength,0);
            fil->Read(LoadIndex, MeshName, MeshHeader.NameLength);
            MeshVertexes = (ExportedMeshVertex *)MemMgr->AllocMem(MeshHeader.VertexCount * sizeof(ExportedMeshVertex),0);
            fil->Read(LoadIndex, MeshVertexes, MeshHeader.VertexCount * sizeof(ExportedMeshVertex));
            MeshFaces = (ExportedMeshFace *)MemMgr->AllocMem(MeshHeader.FaceCount * sizeof(ExportedMeshFace),0);
            fil->Read(LoadIndex, MeshFaces, MeshHeader.FaceCount * sizeof(ExportedMeshFace));
            MeshSubsets = (ExportedMeshSubmesh *)MemMgr->AllocMem(MeshHeader.SubMeshCount * sizeof(ExportedMeshSubmesh),0);
            fil->Read(LoadIndex, MeshSubsets, MeshHeader.SubMeshCount * sizeof(ExportedMeshSubmesh));
            if (!fil->Close(LoadIndex)) {
                _WARN(this,ErrMgr->TFile,"Resource could not be closed");
                result = FALSE;
            }
        } else {
            _WARN(this,ErrMgr->TFile,"Resource could not be opened");
            result = FALSE;
        }
        _LOGE(logEE);
        if (result) {   //loaded
            //------------------------------------
            //Stage 2 - Assign to Mesh structures
            int logS2 = _LOGB(this,D3_DV_GRP2_LEV2,"Creating Mesh structures",LoadIndex);
            //Assign bounding sphere
            this->BoundSphereCenter.x = MeshHeader.BSphereCenter[0];
            this->BoundSphereCenter.y = MeshHeader.BSphereCenter[1];
            this->BoundSphereCenter.z = MeshHeader.BSphereCenter[2];
            this->BoundSphereRadius = MeshHeader.BSphereRadius;

            //first we need to know, if our hardware is capable enough to hold these verticles.
            if ((DWORD)MeshHeader.VertexCount > this->OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.MaxPrimitiveCount ||
                (DWORD)MeshHeader.VertexCount > this->OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.MaxVertexIndex) {
                    _WARN(this,ErrMgr->TNoSupport,"Model has %u verticles, but device supports only %u primitives and %u vertexes",
                        MeshHeader.VertexCount,
                        this->OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.MaxPrimitiveCount,
                        this->OwnerEngine->GetGraphicsManager()->SelectedDeviceInfo->DevCaps.MaxVertexIndex);
                result = FALSE;
            } else {    //the good way
                //Create one huge vertex buffer and fill it (might need optimalization later)
                CMeshPart *Buf = (CMeshPart *)new CMeshPart;
                Buf->VertexBufferCreate(OwnerEngine->GetGraphicsManager()->Direct3DDevice,
                                        MeshHeader.VertexCount * sizeof(D3F_VERTEX), 
                                        OwnerEngine->GetEngineDefaults()->VBuf_Usage, 
                                        0, OwnerEngine->GetEngineDefaults()->VBuf_Pool);
                D3F_VERTEX *buf = (D3F_VERTEX *)Buf->VertexLock(0,MeshHeader.VertexCount * sizeof(D3F_VERTEX),0);
                double minx=0,maxx=0,miny=0,maxy=0,minz=0,maxz=0;
                for (UINT i=0; i<MeshHeader.VertexCount; i++) {
                    buf[i].Position.x = MeshVertexes[i].Position[0];
                    buf[i].Position.y = MeshVertexes[i].Position[1];
                    buf[i].Position.z = MeshVertexes[i].Position[2];
                    buf[i].Normal.x = MeshVertexes[i].Normal[0];
                    buf[i].Normal.y = MeshVertexes[i].Normal[1];
                    buf[i].Normal.z = MeshVertexes[i].Normal[2];
                    buf[i].Texture.x = MeshVertexes[i].Map[0];
                    buf[i].Texture.y = 1.0f - MeshVertexes[i].Map[1];
                    if (i==0 || MeshVertexes[i].Position[0]<minx) minx = MeshVertexes[i].Position[0];
                    if (i==0 || MeshVertexes[i].Position[1]<miny) miny = MeshVertexes[i].Position[1];
                    if (i==0 || MeshVertexes[i].Position[2]<minz) minz = MeshVertexes[i].Position[2];
                    if (i==0 || MeshVertexes[i].Position[0]>maxx) maxx = MeshVertexes[i].Position[0];
                    if (i==0 || MeshVertexes[i].Position[1]>maxy) maxy = MeshVertexes[i].Position[1];
                    if (i==0 || MeshVertexes[i].Position[2]>maxz) maxz = MeshVertexes[i].Position[2];
                }
                _LOG(this,D3_DV_GRP2_LEV2,"MinMax X : %f, %f",minx,maxx);
                _LOG(this,D3_DV_GRP2_LEV2,"MinMax Y : %f, %f",miny,maxy);
                _LOG(this,D3_DV_GRP2_LEV2,"MinMax Z : %f, %f",minz,maxz);
                Buf->VertexUnlock();
                //create one huge index buffer
                if (MeshHeader.FaceCount * 3>0xFFFF) {
                    Buf->IndexBufferCreate(OwnerEngine->GetGraphicsManager()->Direct3DDevice,
                                            MeshHeader.FaceCount * 3 * sizeof(DWORD),
                                            OwnerEngine->GetEngineDefaults()->IBuf_Usage,
                                            D3DFMT_INDEX32, OwnerEngine->GetEngineDefaults()->IBuf_Pool);
                    DWORD *buf = (DWORD *)Buf->IndexLock(0,MeshHeader.FaceCount * 3 * sizeof(DWORD),0);
                    for (UINT k=0; k< MeshHeader.FaceCount; k++) {
                        buf[3*k + 0] = (DWORD)MeshFaces[k].Vertex[0];
                        buf[3*k + 1] = (DWORD)MeshFaces[k].Vertex[1];
                        buf[3*k + 2] = (DWORD)MeshFaces[k].Vertex[2];
                    }
                    Buf->IndexUnlock();
                } else {
                    Buf->IndexBufferCreate(OwnerEngine->GetGraphicsManager()->Direct3DDevice,
                                            MeshHeader.FaceCount * 3 * sizeof(WORD),
                                            OwnerEngine->GetEngineDefaults()->IBuf_Usage,
                                            D3DFMT_INDEX16, OwnerEngine->GetEngineDefaults()->IBuf_Pool);
                    WORD *buf = (WORD *)Buf->IndexLock(0,MeshHeader.FaceCount * 3 * sizeof(WORD),0);
                    for (UINT k=0; k< MeshHeader.FaceCount; k++) {
                        buf[3*k + 0] = (WORD)MeshFaces[k].Vertex[0];
                        buf[3*k + 1] = (WORD)MeshFaces[k].Vertex[1];
                        buf[3*k + 2] = (WORD)MeshFaces[k].Vertex[2];
                    }
                    Buf->IndexUnlock();
                }

                //Create texture space basis vectors (Tangent space)
                CreateBasisVectors(Buf->GetVertexBuffer(), Buf->GetIndexBuffer());
                CMeshPart *OriginBuf = Buf;
                for (UINT s=0;s<MeshHeader.SubMeshCount; s++) {
                    if (s!=0) {
                        Buf = (CMeshPart *)new CMeshPart;
                        Buf->VertexUseExisting(*OriginBuf);
                        Buf->IndexUseExisting(*OriginBuf);
                    }
                    Buf->SetStrideSize(sizeof(D3F_VERTEX));
                    Buf->SetBaseVertexIndex(0);
                    Buf->SetPrimitiveType(D3DPT_TRIANGLELIST);
                    Buf->SetMinIndex(MeshSubsets[s].MinVertIndex);
                    Buf->SetNumVerticles(MeshSubsets[s].MaxVertIndex - MeshSubsets[s].MinVertIndex + 1);
                    Buf->SetStartIndex(MeshSubsets[s].StartFaceIndex * 3);
                    Buf->SetPrimitiveCount(MeshSubsets[s].FaceCount);
                    Buf->SetAppData(MeshSubsets[s].MaterialID);
                    this->AddSubset(*Buf);
                }
                this->SetFilled(true);
                this->SetRestorable(true);
            }
            _LOGE(logS2);
        }
        if (MeshName) MemMgr->FreeMem(MeshName);
        if (MeshVertexes) MemMgr->FreeMem(MeshVertexes);
        if (MeshFaces) MemMgr->FreeMem(MeshFaces);
        if (MeshSubsets) MemMgr->FreeMem(MeshSubsets);
    }
    if (result) {
        _LOGE(logE,"Ending with success");
    } else {
        _LOGE(logE,"Ending with failure");
    }
    return result;
}

bool CMeshD3F::Update(CRenderSettings &RenderSettings) {
    return TRUE;
}

#define SMALL_FLOAT 1e-12
#define WORD_MAX	65535
///////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// Creates basis vectors, based on a vertex and index list.
// NOTE: Assumes an indexed triangle list, with vertices of type Dot3Vertex
// Does not check for degenerate vertex positions - ie vertices with same 
//  position but different texture coords or normals.  Another function 
//  can do this to average the basis vectors & "smooth" the tangent space
//  for those duplicated vertices
HRESULT CMeshD3F::CreateBasisVectors(LPDIRECT3DVERTEXBUFFER9 pVertexBuffer, LPDIRECT3DINDEXBUFFER9 pIndexBuffer) {
	DWORD i;
	D3F_VERTEX* pVertices;
	void* pIndices;
	HRESULT hr;
	
	// Calculate number of vertices and indices
	D3DVERTEXBUFFER_DESC VBDesc;
	D3DINDEXBUFFER_DESC IBDesc;
	
	pVertexBuffer->GetDesc(&VBDesc);
	pIndexBuffer->GetDesc(&IBDesc);

	DWORD dwNumIndices;
	DWORD dwNumVertices;
	switch(IBDesc.Format)
	{
		case D3DFMT_INDEX16:
			dwNumIndices = IBDesc.Size / 2;
			break;
		case D3DFMT_INDEX32:
			dwNumIndices = IBDesc.Size / 4;
			break;
		default:
			return E_FAIL;
	}

	dwNumVertices = VBDesc.Size / sizeof(D3F_VERTEX);

	// Get a pointer to the indices and the vertices
	hr = pVertexBuffer->Lock(0, 0, (VOID**)&pVertices, 0);
	if (FAILED(hr))
		return hr;

	hr = pIndexBuffer->Lock(0, 0, (VOID**)&pIndices, 0);
	if (FAILED(hr))
		return hr;

	// Clear the basis vectors
	for (i = 0; i < dwNumVertices; i++)
	{
		pVertices[i].S = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
//		pVertices[i].T = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	// Walk through the triangle list and calculate gradiants for each triangle.
	// Sum the results into the S and T components.
    for( i = 0; i < dwNumIndices; i += 3 )
    {       
		DWORD TriIndex[3];
		D3DXVECTOR3 du, dv;
		D3DXVECTOR3 edge01;
		D3DXVECTOR3 edge02;
		D3DXVECTOR3 cp;
		
		switch(IBDesc.Format)
		{
		case D3DFMT_INDEX16:
			TriIndex[0] = ((WORD *)pIndices)[i];
			TriIndex[1] = ((WORD *)pIndices)[i+1];
			TriIndex[2] = ((WORD *)pIndices)[i+2];
			break;
		case D3DFMT_INDEX32:
			TriIndex[0] = ((DWORD *)pIndices)[i];
			TriIndex[1] = ((DWORD *)pIndices)[i+1];
			TriIndex[2] = ((DWORD *)pIndices)[i+2];
			break;
		}

		//assert((TriIndex[0] < dwNumVertices) && (TriIndex[1] < dwNumVertices) && (TriIndex[2] < dwNumVertices));

		D3F_VERTEX& v0 = pVertices[TriIndex[0]];
		D3F_VERTEX& v1 = pVertices[TriIndex[1]];
		D3F_VERTEX& v2 = pVertices[TriIndex[2]];

		// x, s, t
		edge01 = D3DXVECTOR3( v1.Position.x - v0.Position.x, v1.Texture.x - v0.Texture.x, v1.Texture.y - v0.Texture.y );
		edge02 = D3DXVECTOR3( v2.Position.x - v0.Position.x, v2.Texture.x - v0.Texture.x, v2.Texture.y - v0.Texture.y );

		D3DXVec3Cross(&cp, &edge01, &edge02);
		if ( fabs(cp.x) > SMALL_FLOAT )
		{
			v0.S.x += -cp.y / cp.x;
//			v0.T.x += -cp.z / cp.x;

			v1.S.x += -cp.y / cp.x;
//			v1.T.x += -cp.z / cp.x;
			
			v2.S.x += -cp.y / cp.x;
//			v2.T.x += -cp.z / cp.x;
		}

		// y, s, t
		edge01 = D3DXVECTOR3( v1.Position.y - v0.Position.y, v1.Texture.x - v0.Texture.x, v1.Texture.y - v0.Texture.y );
		edge02 = D3DXVECTOR3( v2.Position.y - v0.Position.y, v2.Texture.x - v0.Texture.x, v2.Texture.y - v0.Texture.y );

		D3DXVec3Cross(&cp, &edge01, &edge02);
		if ( fabs(cp.x) > SMALL_FLOAT )
		{
			v0.S.y += -cp.y / cp.x;
//			v0.T.y += -cp.z / cp.x;

			v1.S.y += -cp.y / cp.x;
//			v1.T.y += -cp.z / cp.x;
			
			v2.S.y += -cp.y / cp.x;
//			v2.T.y += -cp.z / cp.x;
		}

		// z, s, t
		edge01 = D3DXVECTOR3( v1.Position.z - v0.Position.z, v1.Texture.x - v0.Texture.x, v1.Texture.y - v0.Texture.y );
		edge02 = D3DXVECTOR3( v2.Position.z - v0.Position.z, v2.Texture.x - v0.Texture.x, v2.Texture.y - v0.Texture.y );

		D3DXVec3Cross(&cp, &edge01, &edge02);
		if ( fabs(cp.x) > SMALL_FLOAT )
		{
			v0.S.z += -cp.y / cp.x;
//			v0.T.z += -cp.z / cp.x;

			v1.S.z += -cp.y / cp.x;
//			v1.T.z += -cp.z / cp.x;
			
			v2.S.z += -cp.y / cp.x;
//			v2.T.z += -cp.z / cp.x;
		}
    }


	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	// Before normalizing the S, T vectors we have to 
	//  find degenerate vertices to average the basis
	//  for points which share position but not UV
	//  coordinate (ie lathed objects with a seam where
	//  vertices are duplicated).
	// If we were to search for these degenerate points after 
	//  normalizing the S & T, the averaging of these normalized
	//  (pre-averaged) vectors would not be correct over the
	//  seam, and a discontinuity might appear.

/*		
	FindAndFixDegenerateVertexBasis( pVertices, dwNumVertices, pIndices, dwNumIndices, 
		false, (IBDesc.Format == D3DFMT_INDEX32));
  */

	////////////////////////////////////////////////
	////////////////////////////////////////////////

    // Calculate the SxT vector
  	for(i = 0; i < dwNumVertices; i++)
  	{		
  		// Normalize the S, T vectors
  		D3DXVec3Normalize(&pVertices[i].S, &pVertices[i].S);
//  		D3DXVec3Normalize(&pVertices[i].T, &pVertices[i].T);
  
  		// Get the cross of the S and T vectors
//  		D3DXVec3Cross(&pVertices[i].SxT, &pVertices[i].S, &pVertices[i].T);
  
  		// Need a normalized normal
//  		D3DXVec3Normalize(&pVertices[i].Normal, &pVertices[i].Normal);
    		
  		// Get the direction of the SxT vector
//  		if (D3DXVec3Dot(&pVertices[i].SxT, &pVertices[i].Normal) < 0.0f)
//  		{
//  			pVertices[i].SxT = -pVertices[i].SxT;
//  		}
  	}

	pVertexBuffer->Unlock();
	pIndexBuffer->Unlock();

	return S_OK;
}

/*
////////////////////////////////////////////////////////////////////////////
// FindAndFixDegenerateVertexBasis(..)
//  Takes a vertex buffer with vertices of type Dot3Vertex.  Indices must be
//  for an INDEXED TRIANGLE LIST.
// This finds vertices within epsilon in position and averages their tangent
//  bases to make a smooth tangent space across the model.  This is useful for
//  lathed objects or authored models which duplicate vertices along material
//  boundaries.
// Tangent Basis must have already been computed for this to work! =)
//
// If bUnifyNormals is true, then the vertex normals are averaged too
//
HRESULT CMeshD3F::FindAndFixDegenerateVertexBasis(	D3F_VERTEX	* pVertices,
											DWORD		  dwNumVertices,
											void		* pIndices,
											DWORD		  dwNumIndices,
											bool bUnifyNormals,
											bool is32BitIndices) {

	//assert( pVertices != NULL );
	//assert( pIndices != NULL );


	float epsilon = 1.0e-5f;
	float x,y,z,dist;

	////////////////////////////////////////////////////////////////

	DWORD i,j;

	////////////////////////////////////////////////////////////////
	// Sloppy, but allocate a pointer and char for each vertex
	// As overlapping vertices are found, increment their duplicate_count
	//   and allocate an array of MAX_OVERLAP vertex indices to reference
	//   which vertices overlap.

	#define MAX_OVERLAP 50

	BYTE * duplicate_count = new BYTE[ dwNumVertices ];
		// duplicate_index is array of pointers to bins.  Each bin is
		// allocated when a match is found.
	WORD ** duplicate_index = (WORD**) calloc( dwNumVertices, sizeof(WORD*) );

	memset( duplicate_count, 0, dwNumVertices * sizeof( BYTE  ) );
	memset( duplicate_index, 0, dwNumVertices * sizeof( WORD* ) );

	// Need to search the mesh for vertices with the same spatial coordinate
	// These are vertices duplicated for lathed/wrapped objects to make a
	//   2nd set of texture coordinates at the point in order to avoid bad
	//   texture wrapping
	// In short:  For each vertex, find any other vertices that share it's 
	//   position.  "Average" the tangent space basis calculated above for
	//   these duplicate vertices.  This is not rigorous, but works well 
	//   to fix up authored models.  ** Models should not have T juntions! **

	// Check each vert with every other.  There's no reason to check
	//   j with i after doing i with j, so start j from i ( so we test
	//   1 with 2 but never 2 with 1 again).
	// This is a model pre-processing step and only done once.  For large
	//   models, compute this off-line if you have to and store the resultant
	//   data.
	// The whole thing could be made much more efficient (linked list, etc)

	for( i=0; i < dwNumVertices; i++ )
	{
		for(j=i+1; j < dwNumVertices; j++ )
		{
			x = pVertices[i].Position.x - pVertices[j].Position.x;
			y = pVertices[i].Position.y - pVertices[j].Position.y;
			z = pVertices[i].Position.z - pVertices[j].Position.z;

			dist = x*x + y*y + z*z;
			
			if( dist < epsilon )
			{
				// if i matches j and k, just record into i.  j will be 
				//  half full as it will only match k, but this is
				//  taken care of when i is processed.
				if( duplicate_count[i] == 0 )
				{
					// allocate bin
					duplicate_index[i] = new WORD[MAX_OVERLAP];
				}
				if( duplicate_count[i] < MAX_OVERLAP )
				{
					//assert( j < WORD_MAX );
					duplicate_index[i][duplicate_count[i]] = (WORD) j;
					duplicate_count[i] ++;
				}
				else
				{
                    _WARN(this,ErrMgr->TMemory,"Ran out of bin storage!");
					//assert( false );
				}
			}
		}

//
//		if( duplicate_count[i] > 0 )
//		{
//			FDebug("Vertex %d\t matches: ", i );
//			for(j=0; j < duplicate_count[i]; j++ )
//			{
//				FDebug("%d\t", duplicate_index[i][j] );
//			}
//			FDebug("\n");
//		}
//
	}

	// Now average the tangent spaces & write the new result to
	//  each duplicated vertex


	D3DXVECTOR3	S_temp, T_temp, SxT_temp, N_temp;


	for( i = 0; i < dwNumVertices; i++ )
	{
		// do < 10 check to not average the basis at poles of sphere or
		//  other ridiculous geometry with too many degenerate vertices

		if( duplicate_count[i] > 0 && duplicate_count[i] < 10 )
		{
			//	FDebug("Averaging vert prop at %d for %d vertices\n", i, duplicate_count[i]);

			// If there are more than 5 vertices sharing this point then
			//  the point is probably some kind of lathe axis node.  No need to
			//  process it here

			// Set accumulator to value for vertex in question

			S_temp		= pVertices[i].S;
			T_temp		= pVertices[i].T;
			SxT_temp	= pVertices[i].SxT;
			N_temp		= pVertices[i].Normal;

			// add in basis vectors for all other vertices which
			//  have the same positon (found above)

			for( j=0; j < duplicate_count[i]; j++ )
			{
				S_temp		= S_temp   + pVertices[duplicate_index[i][j]].S;
				T_temp		= T_temp   + pVertices[duplicate_index[i][j]].T;
				SxT_temp	= SxT_temp + pVertices[duplicate_index[i][j]].SxT;

				N_temp		= N_temp   + pVertices[duplicate_index[i][j]].Normal;
			}

			// Normalize the basis vectors
			// Note that SxT might not be perpendicular to S and T
			//  anymore.  Not absolutely necessary to re-do the 
			//  cross product.

			D3DXVec3Normalize( & S_temp, & S_temp );
			D3DXVec3Normalize( & T_temp, & T_temp );
			D3DXVec3Normalize( & SxT_temp, & SxT_temp );
			D3DXVec3Normalize( & N_temp, & N_temp );

			// Write the average basis to the first vertex for which
			//   the duplicates were found

			pVertices[i].S = S_temp;
			pVertices[i].T = T_temp;
			pVertices[i].SxT = SxT_temp;

			if( bUnifyNormals )
				pVertices[i].Normal = N_temp;

			// Now write to all later vertices with the same position
				
			for(j=0; j < duplicate_count[i]; j++ )
			{
				// Set the vertices in the same position to
				//  the average basis.

				pVertices[duplicate_index[i][j]].S = S_temp;
				pVertices[duplicate_index[i][j]].T = T_temp;
				pVertices[duplicate_index[i][j]].SxT = SxT_temp;

				if( bUnifyNormals )
					pVertices[duplicate_index[i][j]].Normal = N_temp;


				// Kill the duplicate index lists of all vertices of
				//  higher index which overlap this one.  This is so
				//  higher index vertices do not average a smaller 
				//  subset of bases.
				// Arrays are de-allocated later

				duplicate_count[ duplicate_index[i][j] ] = 0;

			}

		}

		if( duplicate_index[i] != NULL )
		{
			delete [] duplicate_index[i];
			duplicate_index[i] = NULL;
			duplicate_count[i] = 0;
		}
	}

	delete [] duplicate_count;
	free( duplicate_index );


	return( S_OK );
}
*/


