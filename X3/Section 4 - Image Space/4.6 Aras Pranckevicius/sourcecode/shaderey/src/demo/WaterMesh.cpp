#include "stdafx.h"

#include "WaterMesh.h"


CWaterMesh::CWaterMesh( const CResourceId& meshID )
{
	CMesh& mesh = *RGET_MESH(meshID);
	mAABB = mesh.getTotalAABB();

	CRenderableMesh* m;
	m = new CRenderableMesh( mesh, 0, &mMatrix.getOrigin() );
	mMesh[BASE] = m;
	m->getParams().setEffect( *RGET_FX("water") );
	m->getParams().addMatrix4x4Ref( "mWorld", mMatrix );
	m->getParams().addMatrix4x4Ref( "mWorldView", mWorldView );
	m->getParams().addMatrix4x4Ref( "mWVP", mWVP );
	m->getParams().addTexture( "tRefl", *RGET_S_TEX(RID_LAKEREFL) );
	m->getParams().addTexture( "tWaves", *RGET_TEX("wave") );

	m = new CRenderableMesh( mesh, 0, &mMatrix.getOrigin() );
	mMesh[COLOR] = m;
	m->getParams().setEffect( *RGET_FX("waterColorOnly") );
	m->getParams().addMatrix4x4Ref( "mWorld", mMatrix );
	m->getParams().addMatrix4x4Ref( "mWorldView", mWorldView );
	m->getParams().addMatrix4x4Ref( "mWVP", mWVP );
	m->getParams().addTexture( "tRefl", *RGET_S_TEX(RID_LAKEREFL) );
	m->getParams().addTexture( "tWaves", *RGET_TEX("wave") );

	m = new CRenderableMesh( mesh, 0, &mMatrix.getOrigin() );
	mMesh[NORMALZ] = m;
	m->getParams().setEffect( *RGET_FX("waterNormalZOnly") );
	m->getParams().addMatrix4x4Ref( "mWorld", mMatrix );
	m->getParams().addMatrix4x4Ref( "mWVP", mWVP );
}

void CWaterMesh::render( eRenderMode mode )
{
	if( preRender() ) {
		G_RENDERCTX->attach( *mMesh[mode] );
	}
}
