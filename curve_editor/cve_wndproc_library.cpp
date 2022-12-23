//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル(ウィンドウプロシージャ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		ウィンドウプロシージャ（ライブラリ）
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_library(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static HWND search;
	HFONT font;
	POINT pt_client = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
	RECT rect_wnd;
	cve::Rectangle rect_preset_list;
	static cve::Bitmap_Buffer bitmap_buffer;

	::GetClientRect(hwnd, &rect_wnd);

	rect_preset_list.set(
		0,
		CVE_LIBRARY_SEARCHBAR_HEIGHT + CVE_MARGIN * 2,
		rect_wnd.right,
		rect_wnd.bottom
	);


	switch (msg) {
	case WM_CLOSE:
		bitmap_buffer.exit();
		return 0;

	case WM_CREATE:
		bitmap_buffer.init(hwnd);
		bitmap_buffer.set_size(rect_wnd);

		// プリセットのリスト
		g_window_preset_list.create(
			hwnd,
			"WINDOW_PRESET_LIST",
			wndproc_preset_list,
			NULL,
			CS_DBLCLKS,
			rect_preset_list.rect,
			NULL
		);

#ifdef _DEBUG

		// 検索バー
		search = ::CreateWindowEx(
			NULL,
			"EDIT",
			NULL,
			WS_CHILD | WS_VISIBLE,
			CVE_MARGIN, CVE_MARGIN,
			rect_wnd.right - CVE_MARGIN * 2,
			20,
			hwnd,
			NULL,
			g_fp->dll_hinst,
			NULL
		);

		font = ::CreateFont(
			CVE_CT_FONT_H, 0,
			0, 0,
			FW_REGULAR,
			FALSE, FALSE, FALSE,
			SHIFTJIS_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			NULL,
			CVE_FONT_REGULAR
		);

		::SendMessage(search, WM_SETFONT, (WPARAM)font, MAKELPARAM(TRUE, 0));

#endif

		return 0;

	case WM_SIZE:
		bitmap_buffer.set_size(rect_wnd);
		g_window_preset_list.move(rect_preset_list.rect);
#ifdef _DEBUG
		::MoveWindow(search, CVE_MARGIN, CVE_MARGIN, rect_wnd.right - CVE_MARGIN * 2, 20, TRUE);
#endif
		return 0;

	case WM_PAINT:
		bitmap_buffer.draw_panel();
		return 0;

	case WM_COMMAND:
		switch (wparam) {
		case CVE_CM_REDRAW:
			::InvalidateRect(hwnd, NULL, FALSE);
			g_window_preset_list.redraw();
			return 0;
		}
		return 0;

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ（プリセットリスト）
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_preset_list(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;
	static cve::Bitmap_Buffer bitmap_buffer;

	::GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	case WM_CLOSE:
		bitmap_buffer.exit();
		return 0;

	case WM_CREATE:
		bitmap_buffer.init(hwnd);
		bitmap_buffer.set_size(rect_wnd);

		return 0;

	case WM_SIZE:
		bitmap_buffer.set_size(rect_wnd);
		::PostMessage(hwnd, WM_COMMAND, CVE_CM_PRESET_MOVE, 0);

		return 0;

	case WM_PAINT:
		bitmap_buffer.draw_panel();
		return 0;

	case WM_MOUSEWHEEL:
		::ScrollWindowEx(
			hwnd,
			0,
			GET_Y_LPARAM(wparam) / 3,
			NULL,
			NULL,
			NULL,
			NULL,
			//MAKEWPARAM(SW_INVALIDATE | SW_SCROLLCHILDREN | SW_SMOOTHSCROLL, 100)
			SW_INVALIDATE | SW_SCROLLCHILDREN
		);
		return 0;

	case WM_COMMAND:
		switch (wparam) {
		case CVE_CM_REDRAW:
			::InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		case CVE_CM_PRESET_MOVE:
			switch (g_config.edit_mode) {
			case cve::Mode_Bezier:
				break;

			case cve::Mode_Multibezier:
				break;
			}
			// 後で消す
			for (int i = 0; i < (int)g_presets_normal_custom.size; i++) {
				g_presets_normal_custom[i].move(rect_wnd.right, i);
			}
			return 0;
		}
		return 0;

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
}