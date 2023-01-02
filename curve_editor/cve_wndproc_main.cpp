//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル(ウィンドウプロシージャ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		ウィンドウプロシージャ（メイン）
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_main(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static bool is_separator_moving = false;
	POINT pt_client = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
	RECT	rect_wnd;
	cve::Rectangle rect_sepr, rect_header, rect_editor, rect_library;
	static cve::Bitmap_Buffer bitmap_buffer;
	const bool is_vertical = g_config.layout_mode == cve::Config::Vertical;


	::GetClientRect(hwnd, &rect_wnd);


	rect_sepr.set(
		is_vertical ? 0 : rect_wnd.right - g_config.separator - CVE_SEPARATOR_WIDTH,
		is_vertical ? rect_wnd.bottom - g_config.separator - CVE_SEPARATOR_WIDTH : 0,
		is_vertical ? rect_wnd.right : rect_wnd.right - g_config.separator + CVE_SEPARATOR_WIDTH,
		is_vertical ? rect_wnd.bottom - g_config.separator + CVE_SEPARATOR_WIDTH : rect_wnd.bottom
	);

	rect_header.set(
		0,
		0,
		is_vertical ? rect_wnd.right : rect_wnd.right - g_config.separator - CVE_SEPARATOR_WIDTH,
		CVE_MENU_H
	);

	rect_editor.set(
		0,
		CVE_MENU_H,
		is_vertical ? rect_wnd.right : rect_wnd.right - g_config.separator - CVE_SEPARATOR_WIDTH,
		is_vertical ? rect_wnd.bottom - g_config.separator - CVE_SEPARATOR_WIDTH : rect_wnd.bottom
	);
	rect_editor.set_margin(CVE_MARGIN, 0, CVE_MARGIN, CVE_MARGIN);

	rect_library.set(
		is_vertical ? 0 : rect_wnd.right - g_config.separator + CVE_SEPARATOR_WIDTH,
		is_vertical ? rect_wnd.bottom - g_config.separator + CVE_SEPARATOR_WIDTH : 0,
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

		// エディタパネル
		g_window_editor.create(
			hwnd,
			"WINDOW_EDITOR",
			wndproc_editor,
			NULL,
			CS_DBLCLKS,
			rect_editor.rect,
			NULL
		);

		// ヘッダパネル
		g_window_menu.create(
			hwnd,
			"WINDOW_HEADER",
			wndproc_menu,
			NULL,
			NULL,
			rect_header.rect,
			NULL
		);

		// プリセットパネル
		g_window_library.create(
			hwnd,
			"WINDOW_PRESET",
			wndproc_library,
			NULL,
			NULL,
			rect_library.rect, NULL
		);

		return 0;

	case WM_PAINT:
		bitmap_buffer.draw_panel_main(rect_sepr.rect);
		return 0;

	case WM_SIZE:
		bitmap_buffer.set_size(rect_wnd);
		g_window_menu.move(rect_header.rect);
		g_window_editor.move(rect_editor.rect);
		g_window_library.move(rect_library.rect);
		return 0;

	case WM_LBUTTONDOWN:
		if (::PtInRect(&rect_sepr.rect, pt_client)) {
			is_separator_moving = true;
			if (g_config.layout_mode == cve::Config::Vertical)
				::SetCursor(LoadCursor(NULL, IDC_SIZENS));
			else
				::SetCursor(LoadCursor(NULL, IDC_SIZEWE));
			::SetCapture(hwnd);
		}
		return 0;

	case WM_LBUTTONUP:
		is_separator_moving = false;
		::ReleaseCapture();
		return 0;

	case WM_MOUSEMOVE:
		if (::PtInRect(&rect_sepr.rect, pt_client)) {
			if (g_config.layout_mode == cve::Config::Vertical)
				::SetCursor(LoadCursor(NULL, IDC_SIZENS));
			else
				::SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		}
		if (is_separator_moving) {
			if (g_config.layout_mode == cve::Config::Vertical)
				g_config.separator = MINMAX_LIMIT(rect_wnd.bottom - pt_client.y, CVE_SEPARATOR_WIDTH, rect_wnd.bottom - CVE_SEPARATOR_WIDTH - CVE_MENU_H);

			else
				g_config.separator = MINMAX_LIMIT(rect_wnd.right - pt_client.x, CVE_SEPARATOR_WIDTH, rect_wnd.right - CVE_SEPARATOR_WIDTH);

			::SetActiveWindow(g_window_library.hwnd);
			g_window_menu.move(rect_header.rect);
			g_window_editor.move(rect_editor.rect);
			g_window_library.move(rect_library.rect);
			::InvalidateRect(hwnd, NULL, FALSE);
		}
		return 0;

	case WM_COMMAND:
		switch (wparam) {
		case CVE_CM_REDRAW:
			::InvalidateRect(hwnd, NULL, FALSE);
			g_window_menu.move(rect_header.rect);
			g_window_editor.move(rect_editor.rect);
			g_window_library.move(rect_library.rect);
			g_window_menu.redraw();
			g_window_editor.redraw();
			g_window_library.redraw();
		}

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
}