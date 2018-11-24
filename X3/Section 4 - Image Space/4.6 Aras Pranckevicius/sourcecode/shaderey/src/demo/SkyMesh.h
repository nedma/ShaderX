#ifndef __SKY_MESH_H
#define __SKY_MESH_H

#include <dingus/renderer/RenderableMesh.h>


class CSkyMesh {
public:
	CSkyMesh();

	void render();

private:
	CRenderableMesh* mMesh; // base only
};


#endif
