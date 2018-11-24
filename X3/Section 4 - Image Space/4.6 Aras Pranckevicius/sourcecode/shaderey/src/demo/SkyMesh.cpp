#include "stdafx.h"

#include "SkyMesh.h"


CSkyMesh::CSkyMesh()
{
	CMesh& m = *RGET_MESH("Skydome");
	mMesh = new CRenderableMesh( m, 0 );
	mMesh->getParams().setEffect( *RGET_FX("sky") );
}

void CSkyMesh::render()
{
	/*
	IDirect3DDevice9& dxdev = CD3DDevice::getInstance().getDevice();

	dxdev.SetFVF( D3DFVF_XYZ );
	ID3DXEffect* fx = mMesh->mParams.getEffect()->getObject();
	mMesh->mParams.applyToEffect();
	UINT passes, p;
	fx->Begin( &passes, 0 );
	for( p = 0; p < passes; ++p ) {
		fx->Pass( p );
		mMesh->render();
	}
	fx->End();
	*/
	G_RENDERCTX->attach( *mMesh );
}
