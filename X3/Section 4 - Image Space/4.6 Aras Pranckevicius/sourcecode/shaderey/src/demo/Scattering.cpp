#include "stdafx.h"
#include "Scattering.h"


void CSun::recalcParams()
{
	// direction
	mDirection.y = cosf( mTheta );
	mDirection.x = sinf( mTheta ) * cosf( mPhi );
	mDirection.z = sinf( mTheta ) * sinf( mPhi );
	// color
	recalcColor();
	mColor.w = mIntensity;
	mColorMulIntensity = SVector3(mColor.x,mColor.y,mColor.z) * mIntensity;
}


void CSun::recalcColor( int turbidity )
{
	// ratio of small to large particle sizes (0:4,usually 1.3)
	float fAlpha = 1.3f;
	// amount of aerosols present
	float fBeta = 0.04608365822050f * turbidity - 0.04586025928522f;
	// relative optical mass
	float m = 1.0f / ( cosf(mTheta) + 0.15f * powf(93.885f-mTheta,-1.253f) );
	
	SVector3 lambda( 0.65f, 0.57f, 0.475f ); // R,G,B wavelens in um
	
	for( int i = 0; i < 3; ++i ) {
		// Rayleigh scattering
		float tauR = expf( -m * 0.008735f * powf( lambda[i], -4.08f ) );
		// Aerosal (water + dust) attenuation
		float tauA = expf( -m * fBeta * powf( lambda[i], -fAlpha ) );
		mColor[i] = tauR * tauA;
	}
}

/*
void CSun::lerp( const CSun& a, const CSun& b, float f )
{
	(*this) = *one;
	mIntensity = one->mIntensity * (1-f) + two->mIntensity * f;
	
	// Do better job of interpolating direction.
	SVector4 dir = one->getDirection() * (1-f) + two->getDirection() * f;
	SVector3 dirn, dir3(dir[0],dir[1],dir[2]);
	D3DXVec3Normalize(&dirn, &dir3);
	float horiz = sqrt(dirn[0]*dirn[0] + dirn[2]*dirn[2]);
	mTheta = D3DX_PI/2-atan2(dirn[1], horiz);
	mPhi = atan2(dirn[2],dirn[0]);
	
}
CSun CSun::operator + ( const CSun& s2) const
{
	CSun res;
	res.mIntensity = mIntensity + s2.mIntensity;
	res.mTheta = mTheta + s2.mTheta;
	res.mPhi = mPhi + s2.mPhi;
	return res;
}
CSun CSun::operator * ( float f) const
{
	CSun res;
	res.mIntensity = mIntensity * f;
	res.mTheta = mTheta * f;
	res.mPhi = mPhi * f;
	return res;
}
CSun operator * ( float f, const CSun& s )
{
	CSun res;
	res.mIntensity = s.mIntensity * f;
	res.mTheta = s.mTheta * f;
	res.mPhi = s.mPhi * f;
	return res;
}
*/

CAtmosphere::CAtmosphere()
{	
	mExtinctionMult = 2.0f; // 1.0f
	mInscatteringMult = 0.7f; // 0.3f
	setHGg( 0.8f );
	mBetaRayMult = 0.2f; // 0.2f
	mBetaMieMult = 0.01f; // 0.01f
	calculateScattering();
	calculateMuls();
}

void CAtmosphere::calculateMuls()
{
	mMulBetaRay = mBetaRay * mBetaRayMult;
	mMulBetaMie = mBetaMie * mBetaMieMult;
	mMulBetaRayMieSum = mMulBetaRay + mMulBetaMie;
	mMulInvBetaRayMieSum.set( 1.0f / mMulBetaRayMieSum.x, 1.0f / mMulBetaRayMieSum.y, 1.0f / mMulBetaRayMieSum.z );
	mMulBetaRay_ = mMulBetaRay * 3.0f / (16.0f * D3DX_PI);
	mMulBetaMie_ = mMulBetaMie * 1.0f / (4.0f * D3DX_PI);
}

void CAtmosphere::calculateScattering()
{
	const float n = 1.003f; // refractive index
	const float N = 2.545e25f;
	const float pn = 0.035f;
	const float PIPI = D3DX_PI * D3DX_PI;
	const float PI2 = D3DX_PI * 2.0f;
	const float PI2PI2 = PI2*PI2;

	SVector3 lambda( 1.0f/650.0e-9f, 1.0f/570.0e-9f, 1.0f/475.0e-9f ); // inv. wavelens
	SVector3 lambda2( lambda.x*lambda.x, lambda.y*lambda.y, lambda.z*lambda.z );
	SVector3 lambda4( lambda2.x*lambda2.x, lambda2.y*lambda2.y, lambda2.z*lambda2.z );
	
	// Rayleigh scattering constants
	float fTemp = PIPI * (n*n-1)*(n*n-1) * (6+3*pn) / (6-7*pn) / N;
	float fBeta = 8 * fTemp * D3DX_PI / 3;
	mBetaRay = fBeta * lambda4;
	//float fBetaDash = fTemp/2;
	//mBetaDashRay = fBetaDash * lambda4;
	
	// Mie scattering constants.
	const float T = 2.0f;
	float c = (6.544*T - 6.51) * 1e-17;
	//float fTemp2 = 0.434 * c * PI2PI2 * 0.5f;
	//mBetaDashMie = fTemp2 * lambda2;
	
	SVector3 K(0.685f, 0.679f, 0.670f);
	float fTemp3 = 0.434f * c * D3DX_PI * PI2PI2;
	SVector3 vBetaMieTemp( K.x*lambda2.x, K.y*lambda2.y, K.z*lambda2.z );
	mBetaMie = fTemp3 * vBetaMieTemp;
}
