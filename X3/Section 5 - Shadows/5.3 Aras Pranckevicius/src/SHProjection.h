#ifndef __SH_PROJECTION_H
#define __SH_PROJECTION_H

#include "SphericalSamples.h"
#include "math/Constants.h"

//
// SH projection from R.Green's "SH lighting: the gritty details"
//

namespace sh {


template< typename F >
void projectXYZFunction( F& f, CSHSamples& samples, double res[] )
{
	int i;
	const int nsmp = samples.getSHOrder2();
	for( i = 0; i < nsmp; ++i )
		res[i] = 0.0;

	const double weight = 4.0 * DINGUS_PID;
	int n = samples.getSampleCount();
	float *coeffs = samples.getAllCoeffs();
	for( i = 0; i < n; ++i ) {
		const SVector3& v = samples.getSampleXYZ(i);
		double fval = f.eval( v );
		for( int j = 0; j < nsmp; ++j ) {
			res[j] += fval * coeffs[0];
			++coeffs;
		}
	}

	double factor = weight / n;
	for( i = 0; i < nsmp; ++i )
		res[i] = res[i] * factor;
};


template< typename F >
void projectXYZFunction( F& f, const SVector3& hemiDir, CSHSamples& samples, double res[] )
{
	int i;
	const int nsmp = samples.getSHOrder2();
	for( i = 0; i < nsmp; ++i )
		res[i] = 0.0;

	const double weight = 4.0 * DINGUS_PID;
	int n = samples.getSampleCount();
	float *coeffs = samples.getAllCoeffs();
	for( i = 0; i < n; ++i, coeffs += nsmp ) {
		const SVector3& v = samples.getSampleXYZ(i);
		if( v.dot(hemiDir) < 0.0f ) // discard other hemisphere
			continue;
		double fval = f.eval( v );
		for( int j = 0; j < nsmp; ++j ) {
			res[j] += fval * coeffs[j];
		}
	}

	double factor = weight / n;
	for( i = 0; i < nsmp; ++i )
		res[i] = res[i] * factor;
};


}; // namespace

#endif
