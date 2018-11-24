// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// --------------------------------------------------------------------------
#include "stdafx.h"

#include "SharedSurfaceBundle.h"
#include "../utils/Errors.h"
#include "../kernel/D3DDevice.h"

using namespace dingus;



void CSharedSurfaceBundle::registerSurface( CResourceId const& id, const ISurfaceCreator::TSharedPtr& creator )
{
	assert( creator );
	IDirect3DSurface9* tex = creator->createSurface();
	registerResource( id, *(new CD3DSurface(tex)), creator );
}

void CSharedSurfaceBundle::createResource()
{
}

void CSharedSurfaceBundle::activateResource()
{
	// recreate all objects
	TResourceMap::iterator it;
	for( it = mResourceMap.begin(); it != mResourceMap.end(); ++it ) {
		ISurfaceCreator::TSharedPtr& creator = it->second.first;
		assert( creator );
		CD3DSurface& res = *it->second.second;
		if( !res.isNull() )
			continue; // kind of HACK
		res.setObject( creator->createSurface() );
		assert( !res.isNull() );
	}
}

void CSharedSurfaceBundle::passivateResource()
{
	// unload all objects
	TResourceMap::iterator it;
	for( it = mResourceMap.begin(); it != mResourceMap.end(); ++it ) {
		CD3DSurface& res = *it->second.second;
		assert( !res.isNull() );
		res.getObject()->Release();
		res.setObject( NULL );
	}
}

void CSharedSurfaceBundle::deleteResource()
{
}
