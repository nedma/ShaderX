#ifndef __CAMERA_ANIM_H
#define __CAMERA_ANIM_H

#include "SampledAnim.h"
#include <dingus/math/Matrix4x4.h>


class CCameraAnim {
public:
	CCameraAnim( SMatrix4x4& destMatrix );

	void load( const char* fileName );

	void animate( float t, float& destFov, float& sunTheta );

	float getLastFocus() const { return mLastFocus; }

private:
	TVector3Anim	mPosAnim;
	TQuatAnim		mRotAnim;
	TFloatAnim		mFocusAnim;

	SMatrix4x4*		mDestMatrix;
	float			mLastFocus;
};


#endif
