#ifndef __CLOUD_MESH_H
#define __CLOUD_MESH_H

#include "BaseEntity.h"
#include "Resources.h"


class CCloudMesh : public CBaseEntity {
public:
	CCloudMesh( const CResourceId& meshID );

	void render( eRenderMode mode );

public:
	CRenderableMesh* mMesh[MAX_MODES-1]; // base, color, normal/z
};


#endif
