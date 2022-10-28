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
#include <lua.hpp>
#include <fstream>
#include <regex>
#include <filter.h>
#include <aulslib/exedit.h>
#include <aulslib/aviutl.h>
#include <yulib/memory.h>
#include "cve_macro.hpp"
#include "resource.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "comctl32.lib")