//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル(ウィンドウプロシージャ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		ウィンドウプロシージャ（メイン）
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_main(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static BOOL is_separator_moving = FALSE;
	POINT cl_pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
	RECT	rect_wnd,
			rect_sepr,
			rect_editor,
			rect_header,
			rect_preset;

	static ce::Bitmap_Canvas canvas;

	::GetClientRect(hwnd, &rect_wnd);

	rect_sepr = {
		0,
		rect_wnd.bottom - g_config.separator - CE_SEPR_W,
		rect_wnd.right,
		rect_wnd.bottom - g_config.separator + CE_SEPR_W
	};

	rect_header = {
		0,
		0,
		rect_wnd.right,
		CE_HEADER_H
	};

	rect_editor = {
		0,
		CE_HEADER_H,
		rect_wnd.right,
		rect_wnd.bottom - g_config.separator - CE_SEPR_W
	};
	rect_set_margin(&rect_editor, CE_MARGIN, 0, CE_MARGIN, CE_MARGIN);

	rect_preset = {
		0,
		rect_wnd.bottom - g_config.separator + CE_SEPR_W,
		rect_wnd.right,
		rect_wnd.bottom
	};


	switch (msg) {
	case WM_CLOSE:
		canvas.exit();
		return 0;

	case WM_CREATE:
		canvas.init(hwnd);

		// ヘッダパネル
		g_window_header.create(
			hwnd, "WINDOW_FOOTER", wndproc_header, NULL,
			&rect_header
		);

		// エディタパネル
		g_window_editor.create(
			hwnd, "WINDOW_GPATH", wndproc_editor, NULL,
			&rect_editor
		);

		// プリセットパネル
		g_window_preset.create(
			hwnd, "WINDOW_PRESET", wndproc_preset, NULL,
			&rect_preset
		);

		return 0;

	case WM_PAINT:
		draw_panel_main(&canvas, &rect_wnd, &rect_sepr);
		return 0;

	case WM_SIZE:

		g_window_header.move(&rect_header);
		g_window_editor.move(&rect_editor);
		g_window_preset.move(&rect_preset);
		return 0;

	case WM_LBUTTONDOWN:
		if (::PtInRect(&rect_sepr, cl_pt)) {
			is_separator_moving = TRUE;
			::SetCursor(LoadCursor(NULL, IDC_SIZENS));
			::SetCapture(hwnd);
		}
		return 0;

	case WM_LBUTTONUP:
		is_separator_moving = FALSE;
		::ReleaseCapture();
		return 0;

	case WM_MOUSEMOVE:
		if (::PtInRect(&rect_sepr, cl_pt)) ::SetCursor(LoadCursor(NULL, IDC_SIZENS));
		if (is_separator_moving) {
			g_config.separator = MINMAXLIM(rect_wnd.bottom - cl_pt.y, CE_SEPR_W, rect_wnd.bottom - CE_SEPR_W - CE_HEADER_H);
			g_window_header.move(&rect_header);
			g_window_editor.move(&rect_editor);
			g_window_preset.move(&rect_preset);
			::InvalidateRect(hwnd, NULL, FALSE);
		}
		return 0;

	case WM_COMMAND:
		switch (wparam) {
		case CE_CM_REDRAW:
			::InvalidateRect(hwnd, NULL, FALSE);
			g_window_header.redraw();
			g_window_editor.redraw();
			g_window_preset.redraw();
		}

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ（エディタパネル）
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_editor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//int
	static int		move_pt			= 0;			//0:None, 1:Ctpt1, 2:Ctpt2
	static BOOL		move_view		= 0;
	static int		move_or_scale	= NULL;
	static int		is_dragging		= FALSE;
	static int		id_hoverred		= -1;

	//double
	static double	prev_scale_x, prev_scale_y;
	static float	prev_o_x, prev_o_y;

	//POINT
	static POINT	pt_view;
	POINT			cl_pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
	POINT			gr_pt;

	//RECT
	RECT			rect_wnd;

	//ハンドル
	static HMENU	menu;
	static HWND		hwnd_obj;
	static HWND		hwnd_exedit;

	//その他
	static ce::Point_Address address;
	static ce::Bitmap_Canvas canvas;
	static MENUITEMINFO minfo;


	//クライアント領域の矩形を取得
	::GetClientRect(hwnd, &rect_wnd);
	//グラフ座標
	if (g_view_info.origin.x != NULL)
		gr_pt = to_graph(cl_pt);

	switch (msg) {
	case WM_CLOSE:
		canvas.exit();
		return 0;

		///////////////////////////////////////////
		//		WM_CREATE
		//		(ウィンドウが作成されたとき)
		///////////////////////////////////////////
	case WM_CREATE:
	{
		canvas.init(hwnd);

		// メニュー
		menu = ::GetSubMenu(LoadMenu(g_fp->dll_hinst, MAKEINTRESOURCE(IDR_MENU1)), 0);

		minfo.cbSize = sizeof(MENUITEMINFO);

		// 拡張編集とオブジェクト設定ダイアログのウィンドウハンドルの取得
		hwnd_exedit = auls::Exedit_GetWindow(g_fp);
		hwnd_obj = auls::ObjDlg_GetWindow(hwnd_exedit);

		g_curve_value_previous = g_curve_value;
		g_curve_id_previous = g_curve_id[g_config.current_id];

		g_view_info.fit(&rect_wnd);

		return 0;
	}

		///////////////////////////////////////////
		//		WM_SIZE
		//		(ウィンドウのサイズが変更されたとき)
		///////////////////////////////////////////
	case WM_SIZE:

		return 0;

		///////////////////////////////////////////
		//		WM_PAINT
		//		(ウィンドウが描画されたとき)
		///////////////////////////////////////////
	case WM_PAINT:
		draw_panel_editor(&canvas, &rect_wnd);
		return 0;

		///////////////////////////////////////////
		//		WM_LBUTTONDOWN
		//		(左クリックがされたとき)
		///////////////////////////////////////////
	case WM_LBUTTONDOWN:
		// 値モード&制御点がクリックされたとき
		if (g_config.mode == ce::Config::Value && g_curve_value.point_in_ctpts(cl_pt) >= 1) {
			move_pt = g_curve_value.point_in_ctpts(cl_pt);
			g_curve_value_previous = g_curve_value;
			::SetCursor(LoadCursor(NULL, IDC_HAND));
			::SetCapture(hwnd);
		}
		// 値モードでないとき
		else {
			// カーソルが制御点上にあるかどうか
			address = g_curve_id[g_config.current_id].pt_in_ctpt(cl_pt);
			// カーソルが制御点上にあるとき，ハンドルの座標を記憶
			if (address.position != ce::Point_Address::Null) {
				g_curve_id[g_config.current_id].move_point(address, gr_pt, TRUE);
				g_curve_id_previous = g_curve_id[g_config.current_id];
				::SetCursor(LoadCursor(NULL, IDC_HAND));
				::SetCapture(hwnd);
			}
			// カーブのD&D処理
			else {
				is_dragging = TRUE;
				::SetCapture(hwnd);
			}
		}
		return 0;

		///////////////////////////////////////////
		//		WM_LBUTTONUP
		//		(マウスの左ボタンが離されたとき)
		///////////////////////////////////////////
	case WM_LBUTTONUP:
		// Valueモード
		if (g_config.mode == ce::Config::Value && move_pt) {
			// 移動モード解除
			move_pt = 0;
			if (g_config.auto_copy)
				::SendMessage(hwnd, WM_COMMAND, CE_CM_COPY, 0);
		}
		// IDモード・カーブのD&D処理
		else {
			if (id_hoverred >= 0) {
				g_config.is_hooked_popup = TRUE;
				g_config.is_hooked_dialog = TRUE;

				::SendMessage(hwnd_obj, WM_COMMAND, 4000 + id_hoverred, 0);

				// 競合プラグイン対策
				FILTER* fp_quick_easing_setup = auls::AviUtl_GetFilter(g_fp, EASING_QUICK_SETUP);

				if (!fp_quick_easing_setup || g_fp->exfunc->ini_load_int(fp_quick_easing_setup, "auto_popup", 0) != 1)
					::PostMessage(hwnd_obj, WM_COMMAND, 0x462, id_hoverred);
			}

			address.position = ce::Point_Address::Null;
		}

		is_dragging = FALSE;

		// Aviutlを再描画
		::SendMessage(g_fp->hwnd, WM_COMMAND, CE_CM_REDRAW_AVIUTL, 0);
		::ReleaseCapture();
		return 0;

		///////////////////////////////////////////
		//		WM_MBUTTONDOWN
		//		(マウスの左ボタンがダブルクリックされたとき)
		///////////////////////////////////////////
	case WM_LBUTTONDBLCLK:
		// IDモードのとき
		if (g_config.mode == ce::Config::ID) {
			//カーソルが制御点上にあるかどうか
			address = g_curve_id[g_config.current_id].pt_in_ctpt(cl_pt);

			if (address.position == ce::Point_Address::Center)
				g_curve_id[g_config.current_id].delete_point(cl_pt);
			else if (ISINRANGEEQ(gr_pt.x, 0, CE_GR_RESOLUTION)) {
				g_curve_id_previous = g_curve_id[g_config.current_id];
				g_curve_id[g_config.current_id].add_point(gr_pt);
			}

			::InvalidateRect(hwnd, NULL, FALSE);
		}
		return 0;

		///////////////////////////////////////////
		//		WM_RBUTTONUP
		//		(マウスの右ボタンが離されたとき)
		///////////////////////////////////////////
		//右クリックメニュー
	case WM_RBUTTONUP:
		apply_config_to_menu(menu, &minfo);
		::ClientToScreen(hwnd, &cl_pt);
		::TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN, cl_pt.x, cl_pt.y, 0, hwnd, NULL);
		return 0;
	

		///////////////////////////////////////////
		//		WM_MBUTTONDOWN
		//		(マウスの中央ボタンが押されたとき)
		///////////////////////////////////////////
	case WM_MBUTTONDOWN:
		move_view		= TRUE;
		pt_view			= cl_pt;
		prev_scale_x	= g_view_info.scale.x;
		prev_scale_y	= g_view_info.scale.y;
		prev_o_x		= g_view_info.origin.x;
		prev_o_y		= g_view_info.origin.y;
		SetCursor(LoadCursor(NULL, IDC_SIZEALL));
		SetCapture(hwnd);
		return 0;

		///////////////////////////////////////////
		//		WM_MBUTTONUP
		//		(マウスの中央ボタンが離されたとき)
		///////////////////////////////////////////
	case WM_MBUTTONUP:
		move_view = FALSE;
		move_or_scale = NULL;
		ReleaseCapture();
		return 0;

		///////////////////////////////////////////
		//		WM_MOUSEMOVE
		//		(ウィンドウ上でカーソルが動いたとき)
		///////////////////////////////////////////
	case WM_MOUSEMOVE:
		//制御点移動
		//Valueモード
		if (move_pt) {
			g_curve_value.move_point(move_pt - 1, gr_pt);
			::InvalidateRect(hwnd, NULL, FALSE);
		}
		//IDモード
		if (g_config.mode == ce::Config::ID) {
			if (address.position != ce::Point_Address::Null) {
				g_curve_id[g_config.current_id].move_point(address, gr_pt, FALSE);
				::InvalidateRect(hwnd, NULL, FALSE);
			}
		}

		//制御点ホバー時にカーソルを変更
		//Valueモード
		if (!is_dragging) {
			if (g_config.mode == ce::Config::Value) {
				if (g_curve_value.point_in_ctpts(cl_pt) > 0)
					::SetCursor(LoadCursor(NULL, IDC_HAND));
			}
			//IDモード
			else {
				if (g_curve_id[g_config.current_id].pt_in_ctpt(cl_pt).position != ce::Point_Address::Null)
					::SetCursor(LoadCursor(NULL, IDC_HAND));
			}
		}
		// カーブのD&D処理
		else {
			// カーソル
			if (!::PtInRect(&rect_wnd, cl_pt))
				::SetCursor(LoadCursor(g_fp->dll_hinst, MAKEINTRESOURCE(IDC_DRAGGING)));

			// 枠を描画
			RECT rect_highlight;
			::ClientToScreen(hwnd, &cl_pt);
			
			// 先ほどまでハイライトしていたRECTと現在ホバーしているRECTが異なる場合
			if (id_hoverred != point_to_id(hwnd_obj, cl_pt)) {
				// 先ほどまで何らかのRECTをハイライトしていた場合
				if (id_hoverred >= 0) {
					POINT pt_rect[2];
					id_to_rect(hwnd_obj, id_hoverred, &rect_highlight);

					pt_rect[0] = {
						rect_highlight.left - 4,
						rect_highlight.top - 4
					};
					pt_rect[1] = {
						rect_highlight.right + 4,
						rect_highlight.bottom + 4
					};

					for (int i = 0; i < 2; i++)
						::ScreenToClient(hwnd_obj, &pt_rect[i]);

					RECT rect;
					::GetClientRect(hwnd_obj, &rect);

					rect_highlight = {
						pt_rect[0].x,
						pt_rect[0].y,
						pt_rect[1].x,
						pt_rect[1].y
					};
					::InvalidateRect(hwnd_obj, &rect_highlight, TRUE);
					::UpdateWindow(hwnd_obj);

				}
				// ハイライトするidを更新
				id_hoverred = point_to_id(hwnd_obj, cl_pt);

				// 現在何らかのRECTにホバーしている場合
				if (id_hoverred >= 0) {
					id_to_rect(hwnd_obj, id_hoverred, &rect_highlight);
					Sleep(20);
					highlight_rect(&rect_highlight);
				}
			}
		}

		// ビュー移動・拡大縮小
		if (move_view && !move_pt) {
			// 拡大縮小
			if ((::GetAsyncKeyState(VK_CONTROL) < 0 && move_or_scale == 0) || move_or_scale == 2) {
				double coef_x, coef_y;
				coef_x = MINMAXLIM(std::pow(CE_GR_SCALE_INC, cl_pt.x - pt_view.x),
					CE_GR_SCALE_MIN / prev_scale_x, CE_GR_SCALE_MAX / prev_scale_x);
				coef_y = MINMAXLIM(std::pow(CE_GR_SCALE_INC, pt_view.y - cl_pt.y),
					CE_GR_SCALE_MIN / prev_scale_y, CE_GR_SCALE_MAX / prev_scale_y);
				if (::GetAsyncKeyState(VK_SHIFT) < 0) {
					coef_x = coef_y = LARGER(coef_x, coef_y);
					prev_scale_x = prev_scale_y = LARGER(prev_scale_x, prev_scale_y);
				}
				g_view_info.scale.x = prev_scale_x * coef_x;
				g_view_info.scale.y = prev_scale_y * coef_y;
				g_view_info.origin.x = rect_wnd.right * 0.5f + (float)(coef_x * (prev_o_x - rect_wnd.right * 0.5f));
				g_view_info.origin.y = rect_wnd.bottom * 0.5f + (float)(coef_y * (prev_o_y - rect_wnd.bottom * 0.5f));
				if (move_or_scale == 0)
					move_or_scale = 2;
			}
			// 移動
			else {
				g_view_info.origin.x = prev_o_x + cl_pt.x - pt_view.x;
				g_view_info.origin.y = prev_o_y + cl_pt.y - pt_view.y;
				if (move_or_scale == 0)
					move_or_scale = 1;
			}
			::SetCursor(LoadCursor(NULL, IDC_SIZEALL));
			// 再描画
			::InvalidateRect(hwnd, NULL, FALSE);
		}

		return 0;

		///////////////////////////////////////////
		//		WM_MOUSEWHEEL
		//		(マウスホイールが回転したとき)
		///////////////////////////////////////////
	case WM_MOUSEWHEEL:
	{
		// Ctrlキーが押されているとき(横に移動)
		if (::GetAsyncKeyState(VK_CONTROL) < 0 && GetAsyncKeyState(VK_SHIFT) >= 0)
			g_view_info.origin.x += (float)(GET_Y_LPARAM(wparam) * CE_GR_WHEEL_COEF_POS);

		// Shiftキーが押されているとき(縦に移動)
		else if (::GetAsyncKeyState(VK_SHIFT) < 0 && GetAsyncKeyState(VK_CONTROL) >= 0)
			g_view_info.origin.y += (float)(GET_Y_LPARAM(wparam) * CE_GR_WHEEL_COEF_POS);

		// 縮尺の上限下限を設定
		else {
			double coef = std::pow(CE_GR_SCALE_INC, GET_Y_LPARAM(wparam) * CE_GR_WHEEL_COEF_SCALE);
			double scale_after_x, scale_after_y;

			if (LARGER(g_view_info.scale.x, g_view_info.scale.y) > CE_GR_SCALE_MAX / coef) {
				coef = CE_GR_SCALE_MAX / LARGER(g_view_info.scale.x, g_view_info.scale.y);
			}
			else if (SMALLER(g_view_info.scale.x, g_view_info.scale.y) < CE_GR_SCALE_MIN / coef) {
				coef = CE_GR_SCALE_MIN / SMALLER(g_view_info.scale.x, g_view_info.scale.y);
			}

			scale_after_x = g_view_info.scale.x * coef;
			scale_after_y = g_view_info.scale.y * coef;

			g_view_info.origin.x = (g_view_info.origin.x - rect_wnd.right * 0.5f) * (float)(scale_after_x / g_view_info.scale.x) + rect_wnd.right * 0.5f;
			g_view_info.origin.y = (g_view_info.origin.y - rect_wnd.bottom * 0.5f) * (float)(scale_after_y / g_view_info.scale.y) + rect_wnd.bottom * 0.5f;

			g_view_info.scale.x = scale_after_x;
			g_view_info.scale.y = scale_after_y;
		}
		// 再描画
		::InvalidateRect(hwnd, NULL, FALSE);
		return 0;
	}

		///////////////////////////////////////////
		//		WM_COMMAND
		//		(コマンド)
		///////////////////////////////////////////
	case WM_COMMAND:
		switch (wparam) {
		// 再描画
		case CE_CM_REDRAW:
			::InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		// Valueモードに変更
		case ID_MENU_MODE_VALUE:
			g_config.mode = ce::Config::Value;
			::InvalidateRect(hwnd, NULL, FALSE);
			::SendMessage(g_window_header.hwnd, WM_COMMAND, CE_CT_MODE_VALUE, CE_CM_SELECTED);
			return 0;

		// IDモードに変更
		case ID_MENU_MODE_ID:
			g_config.mode = ce::Config::ID;
			::InvalidateRect(hwnd, NULL, FALSE);
			::SendMessage(g_window_header.hwnd, WM_COMMAND, CE_CT_MODE_ID, CE_CM_SELECTED);
			return 0;

		// グラフをフィット
		case ID_MENU_FIT:
		case CE_CM_FIT:
			g_view_info.fit(&rect_wnd);
			::InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		// ハンドルを整列
		case ID_MENU_ALIGNHANDLE:
		case CE_CT_ALIGN:
			g_config.align_handle = g_config.align_handle ? 0 : 1;
			return 0;

		// カーブを反転
		case ID_MENU_REVERSE:
		case CE_CM_REVERSE:
			// IDモード
			if (g_config.mode == ce::Config::ID)
				g_curve_id[g_config.current_id].reverse_curve();
			// Valueモード
			else {
				POINT pt_tmp = g_curve_value.ctpt[0];
				g_curve_value.ctpt[0].x = CE_GR_RESOLUTION - g_curve_value.ctpt[1].x;
				g_curve_value.ctpt[0].y = CE_GR_RESOLUTION - g_curve_value.ctpt[1].y;
				g_curve_value.ctpt[1].x = CE_GR_RESOLUTION - pt_tmp.x;
				g_curve_value.ctpt[1].y = CE_GR_RESOLUTION - pt_tmp.y;
			}
			::InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		// ハンドルを表示
		case ID_MENU_SHOWHANDLE:
		case CE_CM_SHOWHANDLE:
			g_config.show_handle = g_config.show_handle ? 0 : 1;
			::InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		// カーブを削除
		case CE_CM_CLEAR:
		case ID_MENU_DELETE:
		{
			int response = IDOK;
			if (g_config.alert)
				response = MessageBox(hwnd, CE_STR_DELETE, CE_PLUGIN_NAME, MB_OKCANCEL | MB_ICONEXCLAMATION);
			if (response == IDOK) {
				if (g_config.mode == ce::Config::ID)
					g_curve_id[g_config.current_id].clear();
				else
					g_curve_value.init();

				::InvalidateRect(hwnd, NULL, FALSE);
				// Aviutlを再描画
				::SendMessage(g_fp->hwnd, WM_COMMAND, CE_CM_REDRAW_AVIUTL, 0);
			}
			return 0;
		}

		// カーブのプロパティ
		case ID_MENU_PROPERTY:
		{
			std::string info;
			info = "ID : " + std::to_string(g_config.current_id) + "\n"
				+ "ポイント数 : " + std::to_string(g_curve_id[g_config.current_id].ctpts.size);
			::MessageBox(hwnd, info.c_str(), CE_PLUGIN_NAME, MB_OK | MB_ICONINFORMATION);
			return 0;
		}

		//設定
		case ID_MENU_CONFIG:
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_CONFIG), hwnd, dialogproc_config);
			g_window_main.redraw();
			return 0;

		// 本プラグインについて
		case ID_MENU_ABOUT:
			::MessageBox(hwnd, CE_STR_ABOUT, CE_PLUGIN_NAME, MB_OK);
			return 0;

		// 値をコピー
		case ID_MENU_COPY:
		case CE_CM_COPY:
			if (g_config.mode == ce::Config::Value) {
				TCHAR result_str[12];
				int result = g_curve_value.create_value_1d();
				//文字列へ変換
				::_itoa_s(result, result_str, 12, 10);
				//コピー
				copy_to_clipboard(hwnd, result_str);
			}
			return 0;

		// 値をコピー(4次元)
		case ID_MENU_COPY4D:
			if (g_config.mode == ce::Config::Value) {
				std::string buffer = g_curve_value.create_value_4d();
				LPCTSTR result = buffer.c_str();
				copy_to_clipboard(hwnd, result);
			}
			return 0;

		// 値を読み取り
		case CE_CM_READ:
		case ID_MENU_READ:
			if (g_config.mode == ce::Config::Value) {
				::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_READ), hwnd, dialogproc_read);
				::InvalidateRect(hwnd, NULL, FALSE);
			}
			return 0;

		// 保存ボタン
		case ID_MENU_SAVE:
		case CE_CM_SAVE:
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_SAVE), hwnd, dialogproc_save);
			return 0;

		// ヘルプ
		case ID_MENU_HELP:
			::ShellExecute(0, "open", CE_PLUGIN_LINK_HELP, NULL, NULL, SW_SHOWNORMAL);
			return 0;
		}
		return 0;

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ（ヘッダパネル）
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_header(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;
	static ce::Bitmap_Canvas canvas;
	static ce::Button	copy,
						read,
						save,
						clear,
						fit,
						id_back,
						id_next;
	static ce::Button_Switch mode_value, mode_id;
	static ce::Button_ID id_id;

	RECT	rect_lower_buttons,
			rect_mode_buttons,
			rect_id_buttons,
			rect_copy,
			rect_read,
			rect_save,
			rect_clear,
			rect_fit,
			rect_mode_value,
			rect_mode_id,
			rect_id_back,
			rect_id_id,
			rect_id_next;

	LPRECT mode_buttons[] = {
		&rect_mode_value,
		&rect_mode_id
	};

	LPRECT lower_buttons[] = {
		&rect_copy,
		&rect_read,
		&rect_save,
		&rect_clear,
		&rect_fit
	};

	LPRECT id_buttons[] = {
		&rect_id_back,
		&rect_id_id,
		&rect_id_next
	};

	::GetClientRect(hwnd, &rect_wnd);

	// 下部のボタンが並んだRECT
	rect_lower_buttons = {
		CE_MARGIN,
		CE_MARGIN * 2 + CE_CT_UPPER_H,
		rect_wnd.right - CE_MARGIN,
		CE_MARGIN * 2 + CE_CT_LOWER_H + CE_CT_UPPER_H
	};
	rect_divide(&rect_lower_buttons, lower_buttons, NULL, 5);

	// Value/IDスイッチが並んだRECT
	rect_mode_buttons = {
		CE_MARGIN,
		CE_MARGIN,
		(rect_wnd.right - CE_MARGIN) / 2,
		CE_MARGIN + CE_CT_UPPER_H
	};
	rect_divide(&rect_mode_buttons, mode_buttons, NULL, 2);

	// IDのボタンが並んだRECT
	rect_id_buttons = {
		(rect_wnd.right + CE_MARGIN) / 2,
		CE_MARGIN,
		rect_wnd.right - CE_MARGIN,
		CE_MARGIN + CE_CT_UPPER_H
	};
	rect_divide(&rect_id_buttons, id_buttons, NULL, 3);

	switch (msg) {
	case WM_CLOSE:
		canvas.exit();
		return 0;

	case WM_CREATE:
		canvas.init(hwnd);
		// モード(Value)スイッチ
		mode_value.create(
			hwnd,
			"CTRL_VALUE",
			NULL,
			1,
			NULL, NULL,
			"Value",
			CE_CT_MODE_VALUE,
			&rect_mode_value,
			CE_EDGE_LT | CE_EDGE_LB
		);

		// モード(ID)スイッチ
		mode_id.create(
			hwnd,
			"CTRL_ID",
			NULL,
			1,
			NULL, NULL,
			"ID",
			CE_CT_MODE_ID,
			&rect_mode_id,
			CE_EDGE_RT | CE_EDGE_RB
		);

		// 前のIDのカーブに移動
		id_back.create(
			hwnd,
			"CTRL_ID_BACK",
			"前のIDのカーブに移動",
			0,
			MAKEINTRESOURCE(IDI_BACK),
			MAKEINTRESOURCE(IDI_BACK_LIGHT),
			NULL,
			CE_CM_ID_BACK,
			&rect_id_back,
			CE_EDGE_ALL
		);

		// ID表示ボタン
		id_id.create(
			hwnd,
			"CTRL_ID_ID",
			NULL,
			1,
			NULL, NULL,
			NULL,
			NULL,
			&rect_id_id,
			CE_EDGE_ALL
		);

		// 次のIDのカーブに移動
		id_next.create(
			hwnd,
			"CTRL_ID_NEXT",
			"次のIDのカーブに移動",
			0,
			MAKEINTRESOURCE(IDI_NEXT),
			MAKEINTRESOURCE(IDI_NEXT_LIGHT),
			NULL,
			CE_CM_ID_NEXT,
			&rect_id_next,
			CE_EDGE_ALL
		);

		// コピー
		copy.create(
			hwnd,
			"CTRL_COPY",
			"カーブの値をコピー",
			0,
			MAKEINTRESOURCE(IDI_COPY),
			MAKEINTRESOURCE(IDI_COPY_LIGHT),
			NULL,
			CE_CM_COPY,
			&rect_copy,
			CE_EDGE_ALL
		);

		// 読み取り
		read.create(
			hwnd,
			"CTRL_READ",
			"カーブの値を読み取り",
			0,
			MAKEINTRESOURCE(IDI_READ),
			MAKEINTRESOURCE(IDI_READ_LIGHT),
			NULL,
			CE_CM_READ,
			&rect_read,
			CE_EDGE_ALL
		);

		// 保存
		save.create(
			hwnd,
			"CTRL_SAVE",
			"カーブをプリセットとして保存",
			0,
			MAKEINTRESOURCE(IDI_SAVE),
			MAKEINTRESOURCE(IDI_SAVE_LIGHT),
			NULL,
			CE_CM_SAVE,
			&rect_save,
			CE_EDGE_ALL
		);

		// 初期化
		clear.create(
			hwnd,
			"CTRL_CLEAR",
			"カーブを初期化",
			0,
			MAKEINTRESOURCE(IDI_CLEAR),
			MAKEINTRESOURCE(IDI_CLEAR_LIGHT),
			NULL,
			CE_CM_CLEAR,
			&rect_clear,
			CE_EDGE_ALL
		);

		// フィット
		fit.create(
			hwnd,
			"CTRL_FIT",
			"グラフをフィット",
			0,
			MAKEINTRESOURCE(IDI_FIT),
			MAKEINTRESOURCE(IDI_FIT_LIGHT),
			NULL,
			CE_CM_FIT,
			&rect_fit,
			CE_EDGE_ALL
		);

		if (g_config.mode == ce::Config::ID)
			mode_id.set_status(TRUE);
		else
			mode_value.set_status(TRUE);
		return 0;

	case WM_SIZE:
		mode_value.move(&rect_mode_value);
		mode_id.move(&rect_mode_id);

		id_back.move(&rect_id_back);
		id_id.move(&rect_id_id);
		id_next.move(&rect_id_next);

		copy.move(&rect_copy);
		read.move(&rect_read);
		save.move(&rect_save);
		clear.move(&rect_clear);
		fit.move(&rect_fit);
		return 0;

	case WM_PAINT:
		draw_panel_header(&canvas, &rect_wnd);
		return 0;

	case WM_COMMAND:
		switch (wparam) {
		case CE_CM_REDRAW:
			::InvalidateRect(hwnd, NULL, FALSE);
			mode_value.redraw();
			mode_id.redraw();

			id_back.redraw();
			id_id.redraw();
			id_next.redraw();

			copy.redraw();
			read.redraw();
			save.redraw();
			clear.redraw();
			fit.redraw();
			return 0;

		// モード(Value)スイッチ
		case CE_CT_MODE_VALUE:
			if (lparam == CE_CM_SELECTED) {
				g_config.mode = ce::Config::Value;
				mode_value.set_status(TRUE);
				mode_id.set_status(FALSE);
				::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_REDRAW, 0);
			}
			return 0;

		// モード(ID)スイッチ
		case CE_CT_MODE_ID:
			if (lparam == CE_CM_SELECTED) {
				g_config.mode = ce::Config::ID;
				mode_value.set_status(FALSE);
				mode_id.set_status(TRUE);
				::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_REDRAW, 0);
			}
			return 0;

		// 前のIDのカーブに移動
		case CE_CM_ID_BACK:
			if (g_config.mode == ce::Config::ID) {
				g_config.current_id--;
				g_config.current_id = MINMAXLIM(g_config.current_id, 0, CE_CURVE_MAX - 1);
				g_curve_id_previous = g_curve_id[g_config.current_id];
				id_id.redraw();
				g_window_editor.redraw();
			}
			return 0;

		// 次のIDのカーブに移動
		case CE_CM_ID_NEXT:
			if (g_config.mode == ce::Config::ID) {
				g_config.current_id++;
				g_config.current_id = MINMAXLIM(g_config.current_id, 0, CE_CURVE_MAX - 1);
				g_curve_id_previous = g_curve_id[g_config.current_id];
				id_id.redraw();
				g_window_editor.redraw();
			}
			return 0;

		case CE_CM_CHANGE_ID:
			if (g_config.mode == ce::Config::ID) {
				g_config.current_id = MINMAXLIM(lparam, 0, CE_CURVE_MAX - 1);
				g_curve_id_previous = g_curve_id[g_config.current_id];
				id_id.redraw();
				g_window_editor.redraw();
			}
			return 0;

		case CE_CM_COPY:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_COPY, 0);
			return 0;

		case CE_CM_READ:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_READ, 0);
			return 0;

		case CE_CM_SAVE:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_SAVE, 0);
			return 0;

		case CE_CM_CLEAR:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_CLEAR, 0);
			return 0;

		case CE_CM_FIT:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_FIT, 0);
			return 0;
		}
		return 0;

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ（プリセットパネル）
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_preset(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	POINT			cl_pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
	RECT			rect_wnd;
	static ce::Bitmap_Canvas canvas;

	::GetClientRect(hwnd, &rect_wnd);


	switch (msg) {
	case WM_CLOSE:
		canvas.exit();
		return 0;

	case WM_CREATE:
		canvas.init(hwnd);
		return 0;

	case WM_PAINT:
		draw_panel_preset(&canvas, &rect_wnd);
		return 0;

	case WM_COMMAND:
		switch (wparam) {
		case CE_CM_REDRAW:
			InvalidateRect(hwnd, NULL, FALSE);
			/*SendMessage(buttons.hDefault, WM_COMMAND, CE_CM_REDRAW, 0);
			SendMessage(buttons.hUser, WM_COMMAND, CE_CM_REDRAW, 0);*/
			return 0;
		}
		return 0;

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
}