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
	aului::Window_Rectangle rect_sepr, rect_menu, rect_editor, rect_library;
	static cve::My_Direct2d_Paint_Object bitmap_buffer;
	const bool is_vertical = g_config.layout_mode == cve::Config::Vertical;
	static bool is_sepr_invalid_ver = false;
	static bool is_sepr_invalid_hor = false;
	static int sepr_buffer_ver = 0;
	static int sepr_buffer_hor = 0;

	::GetClientRect(hwnd, &rect_wnd);

	// 縦レイアウトのとき
	if (is_vertical) {
		if (is_sepr_invalid_ver) {
			g_config.separator = sepr_buffer_ver;
		}
		if (g_config.separator > rect_wnd.bottom - CVE_MENU_H - CVE_SEPARATOR_WIDTH) {
			// セパレータの位置があかん状態になった瞬間
			if (!is_sepr_invalid_ver) {
				is_sepr_invalid_ver = true;
				sepr_buffer_ver = g_config.separator;
			}
			g_config.separator = rect_wnd.bottom - CVE_MENU_H - CVE_SEPARATOR_WIDTH;
		}
		else {
			if (is_sepr_invalid_ver) {
				is_sepr_invalid_ver = false;
			}
		}
		g_config.separator = std::max(g_config.separator, CVE_SEPARATOR_WIDTH);
	}
	// 横レイアウトのとき
	else {
		if (is_sepr_invalid_hor) {
			g_config.separator = sepr_buffer_hor;
		}
		if (g_config.separator > rect_wnd.right - CVE_MIN_W + CVE_SEPARATOR_WIDTH) {
			// セパレータの位置があかん状態になった瞬間
			if (!is_sepr_invalid_hor) {
				is_sepr_invalid_hor = true;
				sepr_buffer_hor = g_config.separator;
			}
			g_config.separator = rect_wnd.right - CVE_MIN_W + CVE_SEPARATOR_WIDTH;
		}
		else {
			if (is_sepr_invalid_hor) {
				is_sepr_invalid_hor = false;
			}
		}
		g_config.separator = std::max(g_config.separator, CVE_SEPARATOR_WIDTH);
	}

	rect_sepr.set(
		is_vertical ? 0 : rect_wnd.right - g_config.separator - CVE_SEPARATOR_WIDTH,
		is_vertical ? rect_wnd.bottom - g_config.separator - CVE_SEPARATOR_WIDTH : 0,
		is_vertical ? rect_wnd.right : rect_wnd.right - g_config.separator + CVE_SEPARATOR_WIDTH,
		is_vertical ? rect_wnd.bottom - g_config.separator + CVE_SEPARATOR_WIDTH : rect_wnd.bottom
	);

	rect_menu.set(
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
	case WM_CREATE:
		bitmap_buffer.init(hwnd, g_p_factory, g_p_write_factory);

		// エディタパネル
		g_window_editor.create(
			g_fp->dll_hinst,
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
			g_fp->dll_hinst,
			hwnd,
			"WINDOW_HEADER",
			wndproc_menu,
			NULL,
			NULL,
			rect_menu.rect,
			NULL
		);

		// プリセットパネル
		g_window_library.create(
			g_fp->dll_hinst,
			hwnd,
			"WINDOW_PRESET",
			wndproc_library,
			NULL,
			NULL,
			rect_library.rect, NULL
		);

		return 0;

	case WM_CLOSE:
		bitmap_buffer.exit();
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		::BeginPaint(hwnd, &ps);
		bitmap_buffer.draw_panel_main(rect_sepr.rect);
		::EndPaint(hwnd, &ps);
		return 0;
	}

	case WM_SIZE:
		bitmap_buffer.resize();
		g_window_menu.move(rect_menu.rect);
		g_window_editor.move(rect_editor.rect);
		g_window_library.move(rect_library.rect);

		return 0;

	case WM_LBUTTONDOWN:
		if (::PtInRect(&rect_sepr.rect, pt_client)) {
			is_separator_moving = true;
			if (is_vertical)
				is_sepr_invalid_ver = false;
			else
				is_sepr_invalid_hor = false;
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
			if (is_vertical)
				::SetCursor(LoadCursor(NULL, IDC_SIZENS));
			else
				::SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		}
		if (is_separator_moving) {
			if (is_vertical)
				g_config.separator = std::clamp(
					(int)(rect_wnd.bottom - pt_client.y),
					CVE_SEPARATOR_WIDTH,
					(int)rect_wnd.bottom - CVE_SEPARATOR_WIDTH - CVE_MENU_H
				);
			else
				g_config.separator = std::clamp(
					(int)(rect_wnd.right - pt_client.x),
					CVE_SEPARATOR_WIDTH,
					(int)rect_wnd.right - CVE_MIN_W + CVE_SEPARATOR_WIDTH
				);

			g_window_main.redraw();
		}
		return 0;

	case aului::Window::WM_REDRAW:
	{
		HDWP dwp;
		::InvalidateRect(hwnd, NULL, FALSE);
		dwp = ::BeginDeferWindowPos(3);
		::DeferWindowPos(
			dwp,
			g_window_menu.hwnd,
			NULL,
			rect_menu.rect.left,
			rect_menu.rect.top,
			rect_menu.rect.right - rect_menu.rect.left,
			rect_menu.rect.bottom - rect_menu.rect.top,
			SWP_NOZORDER | SWP_NOACTIVATE
		);
		::DeferWindowPos(
			dwp,
			g_window_editor.hwnd,
			NULL,
			rect_editor.rect.left,
			rect_editor.rect.top,
			rect_editor.rect.right - rect_editor.rect.left,
			rect_editor.rect.bottom - rect_editor.rect.top,
			SWP_NOZORDER | SWP_NOACTIVATE
		);
		::DeferWindowPos(
			dwp,
			g_window_library.hwnd,
			NULL,
			rect_library.rect.left,
			rect_library.rect.top,
			rect_library.rect.right - rect_library.rect.left,
			rect_library.rect.bottom - rect_library.rect.top,
			SWP_NOZORDER | SWP_NOACTIVATE
		);
		::EndDeferWindowPos(dwp);
		/*g_window_menu.move(rect_menu.rect);
		g_window_editor.move(rect_editor.rect);
		g_window_library.move(rect_library.rect);*/
		g_window_menu.redraw();
		g_window_editor.redraw();
		g_window_library.redraw();
		return 0;
	}

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
}