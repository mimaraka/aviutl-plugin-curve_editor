//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (コントロールのプロシージャ)
//		VC++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"


//---------------------------------------------------------------------
//		message router
//---------------------------------------------------------------------
LRESULT CALLBACK ce::Control::MessageRouter(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Control* app;
	if (msg == WM_CREATE) {
		app = (Control*)(((LPCREATESTRUCT)lparam)->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)app);

	}
	else {
		app = (Control*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}
	return app->wndProc(hwnd, msg, wparam, lparam);
}


//---------------------------------------------------------------------
//		wndproc (member)
//---------------------------------------------------------------------
LRESULT ce::Control::wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static HWND				hwnd_parent;
	HDC						hdc;
	static HDC				hdc_mem;
	static HBITMAP			bitmap;
	HBRUSH					brush, brush_old;
	ID2D1SolidColorBrush*	pBrush;
	HFONT					font;
	static RECT				rcBtn;
	static BOOL				hovered, clicked;
	static TRACKMOUSEEVENT	tme;
	RECT					rect_wnd;
	PAINTSTRUCT				ps;

	GetClientRect(hwnd, &rcBtn);

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
			CE_FONT_ICON
		);
		if (clicked)
			D2D1_Init(hdc, &rect_wnd, BRIGHTEN(color, CE_CT_BR_CLICKED));
		else if (hovered)
			D2D1_Init(hdc, &rect_wnd, BRIGHTEN(color, CE_CT_BR_HOVERED));
		else
			D2D1_Init(hdc, &rect_wnd, color);

		if (g_render_target != NULL && g_d2d1_factory != NULL) {
			g_render_target->BeginDraw();
			if (pBrush == NULL)
				g_render_target->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &pBrush);
			g_render_target->EndDraw();
		}

		brush_old = (HBRUSH)SelectObject(hdc_mem, brush);
		FillRect(hdc_mem, &rcBtn, brush);
		DeleteObject(brush);

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

		SelectObject(hdc_mem, brush_old);
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
		case CE_WM_REDRAW:
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;
		}
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}


////---------------------------------------------------------------------
////		Apply Button
////---------------------------------------------------------------------
//LRESULT CALLBACK CtrlProc_Apply(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
//{
//	HDC hdc;
//	static HWND hwnd_parent, hTool;
//	static HDC hdc_mem;
//	static HBITMAP bitmap;
//	HBRUSH brush, brush_old;
//	HFONT hfCopy;
//	static RECT rcBtn;
//	ce::Theme style[] = { g_theme_dark, g_theme_light, g_th_custom };
//	static BOOL hovered, clicked;
//	static TRACKMOUSEEVENT tme;
//	PAINTSTRUCT ps;
//	GetClientRect(hwnd, &rcBtn);
//
//	switch (msg) {
//	case WM_CREATE:
//		hdc = GetDC(hwnd);
//		hdc_mem = CreateCompatibleDC(hdc);
//		bitmap = CreateCompatibleBitmap(hdc, CE_MAX_W, CE_CT_APPLY_H);
//		SelectObject(hdc_mem, bitmap);
//		ReleaseDC(hwnd, hdc);
//
//		hwnd_parent = GetParent(hwnd);
//
//		tme.cbSize = sizeof(tme);
//		tme.dwFlags = TME_LEAVE;
//		tme.hwndTrack = hwnd;
//
//		InitCommonControls();
//
//		hTool = CreateWindowEx(
//			0,
//			"TOOLTIPS_CLASS",
//			NULL,
//			TTS_ALWAYSTIP,
//			CW_USEDEFAULT,
//			CW_USEDEFAULT,
//			CW_USEDEFAULT,
//			CW_USEDEFAULT,
//			hwnd,
//			NULL,
//			g_fp->dll_hinst,
//			NULL
//		);
//
//		return 0;
//
//	case WM_CLOSE:
//		DeleteDC(hdc_mem);
//		DeleteObject(bitmap);
//		return 0;
//
//	case WM_SIZE:
//		SendMessage(hTool, TTM_NEWTOOLRECT, 0, (LPARAM)&tiCopy);
//		return 0;
//
//	case WM_PAINT:
//		hfCopy = CreateFont(
//			24, 0,
//			0, 0,
//			FW_NORMAL,
//			FALSE, FALSE, FALSE,
//			ANSI_CHARSET,
//			OUT_DEFAULT_PRECIS,
//			CLIP_DEFAULT_PRECIS,
//			DEFAULT_QUALITY,
//			0,
//			CE_FONT
//		);
//		if (clicked)
//			brush = CreateSolidBrush(style[g_config.theme].copy_button_clicked);
//		else if (hovered)
//			brush = CreateSolidBrush(style[g_config.theme].copy_button_hovered);
//		else
//			brush = CreateSolidBrush(style[g_config.theme].copy_button);
//
//		brush_old = (HBRUSH)SelectObject(hdc_mem, brush);
//		FillRect(hdc_mem, &rcBtn, brush);
//		DeleteObject(brush);
//
//		//COPY Button (Text)
//		SelectObject(hdc_mem, hfCopy);
//		SetBkMode(hdc_mem, TRANSPARENT);
//		SetTextColor(hdc_mem, style[g_config.theme].copy_button_text);
//		DrawText(hdc_mem, g_config.mode ? "APPLY" : "COPY", g_config.mode ? 5 : 4, &rcBtn, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOCLIP);
//		DeleteObject(hfCopy);
//
//
//		//Transfer bitmap data from buffer to screen
//		hdc = BeginPaint(hwnd, &ps);
//		BitBlt(hdc, 0, 0, rcBtn.right, rcBtn.bottom, hdc_mem, 0, 0, SRCCOPY);
//		EndPaint(hwnd, &ps);
//		DeleteDC(hdc);
//
//		SelectObject(hdc_mem, brush_old);
//		return 0;
//
//	case WM_MOUSEMOVE:
//		SetCursor(LoadCursor(NULL, IDC_HAND));
//		hovered = TRUE;
//		InvalidateRect(hwnd, NULL, FALSE);
//		TrackMouseEvent(&tme);
//		return 0;
//
//	case WM_LBUTTONDOWN:
//		SetCursor(LoadCursor(NULL, IDC_HAND));
//		clicked = TRUE;
//		InvalidateRect(hwnd, NULL, FALSE);
//		TrackMouseEvent(&tme);
//		return 0;
//
//	case WM_LBUTTONUP:
//		SetCursor(LoadCursor(NULL, IDC_HAND));
//		clicked = FALSE;
//		InvalidateRect(hwnd, NULL, FALSE);
//		SendMessage(hwnd_parent, WM_COMMAND, CM_EDITOR_COPY, 0);
//		return 0;
//
//	case WM_MOUSELEAVE:
//		clicked = FALSE;
//		hovered = FALSE;
//		InvalidateRect(hwnd, NULL, FALSE);
//		return 0;
//
//	case WM_COMMAND:
//		switch (wparam) {
//		case CE_WM_REDRAW:
//			InvalidateRect(hwnd, NULL, FALSE);
//			return 0;
//		}
//		return 0;
//
//	default:
//		return DefWindowProc(hwnd, msg, wparam, lparam);
//	}
//}
//
//
////---------------------------------------------------------------------
////		Value Panel
////---------------------------------------------------------------------
//LRESULT CALLBACK CtrlProc_Value(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
//{
//	HDC hdc;
//	static HWND hwnd_parent, hTool;
//	static HDC hdc_mem;
//	static HBITMAP bitmap;
//	HBRUSH brush, brush_old;
//	HFONT hfValue;
//	static RECT rect_wnd;
//	ce::Theme style[] = { g_theme_dark, g_theme_light, g_th_custom };
//	static BOOL hovered, clicked;
//	static TRACKMOUSEEVENT tme;
//	LPCTSTR lpsResult;
//	std::string strBuffer;
//	PAINTSTRUCT ps;
//	GetClientRect(hwnd, &rect_wnd);
//
//	switch (msg) {
//	case WM_CREATE:
//		hdc = GetDC(hwnd);
//		hdc_mem = CreateCompatibleDC(hdc);
//		bitmap = CreateCompatibleBitmap(hdc, CE_MAX_W, CE_CT_EDT_H);
//		SelectObject(hdc_mem, bitmap);
//		ReleaseDC(hwnd, hdc);
//
//		hwnd_parent = GetParent(hwnd);
//
//		tme.cbSize = sizeof(tme);
//		tme.dwFlags = TME_LEAVE;
//		tme.hwndTrack = hwnd;
//
//		return 0;
//
//	case WM_CLOSE:
//		DeleteDC(hdc_mem);
//		DeleteObject(bitmap);
//		return 0;
//
//	case WM_PAINT:
//		strBuffer = Create4DValue();
//		lpsResult = strBuffer.c_str();
//		hfValue = CreateFont(
//			rect_wnd.right >= MINCTRLSIZE * 0.7143 ? 20 : rect_wnd.right * 0.112, 0,
//			0, 0,
//			FW_NORMAL,
//			FALSE, FALSE, FALSE,
//			ANSI_CHARSET,
//			OUT_DEFAULT_PRECIS,
//			CLIP_DEFAULT_PRECIS,
//			DEFAULT_QUALITY,
//			0,
//			CE_FONT
//		);
//
//		//Value Panel
//		if (clicked)
//			brush = CreateSolidBrush(style[g_config.theme].other_buttons_clicked);
//		else if (hovered)
//			brush = CreateSolidBrush(style[g_config.theme].other_buttons_hovered);
//		else if (g_config.theme == 1)
//			brush = CreateSolidBrush(RGB(22, 22, 22));
//		else
//			brush = CreateSolidBrush(style[g_config.theme].other_buttons);
//		brush_old = (HBRUSH)SelectObject(hdc_mem, brush);
//		FillRect(hdc_mem, &rect_wnd, brush);
//		DeleteObject(brush);
//
//
//		//Value Panel (Text)
//		SelectObject(hdc_mem, hfValue);
//		SetBkMode(hdc_mem, TRANSPARENT);
//		if (clicked && g_config.theme == 1)
//			SetTextColor(hdc_mem, style[g_config.theme].switch_selected_contents);
//		else if (g_config.theme == 1)
//			SetTextColor(hdc_mem, RGB(255, 255, 255));
//		else
//			SetTextColor(hdc_mem, style[g_config.theme].other_buttons_contents);
//		DrawText(hdc_mem, lpsResult, strlen(lpsResult), &rect_wnd, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOCLIP);
//		DeleteObject(hfValue);
//
//
//		//Transfer bitmap data from buffer to screen
//		hdc = BeginPaint(hwnd, &ps);
//		BitBlt(hdc, 0, 0, rect_wnd.right, rect_wnd.bottom, hdc_mem, 0, 0, SRCCOPY);
//		EndPaint(hwnd, &ps);
//		DeleteDC(hdc);
//
//		SelectObject(hdc_mem, brush_old);
//		return 0;
//
//	case WM_MOUSEMOVE:
//		SetCursor(LoadCursor(NULL, IDC_HAND));
//		hovered = TRUE;
//		InvalidateRect(hwnd, NULL, FALSE);
//		TrackMouseEvent(&tme);
//		return 0;
//
//	case WM_LBUTTONDOWN:
//		SetCursor(LoadCursor(NULL, IDC_HAND));
//		clicked = TRUE;
//		InvalidateRect(hwnd, NULL, FALSE);
//		TrackMouseEvent(&tme);
//		return 0;
//
//	case WM_LBUTTONUP:
//		SetCursor(LoadCursor(NULL, IDC_HAND));
//		clicked = FALSE;
//		InvalidateRect(hwnd, NULL, FALSE);
//		SendMessage(hwnd_parent, WM_COMMAND, CM_EDITOR_VALUE, 0);
//		return 0;
//
//	case WM_MOUSELEAVE:
//		clicked = FALSE;
//		hovered = FALSE;
//		InvalidateRect(hwnd, NULL, FALSE);
//		return 0;
//
//	case WM_COMMAND:
//		switch (wparam) {
//		case CE_WM_REDRAW:
//			InvalidateRect(hwnd, NULL, FALSE);
//			return 0;
//		}
//		return 0;
//
//	default:
//		return DefWindowProc(hwnd, msg, wparam, lparam);
//	}
//}
//
//
////---------------------------------------------------------------------
////		Mode(Value) Switch
////---------------------------------------------------------------------
//LRESULT CALLBACK CtrlProc_Mode_Value(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
//{
//	HDC hdc;
//	static HWND hwnd_parent, hTool;
//	static HDC hdc_mem;
//	static HBITMAP bitmap;
//	HBRUSH brush, brush_old;
//	HFONT font;
//	static RECT rect_wnd;
//	ce::Theme style[] = { g_theme_dark, g_theme_light, g_th_custom };
//	static BOOL hovered, clicked;
//	static TRACKMOUSEEVENT tme;
//	PAINTSTRUCT ps;
//	GetClientRect(hwnd, &rect_wnd);
//
//	switch (msg) {
//	case WM_CREATE:
//		hdc = GetDC(hwnd);
//		hdc_mem = CreateCompatibleDC(hdc);
//		bitmap = CreateCompatibleBitmap(hdc, CE_MAX_W, CE_CT_SIDE_H);
//		SelectObject(hdc_mem, bitmap);
//		ReleaseDC(hwnd, hdc);
//
//		hwnd_parent = GetParent(hwnd);
//
//		tme.cbSize = sizeof(tme);
//		tme.dwFlags = TME_LEAVE;
//		tme.hwndTrack = hwnd;
//
//		return 0;
//
//	case WM_CLOSE:
//		DeleteDC(hdc_mem);
//		DeleteObject(bitmap);
//		return 0;
//
//	case WM_PAINT:
//		font = CreateFont(
//			rect_wnd.right >= MINCTRLSIZE * 0.25 ? 22 : rect_wnd.right * 0.32, 0,
//			0, 0,
//			FW_NORMAL,
//			FALSE, FALSE, FALSE,
//			ANSI_CHARSET,
//			OUT_DEFAULT_PRECIS,
//			CLIP_DEFAULT_PRECIS,
//			DEFAULT_QUALITY,
//			0,
//			CE_FONT_ICON
//		);
//
//		//Mode(value) Button
//		if (clicked || !g_config.mode)
//			brush = CreateSolidBrush(style[g_config.theme].switch_selected);
//		else if (hovered)
//			brush = CreateSolidBrush(style[g_config.theme].other_buttons_hovered);
//		else
//			brush = CreateSolidBrush(style[g_config.theme].other_buttons);
//		brush_old = (HBRUSH)SelectObject(hdc_mem, brush);
//		FillRect(hdc_mem, &rect_wnd, brush);
//		DeleteObject(brush);
//
//		SelectObject(hdc_mem, font);
//		SetBkMode(hdc_mem, TRANSPARENT);
//		if (clicked || !g_config.mode) SetTextColor(hdc_mem, style[g_config.theme].switch_selected_contents);
//		else SetTextColor(hdc_mem, style[g_config.theme].other_buttons_contents);
//		DrawText(hdc_mem, "#", 1, &rect_wnd, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOCLIP);
//		DeleteObject(font);
//
//
//		//Transfer bitmap data from buffer to screen
//		hdc = BeginPaint(hwnd, &ps);
//		BitBlt(hdc, 0, 0, rect_wnd.right, rect_wnd.bottom, hdc_mem, 0, 0, SRCCOPY);
//		EndPaint(hwnd, &ps);
//		DeleteDC(hdc);
//
//		SelectObject(hdc_mem, brush_old);
//		return 0;
//
//	case WM_MOUSEMOVE:
//		SetCursor(LoadCursor(NULL, IDC_HAND));
//		hovered = TRUE;
//		InvalidateRect(hwnd, NULL, FALSE);
//		TrackMouseEvent(&tme);
//		return 0;
//
//	case WM_LBUTTONDOWN:
//		SetCursor(LoadCursor(NULL, IDC_HAND));
//		clicked = TRUE;
//		InvalidateRect(hwnd, NULL, FALSE);
//		TrackMouseEvent(&tme);
//		return 0;
//
//	case WM_LBUTTONUP:
//		SetCursor(LoadCursor(NULL, IDC_HAND));
//		clicked = FALSE;
//		g_config.mode = 0;
//		SendMessage(hwnd_parent, WM_COMMAND, CE_WM_REDRAW, 0);
//		InvalidateRect(hwnd, NULL, FALSE);
//		return 0;
//
//	case WM_MOUSELEAVE:
//		clicked = FALSE;
//		hovered = FALSE;
//		InvalidateRect(hwnd, NULL, FALSE);
//		return 0;
//
//	case WM_COMMAND:
//		switch (wparam) {
//		case CE_WM_REDRAW:
//			InvalidateRect(hwnd, NULL, FALSE);
//			return 0;
//		}
//		return 0;
//
//	default:
//		return DefWindowProc(hwnd, msg, wparam, lparam);
//	}
//}
//
//
////---------------------------------------------------------------------
////		Mode(ID) Switch
////---------------------------------------------------------------------
//LRESULT CALLBACK CtrlProc_Mode_ID(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
//{
//	HDC hdc;
//	static HWND hwnd_parent, hTool;
//	static HDC hdc_mem;
//	static HBITMAP bitmap;
//	HBRUSH brush, brush_old;
//	HFONT font;
//	static RECT rect_wnd;
//	ce::Theme style[] = { g_theme_dark, g_theme_light, g_th_custom };
//	static BOOL hovered, clicked;
//	static TRACKMOUSEEVENT tme;
//	PAINTSTRUCT ps;
//	GetClientRect(hwnd, &rect_wnd);
//
//	switch (msg) {
//	case WM_CREATE:
//		hdc = GetDC(hwnd);
//		hdc_mem = CreateCompatibleDC(hdc);
//		bitmap = CreateCompatibleBitmap(hdc, CE_MAX_W, CE_CT_SIDE_H);
//		SelectObject(hdc_mem, bitmap);
//		ReleaseDC(hwnd, hdc);
//
//		hwnd_parent = GetParent(hwnd);
//
//		tme.cbSize = sizeof(tme);
//		tme.dwFlags = TME_LEAVE;
//		tme.hwndTrack = hwnd;
//
//		return 0;
//
//	case WM_CLOSE:
//		DeleteDC(hdc_mem);
//		DeleteObject(bitmap);
//		return 0;
//
//	case WM_PAINT:
//		font = CreateFont(
//			rect_wnd.right >= MINCTRLSIZE * 0.25 ? 22 : rect_wnd.right * 0.32, 0,
//			0, 0,
//			FW_NORMAL,
//			FALSE, FALSE, FALSE,
//			ANSI_CHARSET,
//			OUT_DEFAULT_PRECIS,
//			CLIP_DEFAULT_PRECIS,
//			DEFAULT_QUALITY,
//			0,
//			CE_FONT_BOLD
//		);
//
//		//Mode(ID) Button
//		if (clicked || g_config.mode)
//			brush = CreateSolidBrush(style[g_config.theme].switch_selected);
//		else if (hovered)
//			brush = CreateSolidBrush(style[g_config.theme].other_buttons_hovered);
//		else
//			brush = CreateSolidBrush(style[g_config.theme].other_buttons);
//		brush_old = (HBRUSH)SelectObject(hdc_mem, brush);
//		FillRect(hdc_mem, &rect_wnd, brush);
//		DeleteObject(brush);
//
//		SelectObject(hdc_mem, font);
//		SetBkMode(hdc_mem, TRANSPARENT);
//		if (clicked || g_config.mode) SetTextColor(hdc_mem, style[g_config.theme].switch_selected_contents);
//		else SetTextColor(hdc_mem, style[g_config.theme].other_buttons_contents);
//		DrawText(hdc_mem, "ID", 2, &rect_wnd, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOCLIP);
//		DeleteObject(font);
//
//
//		//Transfer bitmap data from buffer to screen
//		hdc = BeginPaint(hwnd, &ps);
//		BitBlt(hdc, 0, 0, rect_wnd.right, rect_wnd.bottom, hdc_mem, 0, 0, SRCCOPY);
//		EndPaint(hwnd, &ps);
//		DeleteDC(hdc);
//
//		SelectObject(hdc_mem, brush_old);
//		return 0;
//
//	case WM_MOUSEMOVE:
//		SetCursor(LoadCursor(NULL, IDC_HAND));
//		hovered = TRUE;
//		InvalidateRect(hwnd, NULL, FALSE);
//		TrackMouseEvent(&tme);
//		return 0;
//
//	case WM_LBUTTONDOWN:
//		SetCursor(LoadCursor(NULL, IDC_HAND));
//		clicked = TRUE;
//		InvalidateRect(hwnd, NULL, FALSE);
//		TrackMouseEvent(&tme);
//		return 0;
//
//	case WM_LBUTTONUP:
//		SetCursor(LoadCursor(NULL, IDC_HAND));
//		clicked = FALSE;
//		g_config.mode = 1;
//		SendMessage(hwnd_parent, WM_COMMAND, CE_WM_REDRAW, 0);
//		InvalidateRect(hwnd, NULL, FALSE);
//		return 0;
//
//	case WM_MOUSELEAVE:
//		clicked = FALSE;
//		hovered = FALSE;
//		InvalidateRect(hwnd, NULL, FALSE);
//		return 0;
//
//	case WM_COMMAND:
//		switch (wparam) {
//		case CE_WM_REDRAW:
//			InvalidateRect(hwnd, NULL, FALSE);
//			return 0;
//		}
//		return 0;
//
//	default:
//		return DefWindowProc(hwnd, msg, wparam, lparam);
//	}
//}
//
//
////---------------------------------------------------------------------
////		ID(ID) Button
////---------------------------------------------------------------------
//LRESULT CALLBACK CtrlProc_ID_ID(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
//{
//	HDC hdc;
//	static HWND hwnd_parent, hTool;
//	static HDC hdc_mem;
//	static HBITMAP bitmap;
//	static POINT ptLock;
//	static int intBuffer;
//	POINT cl_pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
//	HBRUSH brush, brush_old;
//	HFONT font;
//	std::string strNum;
//	LPCTSTR lpcsNum;
//	static RECT rect_wnd;
//	ce::Theme style[] = { g_theme_dark, g_theme_light, g_th_custom };
//	static BOOL hovered, clicked;
//	static TRACKMOUSEEVENT tme;
//	PAINTSTRUCT ps;
//	GetClientRect(hwnd, &rect_wnd);
//
//	switch (msg) {
//	case WM_CREATE:
//		hdc = GetDC(hwnd);
//		hdc_mem = CreateCompatibleDC(hdc);
//		bitmap = CreateCompatibleBitmap(hdc, CE_MAX_W, CE_CT_EDT_H);
//		SelectObject(hdc_mem, bitmap);
//		ReleaseDC(hwnd, hdc);
//
//		hwnd_parent = GetParent(hwnd);
//
//		tme.cbSize = sizeof(tme);
//		tme.dwFlags = TME_LEAVE;
//		tme.hwndTrack = hwnd;
//
//		return 0;
//
//	case WM_CLOSE:
//		DeleteDC(hdc_mem);
//		DeleteObject(bitmap);
//		return 0;
//
//	case WM_PAINT:
//		strNum = std::to_string(g_config.id_current);
//		lpcsNum = strNum.c_str();
//
//		font = CreateFont(
//			rect_wnd.right >= MINCTRLSIZE * 0.167 ? 22 : rect_wnd.right * 0.48, 0,
//			0, 0,
//			FW_NORMAL,
//			FALSE, FALSE, FALSE,
//			ANSI_CHARSET,
//			OUT_DEFAULT_PRECIS,
//			CLIP_DEFAULT_PRECIS,
//			DEFAULT_QUALITY,
//			0,
//			FL_FONT_BOLD
//		);
//
//		//ID(ID) Button
//		if (clicked) brush = CreateSolidBrush(style[g_config.theme].other_buttons_clicked);
//		else brush = CreateSolidBrush(style[g_config.theme].other_buttons_hovered);
//		brush_old = (HBRUSH)SelectObject(hdc_mem, brush);
//		FillRect(hdc_mem, &rect_wnd, brush);
//		DeleteObject(brush);
//
//		SelectObject(hdc_mem, font);
//		SetBkMode(hdc_mem, TRANSPARENT);
//		if (clicked && g_config.theme == 1)
//			SetTextColor(hdc_mem, style[g_config.theme].switch_selected_contents);
//		else
//			SetTextColor(hdc_mem, style[g_config.theme].other_buttons_contents);
//		DrawText(hdc_mem, lpcsNum, strlen(lpcsNum), &rect_wnd, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOCLIP | DT_NOPREFIX);
//		DeleteObject(font);
//
//
//		//Transfer bitmap data from buffer to screen
//		hdc = BeginPaint(hwnd, &ps);
//		BitBlt(hdc, 0, 0, rect_wnd.right, rect_wnd.bottom, hdc_mem, 0, 0, SRCCOPY);
//		EndPaint(hwnd, &ps);
//		DeleteDC(hdc);
//
//		SelectObject(hdc_mem, brush_old);
//		return 0;
//
//	case WM_MOUSEMOVE:
//		if (clicked) {
//			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
//			g_config.id_current = intBuffer + (cl_pt.x - ptLock.x) / 9;
//			g_config.id_current = MINMAXLIM(g_config.id_current, 0, CE_CURVE_MAX - 1);
//			SendMessage(hwnd_parent, WM_COMMAND, CE_WM_REDRAW, 0);
//		}
//		else SetCursor(LoadCursor(NULL, IDC_HAND));
//
//		hovered = TRUE;
//		InvalidateRect(hwnd, NULL, FALSE);
//		TrackMouseEvent(&tme);
//		return 0;
//
//	case WM_LBUTTONDOWN:
//		ptLock = cl_pt;
//		intBuffer = g_config.id_current;
//		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
//		clicked = TRUE;
//		InvalidateRect(hwnd, NULL, FALSE);
//		SetCapture(hwnd);
//		return 0;
//
//	case WM_LBUTTONUP:
//		clicked = FALSE;
//		ReleaseCapture();
//		InvalidateRect(hwnd, NULL, FALSE);
//		return 0;
//
//	case WM_MOUSELEAVE:
//		clicked = FALSE;
//		hovered = FALSE;
//		InvalidateRect(hwnd, NULL, FALSE);
//		return 0;
//
//	case WM_COMMAND:
//		switch (wparam) {
//		case CE_WM_REDRAW:
//			InvalidateRect(hwnd, NULL, FALSE);
//			return 0;
//		}
//		return 0;
//
//	default:
//		return DefWindowProc(hwnd, msg, wparam, lparam);
//	}
//}