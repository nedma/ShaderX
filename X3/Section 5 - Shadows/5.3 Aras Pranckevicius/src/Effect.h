#ifndef __EFFECT_H
#define __EFFECT_H

// --------------------------------------------------------------------------

// We have API changes in D3DX effects from Summer 2004...
#undef USE_D3DX_SUMMER_2004
#if D3DX_VERSION >= 0x0902 && D3DX_SDK_VERSION >= 22
	#define USE_D3DX_SUMMER_2004
#endif


#ifdef USE_D3DX_SUMMER_2004

inline void fxBeginPass( ID3DXEffect* fx, int pass ) { fx->BeginPass(pass); }
inline void fxEndPass( ID3DXEffect* fx ) { fx->EndPass(); }
inline void fxCommit( ID3DXEffect* fx ) { fx->CommitChanges(); }

#else

inline void fxBeginPass( ID3DXEffect* fx, int pass ) { fx->Pass(pass); }
inline void fxEndPass( ID3DXEffect* fx ) { }
inline void fxCommit( ID3DXEffect* fx ) { }

#endif


#endif
