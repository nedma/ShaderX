// --------------------------------------------------------------------------
// Dingus project - a collection of subsystems for game/graphics applications
// --------------------------------------------------------------------------

#ifndef __DINGUS_STDAFX_H
#define __DINGUS_STDAFX_H

// precompiled headers stuff

#pragma warning(disable:4786)
#pragma warning(disable:4018)

// windows
#ifdef _WINDLL
//#ifdef _AFXDLL 

#define VC_EXTRALEAN
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif

#else

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#endif

// STL
#include <list>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <string>

// C libs
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cstdarg>
#include <malloc.h>

// D3D
#include <d3d9.h>
#include <d3dx9.h>

// smart pointers
//#include <boost/shared_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
//#include <boost/weak_ptr.hpp>
#include <boost/noncopyable.hpp>

// common
#include "utils/MemUtil.h"
#include "utils/STLUtils.h"
#include "utils/fastvector.h"
#include "utils/AssertHelper.h"
#include "utils/RefCounted.h"


#endif
