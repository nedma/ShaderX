#include "stdafx.h"
#include "SimpleMesh.h"


CSimpleMesh::CSimpleMesh( const CResourceId& meshID )
:	mMeshID(meshID)
{
	mAABB = RGET_MESH(meshID)->getTotalAABB();
}

void CSimpleMesh::render( eRenderMode mode )
{
	if( preRender() ) {
		TMeshVector& vv = mParts[mode];
		int n = vv.size();
		for( int i = 0; i < n; ++i ) {
			CRenderableMesh* r = vv[i];
			G_RENDERCTX->attach( *r );
		}
	}
}

void CSimpleMesh::addMeshPart( int attrID, const CResourceId& texID )
{
	CMesh* mesh = RGET_MESH(mMeshID);
	CD3DTexture& tex = *RGET_TEX(texID);

	CRenderableMesh* m;
	m = new CRenderableMesh( *mesh, attrID, &mMatrix.getOrigin() );
	mParts[BASE].push_back( m );
	m->getParams().setEffect( *RGET_FX("mesh") );
	m->getParams().addMatrix4x4Ref( "mWorld", mMatrix );
	m->getParams().addMatrix4x4Ref( "mWorldView", mWorldView );
	m->getParams().addMatrix4x4Ref( "mWVP", mWVP );
	m->getParams().addTexture( "tBase", tex );

	m = new CRenderableMesh( *mesh, attrID, &mMatrix.getOrigin() );
	mParts[COLOR].push_back( m );
	m->getParams().setEffect( *RGET_FX("meshColorOnly") );
	m->getParams().addMatrix4x4Ref( "mWorld", mMatrix );
	m->getParams().addMatrix4x4Ref( "mWorldView", mWorldView );
	m->getParams().addMatrix4x4Ref( "mWVP", mWVP );
	m->getParams().addTexture( "tBase", tex );
	
	m = new CRenderableMesh( *mesh, attrID, &mMatrix.getOrigin() );
	mParts[NORMALZ].push_back( m );
	m->getParams().setEffect( *RGET_FX("fallbackNormalZ") );
	m->getParams().addMatrix4x4Ref( "mWorld", mMatrix );
	m->getParams().addMatrix4x4Ref( "mWVP", mWVP );

	m = new CRenderableMesh( *mesh, attrID, &mMatrix.getOrigin() );
	mParts[SHADOW].push_back( m );
	m->getParams().setEffect( *RGET_FX("meshShadow") );
	m->getParams().addMatrix4x4Ref( "mWVP", mWVP );
}
