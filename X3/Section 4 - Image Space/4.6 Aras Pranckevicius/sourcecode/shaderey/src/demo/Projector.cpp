#include "stdafx.h"

#include "Projector.h"


void CProjector::computeTextureProjection( const SMatrix4x4& projectorMatrix, SMatrix4x4& dest )
{
	const CRenderCamera& camera = G_RENDERCTX->getCamera();

    // | -0.5     0        0        0 |
    // | 0        0.5      0        0 |
    // | 0        0        0        0 |
    // | 0.5      0.5      1        1 |
	SMatrix4x4 matTexScale;
	matTexScale.identify();
	matTexScale._11 = 0.5f;
	matTexScale._22 = -0.5f;
	matTexScale._33 = 0.0f;
	matTexScale.getOrigin().set( 0.5f, 0.5f, 1.0f );

	SMatrix4x4 matTmp = camera.getProjectionMatrix() * matTexScale;
	matTmp = projectorMatrix * matTmp;
	dest = camera.getViewMatrix() * matTmp;
}
