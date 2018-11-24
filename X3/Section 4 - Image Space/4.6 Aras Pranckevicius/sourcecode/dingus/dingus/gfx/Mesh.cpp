// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// --------------------------------------------------------------------------
#include "stdafx.h"

#include "Mesh.h"
#include "../kernel/D3DDevice.h"

using namespace dingus;

CMesh::CMesh()
:	mVertexCount(0), mIndexCount(0),
	mVertexFormat(0), mVertexStride(0),
	mIndexStride(0), mVertexDecl(0),
	mVB(0), mIB(0)
{
}

CMesh::~CMesh()
{
	ASSERT_MSG( mVB.isNull() && mIB.isNull(), "mesh not cleaned up" );
}


void CMesh::createResource( int vertCount, int idxCount,
		const CVertexFormat& vertFormat, int indexStride, CD3DVertexDecl& vertDecl,
		eBufferType bufferType )
{
	ASSERT_MSG( mVB.isNull() && mIB.isNull(), "mesh not cleaned up" );
	ASSERT_MSG( vertCount > 0, "bad vertex count" );
	ASSERT_MSG( idxCount > 0, "bad index count" );
	ASSERT_MSG( indexStride==2 || indexStride==4, "bad index stride" );
	mBufferType = bufferType;
	mVertexCount = vertCount;
	mIndexCount = idxCount;
	mVertexFormat = vertFormat;
	mVertexStride = vertFormat.calcVertexSize();
	ASSERT_MSG( mVertexStride > 0, "bad vertex stride" );
	mIndexStride = indexStride;
	mVertexDecl = &vertDecl;
	ASSERT_MSG( mVertexDecl, "null vertex decl" );

	// create buffers
	CD3DDevice& dx = CD3DDevice::getInstance();
	HRESULT hr;

	DWORD usage = 0;
	D3DPOOL bufpool = (mBufferType == BUF_STATIC) ? D3DPOOL_MANAGED : D3DPOOL_SYSTEMMEM;
	if( (dx.getVertexProcessing() == CD3DDevice::VP_SW) ||
		(dx.getVertexProcessing() == CD3DDevice::VP_MIXED && vertFormat.getSkinMode() != CVertexFormat::SKIN_NONE) ||
		(dx.getVertexProcessing() == CD3DDevice::VP_MIXED && vertCount==4 && idxCount==6) ) // HACK
	{
		usage |= D3DUSAGE_SOFTWAREPROCESSING;
		bufpool = D3DPOOL_SYSTEMMEM;
	}

	IDirect3DVertexBuffer9* vb = 0;
	hr = dx.getDevice().CreateVertexBuffer( mVertexCount * mVertexStride, usage, 0, bufpool, &vb, 0 );
	assert( SUCCEEDED(hr) && vb );
	mVB.setObject( vb );
	IDirect3DIndexBuffer9* ib = 0;
	hr = dx.getDevice().CreateIndexBuffer( mIndexCount * mIndexStride, usage, mIndexStride==2?D3DFMT_INDEX16:D3DFMT_INDEX32, bufpool, &ib, 0 );
	assert( SUCCEEDED(hr) && ib );
	mIB.setObject( ib );

	// clear groups
	mGroups.clear();
}

void CMesh::deleteResource()
{
	assert( !mVB.isNull() );
	mVB.getObject()->Release();
	mVB.setObject( 0 );
	assert( !mIB.isNull() );
	mIB.getObject()->Release();
	mIB.setObject( 0 );

	// clear groups
	mGroups.clear();
}

void* CMesh::lockVB( bool readOnly )
{
	assert( !mVB.isNull() );
	void* data = 0;
	mVB.getObject()->Lock( 0, 0, &data, readOnly ? D3DLOCK_READONLY : 0 );
	assert( data );
	return data;
}

void CMesh::unlockVB()
{
	assert( !mVB.isNull() );
	mVB.getObject()->Unlock();
}

void* CMesh::lockIB( bool readOnly )
{
	assert( !mIB.isNull() );
	void* data = 0;
	mIB.getObject()->Lock( 0, 0, &data, readOnly ? D3DLOCK_READONLY : 0 );
	assert( data );
	return data;
}

void CMesh::unlockIB()
{
	assert( !mIB.isNull() );
	mIB.getObject()->Unlock();
}

void CMesh::computeAABBs()
{
	mTotalAABB.setNull();
	if( !mVertexFormat.hasPosition() )
		return;
	const char* vb = reinterpret_cast<const char*>( lockVB( true ) );
	for( int i = 0; i < mGroups.size(); ++i ) {
		CGroup& g = mGroups[i];
		g.getAABB().setNull();
		for( int v = 0; v < g.getVertexCount(); ++v ) {
			const char* vtx = vb + mVertexStride * ( v + g.getFirstVertex() );
			const SVector3& pos = *reinterpret_cast<const SVector3*>(vtx);
			g.getAABB().extend( pos );
		}
		mTotalAABB.extend( g.getAABB() );
	}
	unlockVB();

	// kind of HACK: if we're skinned, inflate total AABB by some amount
	if( mVertexFormat.getSkinMode() != CVertexFormat::SKIN_NONE ) {
		SVector3 aabbSize = mTotalAABB.getMax() - mTotalAABB.getMin();
		if( aabbSize.x < aabbSize.y ) aabbSize.x = aabbSize.y;
		if( aabbSize.z < aabbSize.y ) aabbSize.z = aabbSize.y;
		mTotalAABB.getMin() -= aabbSize;
		mTotalAABB.getMax() += aabbSize;
	}
}
