#ifndef __BASE_ENTITY_H
#define __BASE_ENTITY_H

#include <dingus/math/AABox.h>
#include <dingus/renderer/RenderableMesh.h>


class CBaseEntity {
public:
	/**
	 *  Frustum-culls, if potentially visible then computes WorldView
	 *  and WVP matrices.
	 *  @return true if potentially visible.
	 */
	bool preRender() {
		const CRenderCamera& cam = G_RENDERCTX->getCamera();
		if( !mAABB.frustumCull( mMatrix, cam.getViewProjMatrix() ) ) {
			mWorldView = mMatrix * cam.getViewMatrix();
			mWVP = mWorldView * cam.getProjectionMatrix();
			return true;
		}
		return false;
	}

public:
	SMatrix4x4	mMatrix;
	SMatrix4x4	mWorldView;
	SMatrix4x4	mWVP;
	CAABox		mAABB;
};


#endif
