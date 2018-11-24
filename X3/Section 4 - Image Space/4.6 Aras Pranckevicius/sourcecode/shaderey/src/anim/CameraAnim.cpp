#include "stdafx.h"
#include "CameraAnim.h"


CCameraAnim::CCameraAnim( SMatrix4x4& destMatrix )
:	mPosAnim(TVector3Anim::REPEAT),
	mRotAnim(TQuatAnim::REPEAT),
	mFocusAnim(TFloatAnim::REPEAT),
	mDestMatrix(&destMatrix),
	mLastFocus(1.0f)
{
}

void CCameraAnim::load( const char* fileName )
{
	mPosAnim.clearSamples();
	mRotAnim.clearSamples();
	mFocusAnim.clearSamples();
	FILE* f = fopen( fileName, "rt" );
	if( !f ) return;

	char buf[300];
	fgets( buf, 300, f );
	while( strstr(buf,"END") == 0 ) {
		SVector3 pos;
		SQuaternion rot;
		float focus;
		sscanf( buf, "pos=%f %f %f rot=%f %f %f %f dist=%f", &pos.x, &pos.y, &pos.z, &rot.x, &rot.y, &rot.z, &rot.w, &focus );
		mPosAnim.addSample( pos );
		mRotAnim.addSample( rot );
		mFocusAnim.addSample( focus );
		fgets( buf, 300, f );
	}

	fclose( f );
}

void CCameraAnim::animate( float t, float& destFov, float& sunTheta )
{
	// get sample indices and alpha between them
	int idx1, idx2;
	float alpha;
	mPosAnim.timeToIndex( t, idx1, idx2, alpha );

	// now, hack sudden transitions :)
	if( idx1==300/2 || idx1==600/2 || idx1==800/2 || idx1==1200/2 || idx1==1300/2 || idx1==1400/2 || idx1==1600/2 )
		alpha = 0.0f;

	// sample animations
	SVector3 pos;
	SQuaternion rot;
	mPosAnim.sample( idx1, idx2, alpha, pos );
	mRotAnim.sample( idx1, idx2, alpha, rot );
	mFocusAnim.sample( idx1, idx2, alpha, mLastFocus );
	*mDestMatrix = SMatrix4x4( pos, rot );
	SMatrix4x4 mr;
	D3DXMatrixRotationX( &mr, D3DX_PI*0.5f );
	*mDestMatrix = mr * (*mDestMatrix);

	// fov
	if( idx1 <= 1300/2 )
		destFov = D3DX_PI*0.25f;
	else
		destFov = D3DX_PI*0.20f;

	// sun's theta
	float sunAlpha = 0.5f;
	float inAnim = idx1 + alpha;
	/*
	if( inAnim < 150 ) {
		sunAlpha = (inAnim / 150.0f) * 0.5f + 0.25f;
	} else if( inAnim < 300 ) {
		sunAlpha = 0.8f - (inAnim-150)/150.0f * 0.4f;
	} else if( inAnim < 400 ) {
		sunAlpha = (inAnim-300)/100.0f * 0.8f + 0.2f;
	} else if( inAnim < 600 ) {
		sunAlpha = 0.9f - (inAnim-400)/200.0f * 0.3f;
	} else if( inAnim < 650 ) {
		sunAlpha = 0.8f;
	} else if( inAnim < 700 ) {
		sunAlpha = (inAnim-650)/50.0f * 0.2f + 0.6f;
	} else {
		sunAlpha = 0.1f + (inAnim-700)/100.0f * 0.3f;
	}
	*/
	if( inAnim < 80 )
		sunAlpha = 0.3f;
	else if( inAnim < 100 )
		sunAlpha = 0.3f + (inAnim-80)/20*0.55f;
	else if( inAnim < 200 )
		sunAlpha = 0.85f;
	else if( inAnim < 220 )
		sunAlpha = 0.85f - (inAnim-200)/20 * 0.45f;
	else if( inAnim < 370 )
		sunAlpha = 0.4f;
	else if( inAnim < 390 )
		sunAlpha = 0.4f + (inAnim-370)/20 * 0.5f;
	else if( inAnim <= 400 )
		sunAlpha = 0.9f;
	else if( inAnim < 510 )
		sunAlpha = 0.75f;
	else if( inAnim < 550 )
		sunAlpha = 0.75f - (inAnim-510)/40 * 0.45f;
	else if( inAnim <= 650 )
		sunAlpha = 0.3f;
	else if( inAnim <= 700 )
		sunAlpha = 0.5f + (inAnim-650)/50 * 0.4f;
	else if( inAnim < 790 )
		sunAlpha = 0.3f;
	else
		sunAlpha = 0.3f - (inAnim-790)/10 * 0.2f;
	sunTheta = math_type_traits<float>::interpolate( -D3DX_PI*0.5f, D3DX_PI*0.5f, sunAlpha );
}
