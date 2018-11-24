// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// --------------------------------------------------------------------------

#ifndef __MATH_UTILS_H
#define __MATH_UTILS_H


namespace dingus {


/**
 *  Clamps value to the given range.
 */
inline float clamp( float value, float vmin = 0.0f, float vmax = 1.0f ) {
	assert( vmin <= vmax );
	if( value < vmin )
		value = vmin;
	else if( value > vmax )
		value = vmax;
	return value;
};


/**
 *  Returns a number between 0 ant 1. Returned number is zero when value
 *  equals vZero, and lerps to one at (1/scale) from vZero.
 *
 *  Eg.:
 *		fuzzySwitch( v, 10, -0.2 ) would be 1 when v=5 and 0 when v=10.
 *		fuzzySwitch( v, 10,  0.1 ) would be 1 when v=20 and 0 when v=10.
 */
inline float fuzzySwitch( float value, float vZero, float scale ) {
	return clamp( (value-vZero)*scale, 0.0f, 1.0f );
};


}; // namespace

#endif
