//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル(プリセット)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		プリセットを作成
//---------------------------------------------------------------------
BOOL ce::Preset::create(HWND hwnd_parent)
{
	WNDCLASSEX tmp;
	tmp.cbSize = sizeof(tmp);
	tmp.style = CS_HREDRAW | CS_VREDRAW;
	tmp.lpfnWndProc = wndproc_static;
	tmp.cbClsExtra = 0;
	tmp.cbWndExtra = 0;
	tmp.hInstance = g_fp->dll_hinst;
	tmp.hIcon = NULL;
	tmp.hIconSm = NULL;
	tmp.hCursor = LoadCursor(NULL, IDC_ARROW);
	tmp.hbrBackground = NULL;
	tmp.lpszMenuName = NULL;
	tmp.lpszClassName = name;

	if (RegisterClassEx(&tmp)) {
		hwnd = ::CreateWindowEx(
			NULL,
			name,
			NULL,
			WS_CHILD | WS_VISIBLE,
			0, 0,
			g_config.preset_size,
			g_config.preset_size,
			hwnd_parent,
			NULL,
			g_fp->dll_hinst,
			this
		);
		if (hwnd != nullptr)
			return 1;
	}
	return 0;
}



//---------------------------------------------------------------------
//		プリセットを移動
//---------------------------------------------------------------------
void ce::Preset::move(int width)
{
	const int w = 2 * CE_MARGIN_PRESET + g_config.preset_size;
	int x_count = index % (width / w);
	int y_count = index / (width / w);
	::MoveWindow(
		hwnd,
		w * x_count,
		w * y_count,
		g_config.preset_size,
		g_config.preset_size + CE_MARGIN_PRESET,
		TRUE
		);
}



//---------------------------------------------------------------------
//		静的ウィンドウプロシージャ
//---------------------------------------------------------------------
LRESULT CALLBACK ce::Preset::wndproc_static(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Preset* app = (Preset*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (!app) {//取得できなかった(ウィンドウ生成中)場合
		if (msg == WM_CREATE) {
			app = (Preset*)((LPCREATESTRUCT)lparam)->lpCreateParams;
			if (app) {
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)app);
				return app->wndproc(hwnd, msg, wparam, lparam);
			}
		}
	}
	else {//取得できた場合(ウィンドウ生成後)
		return app->wndproc(hwnd, msg, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ(static変数使用不可)
//---------------------------------------------------------------------
LRESULT ce::Preset::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{

	return ::DefWindowProc(hwnd, msg, wparam, lparam);
}