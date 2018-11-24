#ifndef __COLOR_OPS_H
#define __COLOR_OPS_H

#include <dingus/math/Vector3.h>

/**
 *  Convert RGB to HSV.
 *  @param rgb RGB color (0..1 range).
 *  @return HSV color (all 0..1 range, including hue).
 */
inline SVector3 gRGB2HSV( SVector3 rgb )
{
	float h, s, v;
	float mn = rgb.x,mx=rgb.x;
	int maxVal=0; 
	if (rgb.y > mx){ mx=rgb.y;maxVal=1;}
	if (rgb.z > mx){ mx=rgb.z;maxVal=2;}
	if (rgb.y < mn) mn=rgb.y; 
	if (rgb.z < mn) mn=rgb.z;  
	
	float  delta = mx - mn; 
	
	v = mx;
	if( mx != 0 )
		s = delta / mx;  
	else {
		s = 0; 
		h = 0; 
		return SVector3( h, s, v );
	}
	if( s==0.0f ) {
		h = 0;
		return SVector3( h, s, v );
	} else {  
		switch( maxVal ) { 
		case 0: h = ( rgb.y - rgb.z ) / delta;break;		  // yel < h < mag 
		case 1: h = 2 + ( rgb.z - rgb.x ) / delta;break;	  // cyan < h < yel 
		case 2: h = 4 + ( rgb.x - rgb.y ) / delta;break;	  // mag < h < cyan 
		} 
	} 
	
	// NOTE: we use 0..1 range, not 0..360
	h *= 1.0f/6.0f; // was: 60
	if( h < 0 ) h += 1.0f; // was: 360

	return SVector3(h,s,v);
};

/**
 *  Convert HSV to RGB.
 *  @param hsv HSV color (all 0..1 range, including hue).
 *  @return RGB color (0..1 range).
 */
inline SVector3 gHSV2RGB( SVector3 hsv )
{
	int i;
	float h = hsv.x, s = hsv.y, v = hsv.z;
	float f, p, q, t, hTemp; 
	float r, g, b;
	
	if( s == 0.0 ) { // s==0? Totally unsaturated = grey so R,G and B all equal value
		r = g = b = v; 
		return SVector3(r,g,b);
	} 
	// NOTE: we use 0..1 range, not 0..360
	hTemp = h*6.0f; // was: /60.0f
	i = (int)floor( hTemp );				 // which sector 
	f = hTemp - i;						// how far through sector 
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	
	switch( i ) { 
	case 0:{r = v;g = t;b = p;break;} 
	case 1:{r = q;g = v;b = p;break;} 
	case 2:{r = p;g = v;b = t;break;} 
	case 3:{r = p;g = q;b = v;break;}  
	case 4:{r = t;g = p;b = v;break;} 
	case 5:{r = v;g = p;b = q;break;} 
	}
	return SVector3(r,g,b);
};

static void WINAPI gVolumeFillerRGB2HSV( D3DXVECTOR4* out, const D3DXVECTOR3* uvw, const D3DXVECTOR3* texelSize, void* data )
{
	SVector3 c = gRGB2HSV( *uvw );
	c.x -= fmodf( c.x, 0.05f ) - 0.025f;
	c.y -= fmodf( c.y, 0.19f ) - 0.095f;
	c.z -= fmodf( c.z, 0.08f ) - 0.040f;
	*out = D3DXVECTOR4( c.x, c.y, c.z, 1.0f );
}

static void WINAPI gVolumeFillerHSV2RGB( D3DXVECTOR4* out, const D3DXVECTOR3* uvw, const D3DXVECTOR3* texelSize, void* data )
{
	SVector3 c = gHSV2RGB( *uvw );
	*out = D3DXVECTOR4( c.x, c.y, c.z, 1.0f );
}

#endif
