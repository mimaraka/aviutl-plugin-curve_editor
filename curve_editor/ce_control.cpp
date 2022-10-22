//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル(コントロール)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		コントロールを作成
//---------------------------------------------------------------------
BOOL ce::Button::create(HWND hwnd_p, LPTSTR name, LPTSTR desc, int ic_or_str, LPTSTR ico_res_dark, LPTSTR ico_res_light, LPTSTR lb, int ct_id, LPRECT rect, int flag)
{
	WNDCLASSEX tmp;
	id = ct_id;
	description = desc;
	edge_flag = flag;
	hwnd_parent = hwnd_p;

	icon_or_str = ic_or_str;
	if (ic_or_str) {
		label = lb;
	}
	else {
		label = "";
		icon_res_dark = ico_res_dark;
		icon_res_light = ico_res_light;
	}


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
		hwnd = ::CreateWindowEx(
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
//		コントロール描画用の関数
//---------------------------------------------------------------------
void ce::Button::draw(COLORREF bg, LPRECT rect_wnd, LPTSTR content)
{
	d2d_setup(&canvas, rect_wnd, TO_BGR(bg));

	::SetBkColor(canvas.hdc_memory, bg);

	// 文字列を描画
	if (icon_or_str == 1) {
		::SelectObject(canvas.hdc_memory, font);
		::DrawText(
			canvas.hdc_memory,
			content,
			strlen(content),
			rect_wnd,
			DT_CENTER | DT_VCENTER | DT_NOCLIP | DT_SINGLELINE
		);
		::DeleteObject(font);
	}
	// アイコンを描画
	else {
		::DrawIcon(
			canvas.hdc_memory,
			(rect_wnd->right - CE_ICON_SIZE) / 2,
			(rect_wnd->bottom - CE_ICON_SIZE) / 2,
			g_config.theme ? icon_light : icon_dark
		);
	}

	if (g_render_target != NULL) {
		g_render_target->BeginDraw();
		if (brush == NULL) g_render_target->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &brush);
		d2d_draw_rounded_edge(brush, rect_wnd, edge_flag, CE_ROUND_RADIUS);

		g_render_target->EndDraw();
	}

	canvas.transfer(rect_wnd);
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ(static変数使用不可)
//---------------------------------------------------------------------
LRESULT ce::Button::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;

	::GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	case WM_CREATE:
		canvas.init(hwnd);

		if (icon_or_str == 0) {
			icon_dark = ::LoadIcon(g_fp->dll_hinst, icon_res_dark);
			icon_light = ::LoadIcon(g_fp->dll_hinst, icon_res_light);

			hwnd_tooltip = ::CreateWindowEx(
				0, TOOLTIPS_CLASS,
				NULL, TTS_ALWAYSTIP,
				CW_USEDEFAULT, CW_USEDEFAULT,
				CW_USEDEFAULT, CW_USEDEFAULT,
				hwnd, NULL, g_fp->dll_hinst,
				NULL
			);

			tool_info.cbSize = sizeof(TOOLINFO);
			tool_info.uFlags = TTF_SUBCLASS;
			tool_info.hwnd = hwnd;
			tool_info.uId = id;
			tool_info.lpszText = description;
			::SendMessage(hwnd_tooltip, TTM_ADDTOOL, 0, (LPARAM)&tool_info);
		}

		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hwnd;

		// フォント
		font = ::CreateFont(
			CE_CT_FONT_H, 0,
			0, 0,
			FW_REGULAR,
			FALSE, FALSE, FALSE,
			ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			NULL,
			CE_FONT_YU_GOTHIC
		);
		return 0;

	case WM_CLOSE:
		canvas.exit();
		return 0;

	case WM_SIZE:
		if (!icon_or_str) {
			tool_info.rect = rect_wnd;
			::SendMessage(hwnd_tooltip, TTM_NEWTOOLRECT, 0, (LPARAM)&tool_info);
		}
		return 0;

	// 描画
	case WM_PAINT:
	{
		COLORREF bg;
		
		if (clicked)
			bg = BRIGHTEN(g_theme[g_config.theme].bg, CE_CT_BR_CLICKED);
		else if (hovered)
			bg = BRIGHTEN(g_theme[g_config.theme].bg, CE_CT_BR_HOVERED);
		else
			bg = g_theme[g_config.theme].bg;

		::SetTextColor(canvas.hdc_memory, g_theme[g_config.theme].bt_tx);

		draw(bg, &rect_wnd, label);
		return 0;
	}

	// マウスが動いたとき
	case WM_MOUSEMOVE:
		hovered = TRUE;
		::InvalidateRect(hwnd, NULL, FALSE);
		::TrackMouseEvent(&tme);
		return 0;

	case WM_SETCURSOR:
		::SetCursor(::LoadCursor(NULL, IDC_HAND));
		return 0;

	// 左クリックがされたとき
	case WM_LBUTTONDOWN:
		::SetCursor(::LoadCursor(NULL, IDC_HAND));
		clicked = TRUE;
		::InvalidateRect(hwnd, NULL, FALSE);
		::TrackMouseEvent(&tme);
		return 0;

	// 左クリックが終わったとき
	case WM_LBUTTONUP:
		::SetCursor(LoadCursor(NULL, IDC_HAND));
		clicked = FALSE;
		::SendMessage(hwnd_parent, WM_COMMAND, id, 0);
		::InvalidateRect(hwnd, NULL, FALSE);
		return 0;

	// マウスがウィンドウから離れたとき
	case WM_MOUSELEAVE:
		clicked = FALSE;
		hovered = FALSE;
		::InvalidateRect(hwnd, NULL, FALSE);
		return 0;

	// コマンド
	case WM_COMMAND:
		switch (wparam) {
		case CE_CM_REDRAW:
			::InvalidateRect(hwnd, NULL, FALSE);
			return 0;
		}
		return 0;

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ(スイッチ)
//---------------------------------------------------------------------
LRESULT ce::Button_Switch::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;

	::GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	// 描画
	case WM_PAINT:
	{
		COLORREF bg;
		// 選択時
		if (is_selected) {
			if (clicked)
				bg = BRIGHTEN(g_theme[g_config.theme].bt_selected, CE_CT_BR_CLICKED);
			else if (hovered)
				bg = BRIGHTEN(g_theme[g_config.theme].bt_selected, CE_CT_BR_HOVERED);
			else
				bg = g_theme[g_config.theme].bt_selected;

			::SetTextColor(canvas.hdc_memory, g_theme[g_config.theme].bt_tx_selected);
		}
		// 非選択時
		else {
			if (clicked)
				bg = BRIGHTEN(g_theme[g_config.theme].bt_unselected, CE_CT_BR_CLICKED);
			else if (hovered)
				bg = BRIGHTEN(g_theme[g_config.theme].bt_unselected, CE_CT_BR_HOVERED);
			else
				bg = g_theme[g_config.theme].bt_unselected;

			::SetTextColor(canvas.hdc_memory, g_theme[g_config.theme].bt_tx);
		}

		draw(bg, &rect_wnd, label);
		return 0;
	}

	// 左クリックが終わったとき
	case WM_LBUTTONUP:
		::SetCursor(LoadCursor(NULL, IDC_HAND));
		clicked = FALSE;
		if (!is_selected) {
			is_selected = TRUE;
			::SendMessage(hwnd_parent, WM_COMMAND, id, CE_CM_SELECTED);
		}
		::InvalidateRect(hwnd, NULL, FALSE);
		return 0;

	default:
		return Button::wndproc(hwnd, msg, wparam, lparam);
	}
}



//---------------------------------------------------------------------
//		選択状態を変更(スイッチ)
//---------------------------------------------------------------------
void ce::Button_Switch::set_status(BOOL bl)
{
	is_selected = bl;
	::InvalidateRect(hwnd, NULL, FALSE);
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ(ID)
//---------------------------------------------------------------------
LRESULT ce::Button_ID::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;
	POINT cl_pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };

	::GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	case WM_PAINT:
	{
		COLORREF bg;
		TCHAR id_text[5];
		::_itoa_s(g_config.current_id, id_text, 5, 10);

		if (clicked)
			bg = BRIGHTEN(g_theme[g_config.theme].bg, CE_CT_BR_CLICKED);
		else if (hovered)
			bg = BRIGHTEN(g_theme[g_config.theme].bg, CE_CT_BR_HOVERED);
		else
			bg = g_theme[g_config.theme].bg;

		::SetTextColor(canvas.hdc_memory, g_theme[g_config.theme].bt_tx);

		draw(bg, &rect_wnd, id_text);
		return 0;
	}

	// 左クリックがされたとき
	case WM_LBUTTONDOWN:
		pt_lock = cl_pt;
		id_buffer = g_config.current_id;
		::SetCursor(LoadCursor(NULL, IDC_HAND));

		if (::GetAsyncKeyState(VK_CONTROL) < 0)
			coef_move = CE_COEF_MOVE_FAST;
		else
			coef_move = CE_COEF_MOVE_DEFAULT;

		clicked = TRUE;
		::InvalidateRect(hwnd, NULL, FALSE);
		::SetCapture(hwnd);
		return 0;

	// カーソルが動いたとき
	case WM_MOUSEMOVE:
		if (clicked && g_config.mode == ce::Config::ID) {
			is_scrolling = TRUE;
			::SetCursor(LoadCursor(NULL, IDC_SIZEWE));
			g_config.current_id = MINMAXLIM(id_buffer + (cl_pt.x - pt_lock.x) / coef_move, 0, CE_CURVE_MAX - 1);
			g_curve_id_previous = g_curve_id[g_config.current_id];
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_REDRAW, 0);
		}
		else ::SetCursor(LoadCursor(NULL, IDC_HAND));

		hovered = TRUE;
		::InvalidateRect(hwnd, NULL, FALSE);
		::TrackMouseEvent(&tme);
		return 0;

	case WM_LBUTTONUP:
		if (!is_scrolling && clicked && g_config.mode == ce::Config::ID) {
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_ID), hwnd, dialogproc_id);
		}
		is_scrolling = FALSE;
		clicked = FALSE;
		::ReleaseCapture();
		::InvalidateRect(hwnd, NULL, FALSE);
		return 0;

	default:
		return Button::wndproc(hwnd, msg, wparam, lparam);
	}
}