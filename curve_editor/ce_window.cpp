//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (ウィンドウ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		ウィンドウ作成
//---------------------------------------------------------------------
BOOL ce::Window::create(HWND hwnd_parent, LPTSTR class_name, WNDPROC wndproc, LONG style, const RECT& rect)
{
	WNDCLASSEX tmp;
	tmp.cbSize = sizeof(tmp);
	tmp.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	tmp.lpfnWndProc = wndproc;
	tmp.cbClsExtra = 0;
	tmp.cbWndExtra = 0;
	tmp.hInstance = g_fp->dll_hinst;
	tmp.hIcon = NULL;
	tmp.hCursor = LoadCursor(NULL, IDC_ARROW);
	tmp.hbrBackground = NULL;
	tmp.lpszMenuName = NULL;
	tmp.lpszClassName = class_name;
	tmp.hIconSm = NULL;

	if (RegisterClassEx(&tmp)) {
		hwnd = CreateWindowEx(
			NULL,
			class_name,
			NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | style,
			rect.left,
			rect.top,
			rect.right - rect.left,
			rect.bottom - rect.top,
			hwnd_parent,
			NULL,
			g_fp->dll_hinst,
			NULL
		);
		return TRUE;
	}
	return 0;
}



//---------------------------------------------------------------------
//		ウィンドウ移動
//---------------------------------------------------------------------
void ce::Window::move(const RECT& rect) const
{
	::MoveWindow(
		hwnd,
		SMALLER(rect.left, rect.right),
		SMALLER(rect.top, rect.bottom),
		std::abs(rect.right - rect.left),
		std::abs(rect.bottom - rect.top),
		TRUE
	);
}



//---------------------------------------------------------------------
//		再描画
//---------------------------------------------------------------------
void ce::Window::redraw() const
{
	::SendMessage(hwnd, WM_COMMAND, CE_CM_REDRAW, 0);
}



//---------------------------------------------------------------------
//		表示
//---------------------------------------------------------------------
BOOL ce::Window::show() const
{
	return ::ShowWindow(hwnd, SW_SHOW);
}



//---------------------------------------------------------------------
//		非表示
//---------------------------------------------------------------------
BOOL ce::Window::hide() const
{
	return ::ShowWindow(hwnd, SW_HIDE);
}



//---------------------------------------------------------------------
//		ウィンドウを閉じる
//---------------------------------------------------------------------
BOOL ce::Window::close() const
{
	return ::DestroyWindow(hwnd);
}



//---------------------------------------------------------------------
//		静的ウィンドウプロシージャ
//---------------------------------------------------------------------
LRESULT CALLBACK ce::Window::wndproc_static(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Window* app = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (!app) {//取得できなかった(ウィンドウ生成中)場合
		if (msg == WM_CREATE) {
			app = (Window*)((LPCREATESTRUCT)lparam)->lpCreateParams;
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
//		ウィンドウプロシージャ
//---------------------------------------------------------------------
LRESULT ce::Window::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, msg, wparam, lparam);
}