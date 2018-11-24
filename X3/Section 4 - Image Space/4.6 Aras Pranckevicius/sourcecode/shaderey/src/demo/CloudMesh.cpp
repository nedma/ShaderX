#include "stdafx.h"

#include "CloudMesh.h"


CCloudMesh::CCloudMesh( const CResourceId& meshID )
{
	CMesh& m = *RGET_MESH(meshID);
	mAABB = m.getTotalAABB();

	const char* fxids[MAX_MODES-1] = { "cloud", "cloudColorOnly", "fallbackNormalZ" };
	for( int i = 0; i < MAX_MODES-1; ++i ) {
		mMesh[i] = new CRenderableMesh( m, 0, &mMatrix.getOrigin(), 0 );
		mMesh[i]->getParams().setEffect( *RGET_FX(fxids[i]) );
		mMesh[i]->getParams().addMatrix4x4Ref( "mWorld", mMatrix );
		mMesh[i]->getParams().addMatrix4x4Ref( "mWVP", mWVP );
	}
}

void CCloudMesh::render( eRenderMode mode )
{
	if( preRender() )
		G_RENDERCTX->attach( *mMesh[mode] );
}
