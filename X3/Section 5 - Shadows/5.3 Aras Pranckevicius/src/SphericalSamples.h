#ifndef __SPHERE_SAMPLES_H
#define __SPHERE_SAMPLES_H

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Constants.h"

//
// SH samples from R.Green's "SH lighting: the gritty details"
//

static inline double random01() { return (double)rand() / (double)RAND_MAX; }

// --------------------------------------------------------------------------

/**
 *  Uniformly distributed samples over sphere.
 */
class CSphericalSamples {
public:
	CSphericalSamples( int sqrtOfCount ) : mCount(sqrtOfCount*sqrtOfCount)
	{
		mSpherical = new SVector2[mCount];
		mXYZ = new SVector3[mCount];
		int i = 0;
		double oneOverN = 1.0 / sqrtOfCount;
		for( int a = 0; a < sqrtOfCount; ++a ) {
			for( int b = 0; b < sqrtOfCount; ++b ) {
				double x = (a + random01()) * oneOverN;
				double y = (b + random01()) * oneOverN;
				double theta = 2.0 * acos( sqrt( 1.0 - x ) );
				double phi = 2.0 * DINGUS_PID * y;
				mSpherical[i].set( theta, phi );
				mXYZ[i].set( sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta) );
				++i;
			}
		}
	}
	~CSphericalSamples()
	{
		delete[] mSpherical;
		delete[] mXYZ;
	}

	int		getCount() const { return mCount; }
	const SVector2& getSpherical( int i ) const { return mSpherical[i]; }
	const SVector3& getXYZ( int i ) const { return mXYZ[i]; }

private:
	int			mCount;
	SVector2*	mSpherical;	// spherical coords
	SVector3*	mXYZ;		// xyz coords
};


// --------------------------------------------------------------------------

class CSHSamples {
public:
	CSHSamples( int shOrder, const CSphericalSamples& samples )
		: mSamples(&samples), mSHOrder(shOrder), mSHOrder2(shOrder*shOrder)
	{
		mCoeffs = new float[ samples.getCount() * mSHOrder2 ];
		int n = getSampleCount();
		float *c = mCoeffs;
		float cmax = -1000;
		float cmin = 1000;
		for( int i = 0; i < n; ++i ) {
			const SVector3& xyz = mSamples->getXYZ(i);
			D3DXSHEvalDirection( c, shOrder, &xyz );
			for( int z = 0; z < mSHOrder2; ++z ) {
				if(c[z]>cmax) cmax = c[z];
				if(c[z]<cmin) cmin = c[z];
			}
			c += mSHOrder2;
		}
	}
	~CSHSamples()
	{
		delete[] mCoeffs;
	}

	int getSHOrder() const { return mSHOrder; }
	int getSHOrder2() const { return mSHOrder2; }

	int	getSampleCount() const { return mSamples->getCount(); }
	const SVector3& getSampleXYZ( int i ) const { return mSamples->getXYZ(i); }
	const float* getSampleCoeffs( int i ) const { return &mCoeffs[i*mSHOrder2]; }
	const float* getAllCoeffs() const { return mCoeffs; }
	float* getAllCoeffs() { return mCoeffs; }

private:
	const CSphericalSamples*	mSamples;
	int			mSHOrder;	// SH order. Generates SHOrder^2 coeffs
	int			mSHOrder2;	// SHOrder^2
	float*		mCoeffs;	// SH coeffs, SHOrder^2 per sample
};


#endif
