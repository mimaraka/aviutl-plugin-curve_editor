//----------------------------------------------------------------------------------
//		curve editor
//		source file (functions)
//		(Visual C++ 2019)
//----------------------------------------------------------------------------------

#include "ce_header.hpp"


//---------------------------------------------------------------------
//		Give the pointer of the FILTER structure
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void)
{
	return &g_filter;
}


//---------------------------------------------------------------------
//		Initialize
//---------------------------------------------------------------------
BOOL Init(FILTER* fp)
{
	g_fp = fp;
	//Load Ini (Set Config)
	g_cfg.theme = fp->exfunc->ini_load_int(fp, "theme", 0);
	g_cfg.trace = fp->exfunc->ini_load_int(fp, "show_hst", 1);
	g_cfg.alert = fp->exfunc->ini_load_int(fp, "show_alerts", 1);
	g_cfg.auto_copy = fp->exfunc->ini_load_int(fp, "auto_copy", 0);
	g_cfg.id_current = 0;
	g_cv_vl.ctpt[0].x = fp->exfunc->ini_load_int(fp, "x1", 400);
	g_cv_vl.ctpt[0].y = fp->exfunc->ini_load_int(fp, "y1", 400);
	g_cv_vl.ctpt[1].x = fp->exfunc->ini_load_int(fp, "x2", 600);
	g_cv_vl.ctpt[1].y = fp->exfunc->ini_load_int(fp, "y2", 600);
	g_cfg.sepr = fp->exfunc->ini_load_int(fp, "sepr", 200);
	g_cfg.mode = fp->exfunc->ini_load_int(fp, "mode", 0);
	g_cfg.align_mode = fp->exfunc->ini_load_int(fp, "align_mode", 1);

	g_th_custom = {0};

	//Direct2D
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2d1_factory);
	D2D1_RENDER_TARGET_PROPERTIES prop;
	prop = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(
			DXGI_FORMAT_B8G8R8A8_UNORM,
			D2D1_ALPHA_MODE_IGNORE),
		0, 0,
		D2D1_RENDER_TARGET_USAGE_NONE,
		D2D1_FEATURE_LEVEL_DEFAULT
	);
	g_d2d1_factory->CreateDCRenderTarget(&prop, &g_render_target);

	return TRUE;
}


//---------------------------------------------------------------------
//		Exit
//---------------------------------------------------------------------
BOOL Exit(FILTER* fp)
{
	fp->exfunc->ini_save_int(fp, "x1", g_cv_vl.ctpt[0].x);
	fp->exfunc->ini_save_int(fp, "y1", g_cv_vl.ctpt[0].y);
	fp->exfunc->ini_save_int(fp, "x2", g_cv_vl.ctpt[1].x);
	fp->exfunc->ini_save_int(fp, "y2", g_cv_vl.ctpt[1].y);
	fp->exfunc->ini_save_int(fp, "sepr", g_cfg.sepr);
	fp->exfunc->ini_save_int(fp, "mode", g_cfg.mode);
	fp->exfunc->ini_save_int(fp, "align_mode", g_cfg.align_mode);
	if (NULL != g_render_target) {
		g_render_target->Release();
	}
	if (NULL != g_d2d1_factory) {
		g_d2d1_factory->Release();
	}
	return TRUE;
}


//---------------------------------------------------------------------
//		create child window
//---------------------------------------------------------------------
HWND createChild(HWND hwnd, WNDPROC wndProc, LPSTR name, LONG style, int x, int y, int width, int height)
{
	HWND hCld;
	WNDCLASSEX tmp;
	tmp.cbSize		= sizeof(tmp);
	tmp.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	tmp.lpfnWndProc	= wndProc;
	tmp.cbClsExtra	= 0;
	tmp.cbWndExtra	= 0;
	tmp.hInstance		= g_fp->dll_hinst;
	tmp.hIcon			= NULL; tmp.hCursor = LoadCursor(NULL, IDC_ARROW);
	tmp.hbrBackground = NULL;
	tmp.lpszMenuName	= NULL;
	tmp.lpszClassName = name;
	tmp.hIconSm		= NULL;

	if (RegisterClassEx(&tmp)) {
		hCld = CreateWindowEx(
			NULL,
			name,
			NULL,
			WS_CHILD | WS_VISIBLE | style,
			x, y,
			width, height,
			hwnd,
			NULL,
			g_fp->dll_hinst,
			NULL
		);
		return hCld;
	}
	return 0;
}


//---------------------------------------------------------------------
//		get 1D curve ID and change CT	CE_Points -> change CT
//---------------------------------------------------------------------
void readValue(int value)
{
	UINT usint;
	if (value < 0) usint = value + 2147483647;
	else usint = (UINT)value + (UINT)2147483647;
	g_cv_vl.ctpt[1].y = usint / 6600047;
	g_cv_vl.ctpt[1].x = (usint - g_cv_vl.ctpt[1].y * 6600047) / 65347;
	g_cv_vl.ctpt[0].y = (usint - (g_cv_vl.ctpt[1].y * 6600047 + g_cv_vl.ctpt[1].x * 65347)) / 101;
	g_cv_vl.ctpt[0].x = (usint - (g_cv_vl.ctpt[1].y * 6600047 + g_cv_vl.ctpt[1].x * 65347)) % 101;
	g_cv_vl.ctpt[0].x *= CE_GR_RES / 100;
	g_cv_vl.ctpt[0].y *= CE_GR_RES / 100;
	g_cv_vl.ctpt[1].x *= CE_GR_RES / 100;
	g_cv_vl.ctpt[1].y *= CE_GR_RES / 100;
	g_cv_vl.ctpt[0].y -= 273;
	g_cv_vl.ctpt[0].y -= 273;
}


//---------------------------------------------------------------------
//		split
//---------------------------------------------------------------------
std::vector<std::string> split(const std::string& s, TCHAR c)
{
	std::vector<std::string> elems;
	std::string item;
	for (TCHAR ch : s) {
		if (ch == c) {
			if (!item.empty()) {
				elems.emplace_back(item);
				item.clear();
			}
		}
		else item += ch;
	}
	if (!item.empty())
		elems.emplace_back(item);
	return elems;
}


//---------------------------------------------------------------------
//		Ctpt -> 4D-Value (String)
//---------------------------------------------------------------------
std::string CtptToStr4DValues()
{
	FLOAT ptx, pty;
	std::string strx, stry, strResult;
	for (int i = 0; i < 2; i++) {
		ptx = std::round(g_cv_vl.ctpt[i].x * 100 / (double)CE_GR_RES) * 0.01;
		pty = std::round(g_cv_vl.ctpt[i].y * 100 / (double)CE_GR_RES) * 0.01;
		strx = std::to_string(ptx);
		stry = std::to_string(pty);
		strx.erase(4);
		if (g_cv_vl.ctpt[i].y < 0) stry.erase(5);
		else stry.erase(4);
		strResult += strx + ", " + stry + ", ";
	}
	strResult.erase(strResult.size() - 2, 2);
	return strResult;
}


//---------------------------------------------------------------------
//		Ctpt -> 1D-Value (Number)
//---------------------------------------------------------------------
int CtptToInt1DValue()
{
	int intResult;
	int x1, y1, x2, y2;
	x1 = (int)std::round(g_cv_vl.ctpt[0].x * 100 / (double)CE_GR_RES);
	y1 = (int)std::round(g_cv_vl.ctpt[0].y * 100 / (double)CE_GR_RES);
	x2 = (int)std::round(g_cv_vl.ctpt[1].x * 100 / (double)CE_GR_RES);
	y2 = (int)std::round(g_cv_vl.ctpt[1].y * 100 / (double)CE_GR_RES);
	//Limit Values
	if (y1 < -273 || 373 < y1 || y2 < -273 || 373 < y2) {
		return CE_OUTOFRANGE;
	}
	//Calculate
	intResult = 6600047 * (y2 + 273) + 65347 * x2 + 101 * (y1 + 273) + x1 - 2147483647;
	return intResult;
}


//---------------------------------------------------------------------
//		Copy Texts to Clipboard
//---------------------------------------------------------------------
BOOL copyToClipboard(HWND hwnd, LPCTSTR lpsText)
{
	HGLOBAL hMem;
	LPTSTR lpsBuffer;
	if (!OpenClipboard(hwnd))
		return FALSE;
	EmptyClipboard();
	hMem = GlobalAlloc(GHND | GMEM_SHARE, strlen(lpsText) + 1);
	lpsBuffer = (PTSTR)GlobalLock(hMem);
	lstrcpy(lpsBuffer, lpsText);
	GlobalUnlock(hMem);
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();
	return TRUE;
}