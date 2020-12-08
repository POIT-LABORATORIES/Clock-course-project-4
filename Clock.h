#pragma once

#include "resource.h"

#include <wrl.h>
#include <algorithm>
#include <d2d1_1.h>
#include <d3d11.h>
#include <wincodec.h>

#if WINAPI_FAMILY_DESKTOP_APP == WINAPI_FAMILY
#include <atlbase.h>
#include <atlwin.h>
#endif