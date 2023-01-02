//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル(エディットボックス)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		コントロールを作成
//---------------------------------------------------------------------
BOOL cve::Edit_Box::init(
	HWND			hwnd_p,
	LPCTSTR			name,
	int				l_height,
	int				ct_id,
	const RECT&		rect,
	int				flag
)
{
	id = ct_id;
	edge_flag = flag;
	hwnd_parent = hwnd_p;
	line_height = l_height;

	return create(
		hwnd_p,
		name,
		wndproc_static,
		NULL,
		NULL,
		rect,
		this
	);
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ
//---------------------------------------------------------------------
LRESULT cve::Edit_Box::wndproc(HWND hw, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;

	::GetClientRect(hw, &rect_wnd);

	switch (msg) {
	case WM_CREATE:
		bitmap_buffer.init(hw);
		bitmap_buffer.set_size(rect_wnd);

		editbox = ::CreateWindowEx(
			NULL,
			"EDIT",
			NULL,
			WS_CHILD | WS_VISIBLE,
			CVE_MARGIN,
			(rect_wnd.bottom - line_height) / 2,
			rect_wnd.right - CVE_MARGIN * 2,
			line_height,
			hw,
			NULL,
			g_fp->dll_hinst,
			NULL
		);

		set_font(line_height, CVE_FONT_REGULAR);
		::SendMessage(editbox, WM_SETFONT, (WPARAM)font, MAKELPARAM(TRUE, 0));

		return 0;

	case WM_CLOSE:
		bitmap_buffer.exit();
		return 0;

	case WM_SIZE:
		bitmap_buffer.set_size(rect_wnd);
		::MoveWindow(editbox,
			CVE_MARGIN,
			(rect_wnd.bottom - line_height) / 2,
			rect_wnd.right - CVE_MARGIN * 2,
			line_height,
			TRUE
		);
		return 0;

		// 描画
	case WM_PAINT:
	{
		COLORREF bg = g_theme[g_config.theme].bg_editbox;

		bitmap_buffer.d2d_setup(TO_BGR(bg));

		draw_edge();
		bitmap_buffer.transfer();

		return 0;
	}

	case WM_CTLCOLOREDIT:
	{
		HDC hdc_edit = (HDC)wparam;
		if (editbox == (HWND)lparam) {
			SetBkColor(hdc_edit, g_theme[g_config.theme].bg_editbox);
			SetTextColor(hdc_edit, g_theme[g_config.theme].editbox_tx);
		}
	}
		return 0;

	case WM_SETCURSOR:
		::SetCursor(::LoadCursor(NULL, IDC_IBEAM));
		return 0;

		// コマンド
	case WM_COMMAND:
		switch (wparam) {
		case CVE_CM_REDRAW:
			::InvalidateRect(hw, NULL, FALSE);
			::InvalidateRect(editbox, NULL, FALSE);
			return 0;
		}
		return 0;

	default:
		return ::DefWindowProc(hw, msg, wparam, lparam);
	}
}