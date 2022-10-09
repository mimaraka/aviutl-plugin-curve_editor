//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル(コントロール)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		コントロールを作成
//---------------------------------------------------------------------
BOOL ce::Control::create(HWND hwnd_p, LPSTR name, LPTSTR ico_res, int ct_id, LPRECT rect)
{
	WNDCLASSEX tmp;
	id = ct_id;
	icon_res = ico_res;

	tmp.cbSize			= sizeof(tmp);
	tmp.style			= CS_HREDRAW | CS_VREDRAW;
	tmp.lpfnWndProc		= wndproc_static;
	tmp.cbClsExtra		= 0;
	tmp.cbWndExtra		= 0;
	tmp.hInstance		= g_fp->dll_hinst;
	tmp.hIcon			= NULL;
	tmp.hIconSm			= NULL;
	tmp.hCursor			= LoadCursor(NULL, IDC_ARROW);
	tmp.hbrBackground	= NULL;
	tmp.lpszMenuName	= NULL;
	tmp.lpszClassName	= name;

	if (RegisterClassEx(&tmp)) {
		hwnd = CreateWindowExA(
			NULL,
			name,
			NULL,
			WS_CHILD | WS_VISIBLE,
			rect->left, rect->top,
			rect->right - rect->left,
			rect->bottom - rect->top,
			hwnd_p,
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
//		コントロールを移動
//---------------------------------------------------------------------
void ce::Control::move(LPRECT rect)
{
	MoveWindow(
		hwnd,
		rect->left, rect->top,
		rect->right - rect->left,
		rect->bottom - rect->top,
		TRUE
	);
}



//---------------------------------------------------------------------
//		静的ウィンドウプロシージャ
//---------------------------------------------------------------------
LRESULT CALLBACK ce::Control::wndproc_static(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Control* app = (Control*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (!app) {//取得できなかった(ウィンドウ生成中)場合
		if (msg == WM_CREATE) {
			app = (Control*)((LPCREATESTRUCT)lparam)->lpCreateParams;
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
//		ウィンドウプロシージャ(static変数使っちゃダメ！)
//---------------------------------------------------------------------
LRESULT ce::Control::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;

	GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	case WM_CREATE:
		canvas.init(hwnd);

		hwnd_parent = GetParent(hwnd);



		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hwnd;
		return 0;

	case WM_CLOSE:
		canvas.exit();
		return 0;

	// 描画
	case WM_PAINT:
	{
		COLORREF bg;
		ID2D1SolidColorBrush* pBrush = NULL;

		if (clicked)
			bg = BRIGHTEN(g_theme[g_config.theme].bg_others, CE_CT_BR_CLICKED);
		else if (hovered)
			bg = BRIGHTEN(g_theme[g_config.theme].bg_others, CE_CT_BR_HOVERED);
		else
			bg = g_theme[g_config.theme].bg_others;
		d2d_setup(&canvas, &rect_wnd, bg);

		if (g_render_target != NULL && g_d2d1_factory != NULL) {
			g_render_target->BeginDraw();
			if (pBrush == NULL)
				g_render_target->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &pBrush);
			g_render_target->EndDraw();
		}

		canvas.transfer(&rect_wnd);
		return 0;
	}

	// マウスが動いたとき
	case WM_MOUSEMOVE:
		SetCursor(LoadCursor(NULL, IDC_HAND));
		hovered = TRUE;
		InvalidateRect(hwnd, NULL, FALSE);
		TrackMouseEvent(&tme);
		return 0;

	// 左クリックがされたとき
	case WM_LBUTTONDOWN:
		SetCursor(LoadCursor(NULL, IDC_HAND));
		clicked = TRUE;
		InvalidateRect(hwnd, NULL, FALSE);
		TrackMouseEvent(&tme);
		return 0;

	// 左クリックが終わったとき
	case WM_LBUTTONUP:
		SetCursor(LoadCursor(NULL, IDC_HAND));
		clicked = FALSE;
		InvalidateRect(hwnd, NULL, FALSE);
		SendMessage(hwnd_parent, WM_COMMAND, id, 0);
		return 0;

	// マウスがウィンドウから離れたとき
	case WM_MOUSELEAVE:
		clicked = FALSE;
		hovered = FALSE;
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;

	// コマンド
	case WM_COMMAND:
		switch (wparam) {
		case CE_CM_REDRAW:
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;
		}
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}