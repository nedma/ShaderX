#if !defined(DXSDK_STDAFX_H)
#define DXSDK_STDAFX_H

// Windows
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <stdio.h>
#include <assert.h>
#include <tchar.h>
#include <commctrl.h>
#include <D3DX9.h>

// DX common
#include "common/DXUtil.h"
#include "common/D3DEnumeration.h"
#include "common/D3DSettings.h"
#include "common/D3DApp.h"
#include "common/D3DUtil.h"
#include "common/D3DRes.h"

// STL
#include <vector>
#include <list>
#include <map>
#include <set>
#include <string>
#include <algorithm>

// C
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cassert>

#endif
