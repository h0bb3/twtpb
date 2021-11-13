// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

//#define WINVER 0x0501
//#define _WIN32_WINNT 0x0501
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <winsock.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: reference additional headers your program requires here
#pragma comment(lib, "../engineIII/common/externals/devil/libs/DevIL.lib")
#pragma comment(lib, "../engineIII/common/externals/devil/libs/ILU.lib")

#include <Common/StringDef.h>
#include <Common/Log.h>
#include <DXRenderer/DXDefines.h>
#include <math/vector.h>
#include <math/Ray.h>
#include <math/Geometry.h>
#include <math/Intersect.h>
#include <assert.h>
