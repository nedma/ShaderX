#ifndef __WATER_MESH_H
#define __WATER_MESH_H

#include "BaseEntity.h"
#include "Resources.h"


class CWaterMesh : public CBaseEntity {
public:
	CWaterMesh( const CResourceId& meshID );

	void render( eRenderMode mode );

public:
	CRenderableMesh* mMesh[MAX_MODES-1]; // base, color, normal/Z only
};


#endif
