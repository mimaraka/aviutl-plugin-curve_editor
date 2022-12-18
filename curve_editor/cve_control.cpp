//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル(コントロール)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		コントロールを作成
//---------------------------------------------------------------------
BOOL cve::Button::initialize(
	HWND			hwnd_p,
	LPCTSTR			name,
	LPCTSTR			desc,
	Content_Type	cont_type,
	LPCTSTR			ico_res_dark,
	LPCTSTR			ico_res_light,
	LPCTSTR			lb,
	int				ct_id,
	const RECT&		rect,
	int				flag
)
{
	id = ct_id;
	description = (LPTSTR)desc;
	edge_flag = flag;
	hwnd_parent = hwnd_p;
	content_type = cont_type;
	hovered = false;
	clicked = false;
	disabled = false;

	if (content_type == Button::String) {
		if (lb != nullptr)
			strcpy_s(label, CVE_CT_LABEL_MAX, lb);
		else
			label[0] = NULL;
	}
	else if (content_type == Button::Icon) {
		icon_res_dark = (LPTSTR)ico_res_dark;
		icon_res_light = (LPTSTR)ico_res_light;
	}

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
//		コントロール描画用の関数
//---------------------------------------------------------------------
void cve::Button::draw_content(COLORREF bg, RECT* rect_content, LPCTSTR content, bool change_color)
{
	if (change_color) {
		if (clicked)
			bg = CHANGE_BRIGHTNESS(bg, CVE_CT_BR_CLICKED);
		else if (hovered)
			bg = CHANGE_BRIGHTNESS(bg, CVE_CT_BR_HOVERED);
	}

	bitmap_buffer.d2d_setup(TO_BGR(bg));

	::SetBkColor(bitmap_buffer.hdc_memory, bg);

	// 文字列を描画
	if (content_type == Button::String && content != nullptr) {
		::SelectObject(bitmap_buffer.hdc_memory, font);

		::DrawText(
			bitmap_buffer.hdc_memory,
			content,
			strlen(content),
			rect_content,
			DT_CENTER | DT_VCENTER | DT_NOCLIP | DT_SINGLELINE
		);

		::DeleteObject(font);
	}
	// アイコンを描画
	else if (content_type == Button::Icon) {
		::DrawIcon(
			bitmap_buffer.hdc_memory,
			(rect_content->right - CVE_ICON_SIZE) / 2,
			(rect_content->bottom - CVE_ICON_SIZE) / 2,
			g_config.theme ? icon_light : icon_dark
		);
	}
}



//---------------------------------------------------------------------
//		ラウンドエッジ(等)を描画
//---------------------------------------------------------------------
void cve::Button::draw_edge()
{
	if (g_render_target != nullptr) {
		g_render_target->BeginDraw();

		if (disabled) {
			bitmap_buffer.brush->SetColor(D2D1::ColorF(TO_BGR(g_theme[g_config.theme].bg)));
			bitmap_buffer.brush->SetOpacity(0.5f);
			g_render_target->FillRectangle(
				D2D1::RectF(
					(float)bitmap_buffer.rect.left,
					(float)bitmap_buffer.rect.top,
					(float)bitmap_buffer.rect.right,
					(float)bitmap_buffer.rect.bottom
				),
				bitmap_buffer.brush);
			bitmap_buffer.brush->SetOpacity(1);
		}

		bitmap_buffer.draw_rounded_edge(edge_flag, CVE_ROUND_RADIUS);

		g_render_target->EndDraw();
	}
}



//---------------------------------------------------------------------
//		フォントを設定
//---------------------------------------------------------------------
void cve::Button::set_font(int font_height, LPTSTR font_name)
{
	font = ::CreateFont(
		font_height, 0,
		0, 0,
		FW_REGULAR,
		FALSE, FALSE, FALSE,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		NULL,
		font_name
	);
}



//---------------------------------------------------------------------
//		ボタンの状態を設定
//---------------------------------------------------------------------
void cve::Button::set_status(int flags)
{
	if (flags & CVE_BUTTON_ENABLED)
		disabled = false;
	else if (flags & CVE_BUTTON_DISABLED)
		disabled = true;

	::SendMessage(hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ(static変数使用不可)
//---------------------------------------------------------------------
LRESULT cve::Button::wndproc(HWND hw, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;

	::GetClientRect(hw, &rect_wnd);

	switch (msg) {
	case WM_CREATE:
		bitmap_buffer.init(hw);
		bitmap_buffer.set_size(rect_wnd);

		// アイコンだった場合
		if (content_type == Button::Icon) {
			icon_dark = ::LoadIcon(g_fp->dll_hinst, icon_res_dark);
			icon_light = ::LoadIcon(g_fp->dll_hinst, icon_res_light);

			hwnd_tooltip = ::CreateWindowEx(
				0, TOOLTIPS_CLASS,
				NULL, TTS_ALWAYSTIP,
				CW_USEDEFAULT, CW_USEDEFAULT,
				CW_USEDEFAULT, CW_USEDEFAULT,
				hw, NULL, g_fp->dll_hinst,
				NULL
			);

			tool_info.cbSize = sizeof(TOOLINFO);
			tool_info.uFlags = TTF_SUBCLASS;
			tool_info.hwnd = hw;
			tool_info.uId = id;
			tool_info.lpszText = (LPTSTR)description;
			::SendMessage(hwnd_tooltip, TTM_ADDTOOL, 0, (LPARAM)&tool_info);
		}

		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hw;

		set_font(CVE_CT_FONT_H, CVE_FONT_SEMIBOLD);
		return 0;

	case WM_CLOSE:
		bitmap_buffer.exit();
		return 0;

	case WM_SIZE:
		bitmap_buffer.set_size(rect_wnd);

		if (content_type == Button::Icon) {
			tool_info.rect = rect_wnd;
			::SendMessage(hwnd_tooltip, TTM_NEWTOOLRECT, 0, (LPARAM)&tool_info);
		}
		return 0;

	// 描画
	case WM_PAINT:
	{
		COLORREF bg = g_theme[g_config.theme].bg;

		::SetTextColor(bitmap_buffer.hdc_memory, g_theme[g_config.theme].bt_tx);

		draw_content(bg, &rect_wnd, label, true);

		draw_edge();
		bitmap_buffer.transfer();

		return 0;
	}

	// マウスが動いたとき
	case WM_MOUSEMOVE:
		if (!disabled) {
			hovered = true;
			::InvalidateRect(hw, NULL, FALSE);
			::TrackMouseEvent(&tme);
		}
		return 0;

	case WM_SETCURSOR:
		::SetCursor(::LoadCursor(NULL, IDC_HAND));
		return 0;

	// 左クリックがされたとき
	case WM_LBUTTONDOWN:
		if (!disabled) {
			::SetCursor(::LoadCursor(NULL, IDC_HAND));
			clicked = true;
			::InvalidateRect(hw, NULL, FALSE);
			::TrackMouseEvent(&tme);
		}
		return 0;

	// 左クリックが終わったとき
	case WM_LBUTTONUP:
		if (!disabled) {
			::SetCursor(::LoadCursor(NULL, IDC_HAND));
			clicked = false;
			::SendMessage(hwnd_parent, WM_COMMAND, id, 0);
			::InvalidateRect(hw, NULL, FALSE);
		}
		return 0;

	// マウスがウィンドウから離れたとき
	case WM_MOUSELEAVE:
		clicked = false;
		hovered = false;
		::InvalidateRect(hw, NULL, FALSE);
		return 0;

	// コマンド
	case WM_COMMAND:
		switch (wparam) {
		case CVE_CM_REDRAW:
			::InvalidateRect(hw, NULL, FALSE);
			return 0;

		case CVE_CM_CHANGE_LABEL:
		{
			strcpy_s(label, CVE_CT_LABEL_MAX, (LPTSTR)lparam);
			::InvalidateRect(hw, NULL, FALSE);
			return 0;
		}

		case CVE_CM_SET_STATUS:
			set_status(lparam);
			return 0;
		}
		return 0;

	default:
		return ::DefWindowProc(hw, msg, wparam, lparam);
	}
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ(スイッチ)
//---------------------------------------------------------------------
LRESULT cve::Button_Switch::wndproc(HWND hw, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;

	::GetClientRect(hw, &rect_wnd);

	switch (msg) {
	// 描画
	case WM_PAINT:
	{
		COLORREF bg;
		// 選択時
		if (is_selected) {
			if (clicked)
				bg = CHANGE_BRIGHTNESS(g_theme[g_config.theme].bt_selected, CVE_CT_BR_CLICKED);
			else if (hovered)
				bg = CHANGE_BRIGHTNESS(g_theme[g_config.theme].bt_selected, CVE_CT_BR_HOVERED);
			else
				bg = g_theme[g_config.theme].bt_selected;

			::SetTextColor(bitmap_buffer.hdc_memory, g_theme[g_config.theme].bt_tx_selected);
		}
		// 非選択時
		else {
			if (clicked)
				bg = CHANGE_BRIGHTNESS(g_theme[g_config.theme].bt_unselected, CVE_CT_BR_CLICKED);
			else if (hovered)
				bg = CHANGE_BRIGHTNESS(g_theme[g_config.theme].bt_unselected, CVE_CT_BR_HOVERED);
			else
				bg = g_theme[g_config.theme].bt_unselected;

			::SetTextColor(bitmap_buffer.hdc_memory, g_theme[g_config.theme].bt_tx);
		}

		draw_content(bg, &rect_wnd, label, false);
		draw_edge();
		bitmap_buffer.transfer();

		return 0;
	}

	// 左クリックが終わったとき
	case WM_LBUTTONUP:
		::SetCursor(LoadCursor(NULL, IDC_HAND));
		clicked = false;
		if (!is_selected) {
			is_selected = true;
			::SendMessage(hwnd_parent, WM_COMMAND, id, CVE_CM_SELECTED);
		}
		::InvalidateRect(hw, NULL, FALSE);
		return 0;
	}
	return Button::wndproc(hw, msg, wparam, lparam);
}



//---------------------------------------------------------------------
//		選択状態を変更(スイッチ)
//---------------------------------------------------------------------
void cve::Button_Switch::set_status(BOOL bl)
{
	is_selected = bl;
	::InvalidateRect(hwnd, NULL, FALSE);
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ(パラメータ)
//---------------------------------------------------------------------
LRESULT cve::Button_Param::wndproc(HWND hw, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;

	::GetClientRect(hw, &rect_wnd);

	switch (msg) {
	case WM_PAINT:
	{
		COLORREF bg = g_theme[g_config.theme].bg;
		std::string str_param;
		LPTSTR param = "";

		::SetTextColor(bitmap_buffer.hdc_memory, g_theme[g_config.theme].bt_tx);

		switch (g_config.edit_mode) {
		case cve::Mode_Bezier:
			str_param = g_curve_bezier.create_parameters();
			param = const_cast<LPTSTR>(str_param.c_str());
			break;

		case cve::Mode_Elastic:
			str_param = std::to_string(g_curve_elastic.create_number());
			param = const_cast<LPTSTR>(str_param.c_str());
			break;

		case cve::Mode_Bounce:
			str_param = std::to_string(g_curve_bounce.create_number());
			param = const_cast<LPTSTR>(str_param.c_str());
			break;
		}

		draw_content(bg, &rect_wnd, param, true);
		draw_edge();
		bitmap_buffer.transfer();

		return 0;
	}
	}
	return Button::wndproc(hw, msg, wparam, lparam);
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ(ID)
//---------------------------------------------------------------------
LRESULT cve::Button_ID::wndproc(HWND hw, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;
	POINT pt_client = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
	int* current_id = nullptr;

	switch (g_config.edit_mode) {
	case Mode_Multibezier:
		current_id = &g_config.current_id.multibezier;
		break;

	case Mode_Value:
	default:
		current_id = &g_config.current_id.value;
		break;
	}

	::GetClientRect(hw, &rect_wnd);

	switch (msg) {
	case WM_PAINT:
	{
		COLORREF bg = g_theme[g_config.theme].bg;
		TCHAR id_text[5];

		::_itoa_s(g_config.current_id.multibezier, id_text, 5, 10);

		::SetTextColor(bitmap_buffer.hdc_memory, g_theme[g_config.theme].bt_tx);

		draw_content(bg, &rect_wnd, id_text, true);
		draw_edge();
		bitmap_buffer.transfer();

		return 0;
	}

	// 左クリックがされたとき
	case WM_LBUTTONDOWN:
		pt_lock = pt_client;
		id_buffer = *current_id;

		::SetCursor(LoadCursor(NULL, IDC_HAND));

		if (::GetAsyncKeyState(VK_CONTROL) < 0)
			coef_move = CVE_COEF_MOVE_FAST;
		else
			coef_move = CVE_COEF_MOVE_DEFAULT;

		clicked = true;

		::InvalidateRect(hw, NULL, FALSE);
		::SetCapture(hw);

		return 0;

	// カーソルが動いたとき
	case WM_MOUSEMOVE:
		if (clicked && g_config.edit_mode == Mode_Multibezier) {
			is_scrolling = true;

			::SetCursor(LoadCursor(NULL, IDC_SIZEWE));

			*current_id = MINMAX_LIMIT(id_buffer + (pt_client.x - pt_lock.x) / coef_move, 1, CVE_CURVE_MAX);
			switch (g_config.edit_mode) {
			case Mode_Multibezier:
				g_curve_mb_previous = g_curve_mb[*current_id - 1];
				break;

			case Mode_Value:
				g_curve_value_previous = g_curve_value[*current_id - 1];
				break;
			}

			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);
		}
		else ::SetCursor(::LoadCursor(NULL, IDC_HAND));

		hovered = true;

		::InvalidateRect(hw, NULL, FALSE);
		::TrackMouseEvent(&tme);

		return 0;

	case WM_LBUTTONUP:
		if (!is_scrolling && clicked)
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_ID), hw, dialogproc_id);
		
		is_scrolling = false;
		clicked = false;

		::ReleaseCapture();
		::InvalidateRect(hw, NULL, FALSE);

		return 0;
	}
	return Button::wndproc(hw, msg, wparam, lparam);
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ(カテゴリー)
//---------------------------------------------------------------------
LRESULT cve::Button_Category::wndproc(HWND hw, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;
	POINT pt_client = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };

	::GetClientRect(hw, &rect_wnd);

	switch (msg) {
	case WM_CREATE:
	{
		LRESULT result = Button::wndproc(hw, msg, wparam, lparam);

		set_font(14, CVE_FONT_REGULAR);

		return result;
	}

	case WM_PAINT:
	{
		COLORREF bg = g_theme[g_config.theme].bg;
		COLORREF text_color;

		if (clicked)
			text_color = CHANGE_BRIGHTNESS(g_theme[g_config.theme].preset_tx, CVE_CT_BR_CLICKED);
		else if (hovered)
			text_color = CHANGE_BRIGHTNESS(g_theme[g_config.theme].preset_tx, CVE_CT_BR_HOVERED);
		else
			text_color = g_theme[g_config.theme].preset_tx;

		::SetTextColor(bitmap_buffer.hdc_memory, text_color);

		draw_content(bg, &rect_wnd, "▼ Default(23)", false);
		bitmap_buffer.transfer();

		return 0;
	}

	case WM_LBUTTONUP:
		if (clicked) {

		}
		return 0;
	}
	return Button::wndproc(hw, msg, wparam, lparam);
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ(色プレビュー&選択ボタン)
//---------------------------------------------------------------------
LRESULT cve::Button_Color::wndproc(HWND hw, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;

	::GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	case WM_CREATE:
		bg_color = NULL;
		break;

	case WM_PAINT:
	{
		COLORREF bg = bg_color;

		::SetTextColor(bitmap_buffer.hdc_memory, g_theme[g_config.theme].bt_tx);

		draw_content(bg, &rect_wnd, label, true);
		draw_edge();
		bitmap_buffer.transfer();

		return 0;
	}

	case WM_COMMAND:
		switch (wparam) {
		case CVE_CM_CHANGE_COLOR:
			bg_color = (COLORREF)lparam;
			::InvalidateRect(hw, NULL, FALSE);

			return 0;
		}
		break;
	}
	return Button::wndproc(hw, msg, wparam, lparam);
}