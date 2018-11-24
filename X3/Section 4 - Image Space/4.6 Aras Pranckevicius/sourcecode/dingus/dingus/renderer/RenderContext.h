// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// --------------------------------------------------------------------------

#ifndef __RENDER_CONTEXT_H
#define __RENDER_CONTEXT_H


#include "RenderCamera.h"
#include "../utils/Notifier.h"
#include "EffectParams.h"

namespace dingus {

class CRenderContext;
class CRenderable;


/**
 *  Gets notified when scene rendering occurs.
 */
class IRenderContextListener {
public:
	virtual ~IRenderContextListener() = 0 {};
	
	virtual void beforeRendering( CRenderContext& ctx ) = 0;
	virtual void afterRendering( CRenderContext& ctx ) = 0;
};



class CRenderContext : public CNotifier<IRenderContextListener> {
public:
	CRenderContext( CD3DXEffect& globalFx );

	void attach( CRenderable& r ) { mRenderables.push_back(&r); };
	void clear() { mRenderables.clear(); };

	void perform();

	const CRenderCamera& getCamera() const { return mRenderCamera; };
	CRenderCamera& getCamera() { return mRenderCamera; };
	
	CEffectParams const& getGlobalParams() const { return mGlobalParams; };
	CEffectParams& getGlobalParams() { return mGlobalParams; };

	//
	// "direct" rendering

	void directBegin();
	void directRender( CRenderable& r );
	void directEnd();
	int directSetFX( CD3DXEffect& fx ); // returns pass count-1
	void directFXPassBegin( int pass );
	void directFXPassEnd();
	
private:
	void	sortRenderables();
	void	renderRenderables();

	/// Notifies IRenderContextListeners
	void beforeRendering();
	/// Notifies IRenderContextListeners
	void afterRendering();

private:
	typedef std::vector<CRenderable*>	TRenderableVector;
private:
	TRenderableVector	mRenderables;
	CRenderCamera		mRenderCamera;

	CEffectParams		mGlobalParams;
	CD3DXEffect*		mGlobalEffect;

	// direct rendering part
	bool			mInsideDirect;
	CD3DXEffect*	mDirectCurrFX;
	int				mDirectCurrPasses;
};


}; // namespace

#endif
