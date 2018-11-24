#include "stdafx.h"

#include "QuadMesh.h"


CQuadMesh::CQuadMesh( const CResourceId& fxID )
:	CRenderableMesh( *RGET_MESH("billboard"), 0 )
{
	getParams().setEffect( *RGET_FX(fxID) );
}
