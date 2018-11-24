#ifndef __SIMPLE_MESH_H
#define __SIMPLE_MESH_H

#include "BaseEntity.h"
#include "Resources.h"


class CSimpleMesh : public CBaseEntity {
public:
	typedef std::vector<CRenderableMesh*>	TMeshVector;

public:
	CSimpleMesh( const CResourceId& meshID );

	void render( eRenderMode mode );
	void addMeshPart( int attrID, const CResourceId& texID );

public:
	CResourceId	mMeshID;
	TMeshVector mParts[MAX_MODES];
};


#endif
