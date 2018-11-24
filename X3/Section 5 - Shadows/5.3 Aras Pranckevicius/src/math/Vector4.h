#ifndef __VECTOR_4_H
#define __VECTOR_4_H

#include "Vector3.h"



// --------------------------------------------------------------------------

/**
 *  4D vector.
 */
struct SVector4 : public D3DXVECTOR4 {
public:
	SVector4();
	SVector4( const float* f );
	SVector4( const D3DXFLOAT16* f );
	SVector4( float x, float y, float z, float w );
	SVector4( const D3DXVECTOR4& v );
	SVector4( const SVector3& v ); // w=1

	void		set( float vx, float vy, float vz, float vw );
};


inline SVector4::SVector4() : D3DXVECTOR4() { };
inline SVector4::SVector4( const float *f ) : D3DXVECTOR4(f) { };
inline SVector4::SVector4( const D3DXFLOAT16 *f ) : D3DXVECTOR4(f) { };
inline SVector4::SVector4( float vx, float vy, float vz, float vw ) : D3DXVECTOR4(vx,vy,vz,vw) { };
inline SVector4::SVector4( const D3DXVECTOR4& v ) : D3DXVECTOR4(v) { };
inline SVector4::SVector4( const SVector3& v ) : D3DXVECTOR4(v.x,v.y,v.z,1) { };

inline void SVector4::set( float vx, float vy, float vz, float vw ) { x=vx; y=vy; z=vz; w=vw; };


#endif
