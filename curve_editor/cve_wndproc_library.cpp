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
	static cve::Edit_Box search;
	POINT pt_client = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
	RECT rect_wnd;
	aului::Window_Rectangle rect_preset_list, rect_search_bar;
	static cve::My_Direct2d_Paint_Object bitmap_buffer;

	::GetClientRect(hwnd, &rect_wnd);

	rect_preset_list.set(
		0,
		CVE_LIBRARY_SEARCHBAR_HEIGHT + CVE_MARGIN * 2,
		rect_wnd.right,
		rect_wnd.bottom
	);

	rect_search_bar.set(
		CVE_MARGIN,
		CVE_MARGIN,
		rect_wnd.right - CVE_MARGIN,
		CVE_MARGIN + CVE_LIBRARY_SEARCHBAR_HEIGHT
	);


	switch (msg) {
	case WM_CREATE:
		bitmap_buffer.init(hwnd, g_p_factory, g_p_write_factory);

		// プリセットのリスト
		g_window_preset_list.create(
			g_fp->dll_hinst,
			hwnd,
			"WINDOW_PRESET_LIST",
			wndproc_preset_list,
			NULL,
			CS_DBLCLKS,
			rect_preset_list.rect,
			NULL
		);

#ifdef _DEBUG

		search.init(
			hwnd,
			"CVE_SEARCHBAR",
			20,
			CVE_CT_SEARCHBAR,
			rect_search_bar.rect,
			cve::Button::FLAG_EDGE_ALL
		);

#endif

		return 0;

	case WM_CLOSE:
		bitmap_buffer.exit();
		return 0;

	case WM_SIZE:
		bitmap_buffer.resize();
		g_window_preset_list.move(rect_preset_list.rect);
#ifdef _DEBUG
		search.move(rect_search_bar.rect);
#endif
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		::BeginPaint(hwnd, &ps);
		bitmap_buffer.draw_panel();
		::EndPaint(hwnd, &ps);
		return 0;
	}

	case aului::Window::WM_REDRAW:
		::InvalidateRect(hwnd, NULL, FALSE);
		g_window_preset_list.redraw();
		search.redraw();
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
	static cve::My_Direct2d_Paint_Object bitmap_buffer;

	::GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	case WM_CREATE:
		bitmap_buffer.init(hwnd, g_p_factory, g_p_write_factory);

		return 0;

	case WM_CLOSE:
		bitmap_buffer.exit();
		return 0;

	case WM_SIZE:
		bitmap_buffer.resize();
		::PostMessage(hwnd, WM_COMMAND, CVE_CM_PRESET_MOVE, 0);

		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		::BeginPaint(hwnd, &ps);
		bitmap_buffer.draw_panel();
		::EndPaint(hwnd, &ps);
		return 0;
	}

	case WM_MOUSEWHEEL:
		if (::GetAsyncKeyState(VK_CONTROL) < 0) {
			g_config.preset_size += GET_Y_LPARAM(wparam) / 60;
			::SendMessage(hwnd, WM_COMMAND, CVE_CM_PRESET_MOVE, 0);
			::InvalidateRect(hwnd, NULL, FALSE);
		}
		else
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

	case aului::Window::WM_REDRAW:
		::InvalidateRect(hwnd, NULL, FALSE);
		return 0;

	case WM_COMMAND:
		switch (wparam) {
		case CVE_CM_PRESET_MOVE:
			switch (g_config.edit_mode) {
			case cve::Mode_Bezier:
				break;

			case cve::Mode_Bezier_Multi:
				break;
			}
			// 後で消す
			for (int i = 0; i < (int)g_presets_bezier_custom.size; i++) {
				g_presets_bezier_custom[i].move(rect_wnd.right, i);
			}
			return 0;
		}
		return 0;

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
}