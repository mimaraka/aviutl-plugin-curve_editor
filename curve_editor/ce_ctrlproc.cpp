//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (コントロールのプロシージャ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		Value Panel
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_control_value(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	HDC hdc;
	static HWND hwnd_parent, hTool;
	static HDC hdc_mem;
	static HBITMAP bitmap;
	HBRUSH brush, brush_old;
	HFONT hfValue;
	static RECT rect_wnd;
	ce::Theme style[] = { g_theme_dark, g_theme_light, g_th_custom };
	static BOOL hovered, clicked;
	static TRACKMOUSEEVENT tme;
	LPCTSTR lpsResult;
	std::string strBuffer;
	PAINTSTRUCT ps;
	GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	case WM_CREATE:
		hdc = GetDC(hwnd);
		hdc_mem = CreateCompatibleDC(hdc);
		bitmap = CreateCompatibleBitmap(hdc, CE_MAX_W, CE_CT_EDT_H);
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
		strBuffer = create_curve_value_4d();
		lpsResult = strBuffer.c_str();
		hfValue = CreateFont(
			rect_wnd.right >= MINCTRLSIZE * 0.7143 ? 20 : rect_wnd.right * 0.112, 0,
			0, 0,
			FW_NORMAL,
			FALSE, FALSE, FALSE,
			ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			0,
			CE_FONT
		);

		//Value Panel
		if (clicked)
			brush = CreateSolidBrush(style[g_config.theme].other_buttons_clicked);
		else if (hovered)
			brush = CreateSolidBrush(style[g_config.theme].other_buttons_hovered);
		else if (g_config.theme == 1)
			brush = CreateSolidBrush(RGB(22, 22, 22));
		else
			brush = CreateSolidBrush(style[g_config.theme].other_buttons);
		brush_old = (HBRUSH)SelectObject(hdc_mem, brush);
		FillRect(hdc_mem, &rect_wnd, brush);
		DeleteObject(brush);


		//Value Panel (Text)
		SelectObject(hdc_mem, hfValue);
		SetBkMode(hdc_mem, TRANSPARENT);
		if (clicked && g_config.theme == 1)
			SetTextColor(hdc_mem, style[g_config.theme].switch_selected_contents);
		else if (g_config.theme == 1)
			SetTextColor(hdc_mem, RGB(255, 255, 255));
		else
			SetTextColor(hdc_mem, style[g_config.theme].other_buttons_contents);
		DrawText(hdc_mem, lpsResult, strlen(lpsResult), &rect_wnd, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOCLIP);
		DeleteObject(hfValue);


		//Transfer bitmap data from buffer to screen
		hdc = BeginPaint(hwnd, &ps);
		BitBlt(hdc, 0, 0, rect_wnd.right, rect_wnd.bottom, hdc_mem, 0, 0, SRCCOPY);
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
		SendMessage(hwnd_parent, WM_COMMAND, CM_EDITOR_VALUE, 0);
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