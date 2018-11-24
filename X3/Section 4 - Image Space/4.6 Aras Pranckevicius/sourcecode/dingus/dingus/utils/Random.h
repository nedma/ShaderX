// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// --------------------------------------------------------------------------

#ifndef __RANDOM_H__
#define __RANDOM_H__

namespace dingus {

/**
 *  Returns random number [0..interv)
 */
inline float randf( float interv = 1.0 )
{
	return (float)rand() / RAND_MAX * interv;
};

/**
 *  Returns random number [from..to)
 */
inline float randf( float from, float to )
{
	return randf( to - from ) + from;
};


}; // namespace

#endif 
