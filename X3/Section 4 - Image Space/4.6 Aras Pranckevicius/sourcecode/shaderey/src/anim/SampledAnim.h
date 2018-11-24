#ifndef __SAMPLED_ANIM_H
#define __SAMPLED_ANIM_H

#include <dingus/math/TypeTraits.h>


/**
 *  A sampled animation.
 */
template<typename _V>
class CSampledAnim : public boost::noncopyable {
public:
	enum eLoopType { CLAMP = 0, REPEAT };
	typedef _V value_type;

public:
	CSampledAnim( eLoopType loopType = REPEAT ) : mLoopType(loopType) { };

	void	addSample( const value_type& sample ) { mSamples.push_back(sample); }
	void	reserveSamples( int sampleCount ) { mSamples.reserve(sampleCount); }
	int		getSampleCount() const { return mSamples.size(); }
	const value_type& getSample( int index ) const;
	void	clearSamples() { mSamples.clear(); }

	void		setLoopType( eLoopType loopType ) { mLoopType = loopType; }
	eLoopType	getLoopType() const { return mLoopType; }

	/**
	 *  @param time Relative time (zero is start, one is end).
	 */
	void	timeToIndex( float time, int& index1, int& index2, float& alpha ) const;
	/**
	 *  @param time Relative time (zero is start, one is end).
	 */
	void	sample( int idx1, int idx2, float alpha, value_type& dest ) const;

private:
	typedef std::vector<value_type>	TSampleVector;

private:
	TSampleVector	mSamples;
	eLoopType		mLoopType;
};

typedef CSampledAnim<SVector3>		TVector3Anim;
typedef CSampledAnim<SQuaternion>	TQuatAnim;
typedef CSampledAnim<float>			TFloatAnim;


// --------------------------------------------------------------------------

template<typename _V>
inline void CSampledAnim<_V>::timeToIndex( float time, int& index1, int& index2, float& alpha ) const
{
	float frame = time * getSampleCount();
	index1 = int(frame);
	index2 = index1 + 1;
	alpha = frame - float(index1);
	
	if( mLoopType == CLAMP ) {
		// clamp
		if( index1 < 0 ) index1 = 0;
		else if( index1 >= getSampleCount() ) index1 = getSampleCount()-1;
		if( index2 < 0 ) index2 = 0;
		else if( index2 >= getSampleCount() ) index2 = getSampleCount()-1;
	} else {
		// repeat
		index1 %= getSampleCount();
		index2 %= getSampleCount();
	}
};

template<typename _V>
inline const _V& CSampledAnim<_V>::getSample( int index ) const
{
	assert( index>=0 && index < getSampleCount() );
	return mSamples[index];
};

template<typename _V>
void CSampledAnim<_V>::sample( int idx1, int idx2, float alpha, _V& dest ) const
{
	assert( &dest );

	const value_type& s1 = getSample( idx1 );
	const value_type& s2 = getSample( idx2 );
	dest = math_type_traits<value_type>::interpolate( s1, s2, alpha );
};


#endif
