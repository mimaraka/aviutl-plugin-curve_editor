//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル(func_ctrl)
//		(Visual C++ 2022)
//----------------------------------------------------------------------------------

#include "ce_header.hpp"


//---------------------------------------------------------------------
//		コントロールを作成
//---------------------------------------------------------------------
BOOL ce::Control::create(HWND hwnd_p, WNDPROC exWndProc, LPSTR name, LPCTSTR icon, int ct_id, int x, int y, int w, int h, int fl)
{
	WNDCLASSEX tmp;
	id					= ct_id;
	icon				= icon;
	flag				= fl;
	tmp.cbSize			= sizeof(tmp);
	tmp.style			= CS_HREDRAW | CS_VREDRAW;
	if (exWndProc != NULL)
		tmp.lpfnWndProc	= exWndProc;
	else
		tmp.lpfnWndProc	= message_router;
	tmp.cbClsExtra		= 0;
	tmp.cbWndExtra		= 0;
	tmp.hInstance		= g_fp->dll_hinst;
	tmp.hIcon			= NULL;
	tmp.hCursor			= LoadCursor(NULL, IDC_ARROW);
	tmp.hbrBackground	= NULL;
	tmp.lpszMenuName	= NULL;
	tmp.lpszClassName	= name;
	tmp.hIconSm			= NULL;

	if (RegisterClassEx(&tmp)) {
		hwnd = CreateWindowEx(
			NULL,
			name,
			NULL,
			WS_CHILD | WS_VISIBLE,
			x, y,
			w, h,
			hwnd_p,
			NULL,
			g_fp->dll_hinst,
			NULL
		);
		return 1;
	}
	else return 0;
}


//---------------------------------------------------------------------
//		コントロールを移動
//---------------------------------------------------------------------
void ce::Control::move(int x, int y, int w, int h)
{
	MoveWindow(hwnd, x, y, w, h, TRUE);
}


//---------------------------------------------------------------------
//		仮ウィンドウプロシージャ
//---------------------------------------------------------------------
LRESULT CALLBACK ce::Control::message_router(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Control* app;
	if (msg == WM_CREATE) {
		app = (Control*)(((LPCREATESTRUCT)lparam)->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)app);

	}
	else {
		app = (Control*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}
	return app->wndproc(hwnd, msg, wparam, lparam);
}


//---------------------------------------------------------------------
//		ウィンドウプロシージャ（メンバ）
//---------------------------------------------------------------------
LRESULT ce::Control::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static HWND hwnd_parent;
	HDC hdc;
	static HDC hdc_mem;
	static HBITMAP bitmap;
	static ID2D1SolidColorBrush* pBrush = NULL;
	HFONT font;
	static RECT rcBtn;
	static BOOL hovered, clicked;
	static TRACKMOUSEEVENT tme;
	RECT rect_wnd;
	PAINTSTRUCT ps;
	GetClientRect(hwnd, &rcBtn);

	switch (msg) {
	case WM_CREATE:
		hdc = GetDC(hwnd);
		hdc_mem = CreateCompatibleDC(hdc);
		bitmap = CreateCompatibleBitmap(hdc, CE_MAX_W, CE_CT_APPLY_H);
		SelectObject(hdc_mem, bitmap);
		ReleaseDC(hwnd, hdc);

		hwnd_parent = GetParent(hwnd);

		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hwnd;

		return 0;

	case WM_CLOSE:
		DeleteDC(hdc_mem);
		DeleteObject(bitmap);
		return 0;

	case WM_PAINT:
		font = CreateFont(
			CE_CT_ICON_SIZE, 0,
			0, 0,
			FW_NORMAL,
			FALSE, FALSE, FALSE,
			ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			0,
			NULL
		);
		if (this != nullptr) {
			if (clicked)
				d2d_setup(hdc_mem, &rect_wnd, BRIGHTEN(color, CE_CT_BR_CLICKED));
			else if (hovered)
				d2d_setup(hdc_mem, &rect_wnd, BRIGHTEN(color, CE_CT_BR_HOVERED));
			else
				d2d_setup(hdc_mem, &rect_wnd, color);


			if (g_render_target != NULL && g_d2d1_factory != NULL) {
				g_render_target->BeginDraw();
				if (pBrush == NULL) g_render_target->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &pBrush);
				g_render_target->EndDraw();
			}

			//icon
			SelectObject(hdc_mem, font);
			SetBkMode(hdc_mem, TRANSPARENT);
			SetTextColor(hdc_mem, g_theme[g_config.theme].bt_tx);
			DrawText(hdc_mem, icon, 1, &rcBtn, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOCLIP);
			DeleteObject(font);


			//Transfer bitmap data from buffer to screen
			hdc = BeginPaint(hwnd, &ps);
			BitBlt(hdc, 0, 0, rcBtn.right, rcBtn.bottom, hdc_mem, 0, 0, SRCCOPY);
			EndPaint(hwnd, &ps);
			DeleteDC(hdc);
		}

		return 0;

	case WM_MOUSEMOVE:
		SetCursor(LoadCursor(NULL, IDC_HAND));
		hovered = TRUE;
		InvalidateRect(hwnd, NULL, FALSE);
		TrackMouseEvent(&tme);
		return 0;

	case WM_LBUTTONDOWN:
		SetCursor(LoadCursor(NULL, IDC_HAND));
		clicked = TRUE;
		InvalidateRect(hwnd, NULL, FALSE);
		TrackMouseEvent(&tme);
		return 0;

	case WM_LBUTTONUP:
		SetCursor(LoadCursor(NULL, IDC_HAND));
		clicked = FALSE;
		InvalidateRect(hwnd, NULL, FALSE);
		SendMessage(hwnd_parent, WM_COMMAND, id, 0);
		return 0;

	case WM_MOUSELEAVE:
		clicked = FALSE;
		hovered = FALSE;
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;

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


//---------------------------------------------------------------------
//		バグってるのでテスト用ウィンドウプロシージャ(Apply)
//---------------------------------------------------------------------
LRESULT CALLBACK WndProc_Test(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static	HWND					hwnd_parent;
			HDC						hdc;
	static	HDC						hdc_mem;
	static	HBITMAP					bitmap;
	static	ID2D1SolidColorBrush*	pBrush	= NULL;
			HFONT					font;
	static	BOOL					hovered, clicked;
	static	TRACKMOUSEEVENT			tme;
			RECT					rect_wnd;
			PAINTSTRUCT				ps;
			COLORREF				color	= g_theme[g_config.theme].bt_apply;
			int						flag	= NULL;
			LPCSTR					icon	= CE_CT_APPLY_TEXT;
			int						id		= CE_CT_APPLY;
	
	GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	case WM_CREATE:
		hdc		= GetDC(hwnd);
		hdc_mem = CreateCompatibleDC(hdc);
		bitmap	= CreateCompatibleBitmap(hdc, CE_MAX_W, CE_CT_APPLY_H);
		SelectObject(hdc_mem, bitmap);
		ReleaseDC(hwnd, hdc);

		hwnd_parent = GetParent(hwnd);

		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hwnd;

		return 0;

		///////////////////////////////////////////
		//		WM_CLOSE
		//		(ウィンドウが閉じられたとき)
		///////////////////////////////////////////
	case WM_CLOSE:
		DeleteDC(hdc_mem);
		DeleteObject(bitmap);
		return 0;

		///////////////////////////////////////////
		//		WM_PAINT
		//		(ウィンドウが描画されたとき)
		///////////////////////////////////////////
	case WM_PAINT:
		font = CreateFont(
			24, 0,
			0, 0,
			FW_NORMAL,
			FALSE, FALSE, FALSE,
			ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			0,
			NULL
		);
	
		if (clicked)
			d2d_setup(hdc_mem, &rect_wnd, TO_BGR(BRIGHTEN(color, CE_CT_BR_CLICKED)));
		else if (hovered)
			d2d_setup(hdc_mem, &rect_wnd, TO_BGR(BRIGHTEN(color, CE_CT_BR_HOVERED)));
		else
			d2d_setup(hdc_mem, &rect_wnd, TO_BGR(color));

		//icon
		SelectObject(hdc_mem, font);
		SetBkMode(hdc_mem, TRANSPARENT);
		SetTextColor(hdc_mem, g_theme[g_config.theme].bt_tx);
		DrawText(hdc_mem, icon, strlen(icon), &rect_wnd, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOCLIP);
		DeleteObject(font);


		if (g_render_target != NULL && g_d2d1_factory != NULL) {
			g_render_target->BeginDraw();
			if (pBrush == NULL) g_render_target->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &pBrush);
			g_render_target->EndDraw();
		}

		//Transfer bitmap data from buffer to screen
		hdc = BeginPaint(hwnd, &ps);
		BitBlt(hdc, 0, 0, rect_wnd.right, rect_wnd.bottom, hdc_mem, 0, 0, SRCCOPY);
		EndPaint(hwnd, &ps);
		DeleteDC(hdc);
		

		return 0;

	case WM_MOUSEMOVE:
		SetCursor(LoadCursor(NULL, IDC_HAND));
		hovered = TRUE;
		InvalidateRect(hwnd, NULL, FALSE);
		TrackMouseEvent(&tme);
		return 0;

	case WM_LBUTTONDOWN:
		SetCursor(LoadCursor(NULL, IDC_HAND));
		clicked = TRUE;
		InvalidateRect(hwnd, NULL, FALSE);
		TrackMouseEvent(&tme);
		return 0;

	case WM_LBUTTONUP:
		SetCursor(LoadCursor(NULL, IDC_HAND));
		clicked = FALSE;
		InvalidateRect(hwnd, NULL, FALSE);
		SendMessage(hwnd_parent, WM_COMMAND, id, 0);
		return 0;

	case WM_MOUSELEAVE:
		clicked = FALSE;
		hovered = FALSE;
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;

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