//----------------------------------------------------------------------------------
//		Curve Editor
//		ヘッダファイル (フレームワーク)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include <windows.h>
#include <windowsx.h>
#include <wininet.h>
#include <commctrl.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wincodecsdk.h>
#include <lua.hpp>
#include <fstream>
#include <numbers>
#include <regex>
#include <filter.h>
#include <aulslib/exedit.h>
#include <aulslib/aviutl.h>
#include <yulib/memory.h>
#include <auluilib/aului.hpp>
#include "cve_macro.hpp"
#include "resource.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "wininet.lib")