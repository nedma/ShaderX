#ifndef __SCATTERING_H
#define __SCATTERING_H

#include <dingus/math/Vector4.h>


/**
 *  Sun parameters.
 *
 *  Theta is angle from zenith (Y up is 0, Y down is PI).
 *  Phi is azimuth angle (CCW, from X axis - so Xpos is 0, Zpos is PI/2 etc.).
 */
class CSun {
public:
	CSun( float theta = 0.0f, float phi = 0.0f, float intensity = 100.0f )
		:	mTheta(theta), mPhi(phi), mIntensity(intensity) { recalcParams(); }

	float getTheta() const { return mTheta; }
	float getPhi() const { return mPhi; }
	float getIntensity() const { return mIntensity; }
	void setParams( float theta, float phi, float intensity ) {
		mTheta = theta; mPhi = phi; mIntensity = intensity;
		recalcParams();
	}

	const SVector3& getDirection() const { return mDirection; }
	const SVector4& getColor() const { return mColor; }
	const SVector3& getColorMulIntensity() const { return mColorMulIntensity; }
	
	//void lerp( const CSun& a, const CSun& b, float f );
	//CSun operator + ( const CSun&) const;
	//CSun operator * ( float ) const;
	//friend CSun operator * ( float , const CSun& );
	
private:
	void	recalcParams();
	void	recalcColor( int turbidity = 2 );

private:
	/// Angle from zenith (0 is up, PI is down).
	float mTheta;
	/// Azimuth (CCW, from X axis).
	float mPhi;
	/// Intensity
	float mIntensity;

	SVector3	mDirection;
	/// R,G,B - color, A - intensity
	SVector4	mColor;
	/// Color * intensity
	SVector3	mColorMulIntensity;
};



class CAtmosphere {
public:
	CAtmosphere();
	
	const SVector3& getMulBetaRay() const { return mMulBetaRay; }
	const SVector3& getMulBetaMie() const { return mMulBetaMie; }
	const SVector3& getMulBetaRay_() const { return mMulBetaRay_; }
	const SVector3& getMulBetaMie_() const { return mMulBetaMie_; }
	const SVector3& getMulBetaRayMieSum() const { return mMulBetaRayMieSum; }
	const SVector3& getMulInvBetaRayMieSum() const { return mMulInvBetaRayMieSum; }
	
	const SVector4& getHGg() const { return mHGg; }
	const float& getInscatteringMult() const { return mInscatteringMult; }
	const float& getExtinctionMult() const { return mExtinctionMult; }
	float getBetaRayMult() const { return mBetaRayMult; }
	float getBetaMieMult() const { return mBetaMieMult; }
	
	void setHGg( float v ) { mHGg.set( v, 1-v*v, 1+v, 2*v ); }
	void setInscatteringMult( float v ) { mInscatteringMult = v; }
	void setExtinctionMult( float v ) { mExtinctionMult = v; }
	void setBetaRayMult( float v ) { mBetaRayMult = v; calculateMuls(); }
	void setBetaMieMult( float v ) { mBetaMieMult = v; calculateMuls(); }

	//void lerp(CAtmosphere *one, CAtmosphere *two, float f);
	//CAtmosphere operator + ( const CAtmosphere&) const;
	//CAtmosphere operator * ( float ) const;
	//friend CAtmosphere operator * ( float , const CAtmosphere& );
	
private:
	void calculateScattering();
	void calculateMuls();

private:
	float mInscatteringMult;	// Multiply inscattering term with this
	float mExtinctionMult;		// Multiply extinction term with this
	float mBetaRayMult; 		// Multiply Rayleigh scattering coefficient with this
	float mBetaMieMult; 		// Multiply Mie scattering coefficient with this
	
	SVector3 mBetaRay;		// Rayleigh scattering coeff
	SVector3 mBetaMie;		// Mie scattering coeff

	SVector3 mMulBetaRay;
	SVector3 mMulBetaMie;
	SVector3 mMulBetaRay_;
	SVector3 mMulBetaMie_;
	SVector3 mMulBetaRayMieSum; // sum of mBetaRay and mBetaMie
	SVector3 mMulInvBetaRayMieSum; // 1 / mBetaRayMieSum
	SVector4 mHGg; // g value in Henyey Greenstein approximation fn. (g, (1-g)^2, 1+g*g, 2*g)
};

//CAtmosphere operator * ( float , const CAtmosphere& );


#endif
