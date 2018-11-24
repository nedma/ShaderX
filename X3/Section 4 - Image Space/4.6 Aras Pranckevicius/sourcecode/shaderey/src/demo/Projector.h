#ifndef __PROJECTOR_H
#define __PROJECTOR_H

#include <dingus/math/Matrix4x4.h>

/**
 *  Texture projector.
 */
class CProjector {
public:
	static void computeTextureProjection(
		const SMatrix4x4& projectorMatrix,
		SMatrix4x4& dest );
};


#endif
