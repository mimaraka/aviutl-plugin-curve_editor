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
		is_vertical? rect_wnd.right : rect_wnd.right - g_config.separator + CVE_SEPARATOR_WIDTH,
		is_vertical ? rect_wnd.bottom - g_config.separator + CVE_SEPARATOR_WIDTH : rect_wnd.bottom
	);

	rect_header.set(
		0,
		0,
		is_vertical ? rect_wnd.right : rect_wnd.right - g_config.separator - CVE_SEPARATOR_WIDTH,
		CVE_HEADER_H
	);

	rect_editor.set(
		0,
		CVE_HEADER_H,
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

		// ヘッダパネル
		g_window_header.create(
			hwnd,
			"WINDOW_HEADER",
			wndproc_header,
			NULL,
			NULL,
			rect_header.rect,
			NULL
		);

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
		g_window_header.move(rect_header.rect);
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
				g_config.separator = MINMAX_LIMIT(rect_wnd.bottom - pt_client.y, CVE_SEPARATOR_WIDTH, rect_wnd.bottom - CVE_SEPARATOR_WIDTH - CVE_HEADER_H);
			
			else
				g_config.separator = MINMAX_LIMIT(rect_wnd.right - pt_client.x, CVE_SEPARATOR_WIDTH, rect_wnd.right - CVE_SEPARATOR_WIDTH);
			
			::SetActiveWindow(g_window_library.hwnd);
			g_window_header.move(rect_header.rect);
			g_window_editor.move(rect_editor.rect);
			g_window_library.move(rect_library.rect);
			::InvalidateRect(hwnd, NULL, FALSE);
		}
		return 0;

	case WM_COMMAND:
		switch (wparam) {
		case CVE_CM_REDRAW:
			::InvalidateRect(hwnd, NULL, FALSE);
			g_window_header.move(rect_header.rect);
			g_window_editor.move(rect_editor.rect);
			g_window_library.move(rect_library.rect);
			g_window_header.redraw();
			g_window_editor.redraw();
			g_window_library.redraw();
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
	static bool		move_view		= false;
	static bool		is_dragging		= false;
	// 0: NULL, 1: 移動, 2: 拡大縮小
	static int		move_or_scale = NULL;

	//double
	static double	prev_scale_x, prev_scale_y;
	static float	prev_o_x, prev_o_y;

	//POINT
	static POINT	pt_view;

	POINT			pt_client = {
		GET_X_LPARAM(lparam),
		GET_Y_LPARAM(lparam)
	};

	POINT			pt_graph;

	//RECT
	RECT			rect_wnd;

	//ハンドル
	static HMENU	menu;
	static HWND		hwnd_obj;
	static HWND		hwnd_exedit;

	//その他
	static cve::Point_Address		pt_address;
	static cve::Bitmap_Buffer		bitmap_buffer;
	static cve::Obj_Dialog_Buttons	obj_buttons;
	static MENUITEMINFO				minfo;


	//クライアント領域の矩形を取得
	::GetClientRect(hwnd, &rect_wnd);

	//グラフ座標
	if (g_view_info.origin.x != NULL)
		pt_graph = to_graph(pt_client);


	switch (msg) {
	case WM_CLOSE:
		bitmap_buffer.exit();
		return 0;

		///////////////////////////////////////////
		//		WM_CREATE
		//		(ウィンドウが作成されたとき)
		///////////////////////////////////////////
	case WM_CREATE:
	{
		bitmap_buffer.init(hwnd);
		bitmap_buffer.set_size(rect_wnd);

		// メニュー
		menu = ::GetSubMenu(LoadMenu(g_fp->dll_hinst, MAKEINTRESOURCE(IDR_MENU_EDITOR)), 0);

		minfo.cbSize = sizeof(MENUITEMINFO);

		// 拡張編集とオブジェクト設定ダイアログのウィンドウハンドルの取得
		hwnd_exedit = auls::Exedit_GetWindow(g_fp);
		hwnd_obj = auls::ObjDlg_GetWindow(hwnd_exedit);

		obj_buttons.init(hwnd_obj);

		g_curve_normal_previous = g_curve_normal;
		g_curve_mb_previous = g_curve_mb[g_config.current_id];

		g_view_info.fit(rect_wnd);

		return 0;
	}

		///////////////////////////////////////////
		//		WM_SIZE
		//		(ウィンドウのサイズが変更されたとき)
		///////////////////////////////////////////
	case WM_SIZE:
		bitmap_buffer.set_size(rect_wnd);
		return 0;

		///////////////////////////////////////////
		//		WM_PAINT
		//		(ウィンドウが描画されたとき)
		///////////////////////////////////////////
	case WM_PAINT:
		bitmap_buffer.draw_panel_editor();
		return 0;

		///////////////////////////////////////////
		//		WM_LBUTTONDOWN
		//		(左クリックがされたとき)
		///////////////////////////////////////////
	case WM_LBUTTONDOWN:
		// 標準モードのとき
		if (g_config.edit_mode == cve::Mode_Normal) {
			g_curve_normal.pt_in_ctpt(pt_client, &pt_address);

			// 何らかの制御点をクリックしていた場合
			if (pt_address.position != cve::Point_Address::Null) {
				g_curve_normal_previous = g_curve_normal;
				::SetCursor(LoadCursor(NULL, IDC_HAND));
				::SetCapture(hwnd);

				return 0;
			}
		}
		// マルチベジェモードのとき
		else {
			g_curve_mb[g_config.current_id].pt_in_ctpt(pt_client, &pt_address);

			// 何らかの制御点をクリックしていた場合
			if (pt_address.position != cve::Point_Address::Null) {
				// ハンドルの座標を記憶
				if (pt_address.position == cve::Point_Address::Center)
					g_curve_mb[g_config.current_id].move_point(pt_address.index, pt_graph, true);
				else
					g_curve_mb[g_config.current_id].move_handle(pt_address, pt_graph, true);

				g_curve_mb_previous = g_curve_mb[g_config.current_id];
				::SetCursor(LoadCursor(NULL, IDC_HAND));
				::SetCapture(hwnd);

				return 0;
			}
		}

		// カーブのD&D処理
		if (!move_view)
			is_dragging = true;
		
		::SetCapture(hwnd);
		return 0;

		///////////////////////////////////////////
		//		WM_LBUTTONUP
		//		(マウスの左ボタンが離されたとき)
		///////////////////////////////////////////
	case WM_LBUTTONUP:
		// 標準モード
		if (g_config.edit_mode == cve::Mode_Normal) {
			// 移動モード解除
			if (g_config.auto_copy && pt_address.position != cve::Point_Address::Null)
				::SendMessage(hwnd, WM_COMMAND, CVE_CM_COPY, 0);
		}

		// カーブのD&D処理
		if (is_dragging) {
			// Altキーを押していない場合
			if (obj_buttons.is_hovered() && ::GetAsyncKeyState(VK_MENU) >= 0) {
				g_config.is_hooked_popup = TRUE;
				g_config.is_hooked_dialog = TRUE;
				
				obj_buttons.click();
				obj_buttons.invalidate(NULL);
			}
		}

		pt_address.position = cve::Point_Address::Null;
		is_dragging = false;

		// Aviutlを再描画
		::SendMessage(g_fp->hwnd, WM_COMMAND, CVE_CM_REDRAW_AVIUTL, 0);
		::ReleaseCapture();
		return 0;

		///////////////////////////////////////////
		//		WM_MBUTTONDOWN
		//		(マウスの左ボタンがダブルクリックされたとき)
		///////////////////////////////////////////
	case WM_LBUTTONDBLCLK:
		// マルチベジェモードのとき
		if (g_config.edit_mode == cve::Mode_Multibezier) {
			//カーソルが制御点上にあるかどうか
			g_curve_mb[g_config.current_id].pt_in_ctpt(pt_client, &pt_address);

			if (pt_address.position == cve::Point_Address::Center)
				g_curve_mb[g_config.current_id].delete_point(pt_client);
			else if (ISINRANGEEQ(pt_graph.x, 0, CVE_GRAPH_RESOLUTION)) {
				g_curve_mb_previous = g_curve_mb[g_config.current_id];
				g_curve_mb[g_config.current_id].add_point(pt_graph);
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
		cve::apply_config_to_menu(menu, &minfo);
		::ClientToScreen(hwnd, &pt_client);
		::TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN, pt_client.x, pt_client.y, 0, hwnd, NULL);
		return 0;
	

		///////////////////////////////////////////
		//		WM_MBUTTONDOWN
		//		(マウスの中央ボタンが押されたとき)
		///////////////////////////////////////////
	case WM_MBUTTONDOWN:
		if (pt_address.position == cve::Point_Address::Null && !is_dragging) {
			move_view = true;
			pt_view = pt_client;
			prev_scale_x = g_view_info.scale.x;
			prev_scale_y = g_view_info.scale.y;
			prev_o_x = g_view_info.origin.x;
			prev_o_y = g_view_info.origin.y;
			::SetCursor(::LoadCursor(NULL, IDC_SIZEALL));
			::SetCapture(hwnd);
		}
		return 0;

		///////////////////////////////////////////
		//		WM_MBUTTONUP
		//		(マウスの中央ボタンが離されたとき)
		///////////////////////////////////////////
	case WM_MBUTTONUP:
		move_view = false;
		move_or_scale = NULL;

		::ReleaseCapture();
		return 0;

		///////////////////////////////////////////
		//		WM_MOUSEMOVE
		//		(ウィンドウ上でカーソルが動いたとき)
		///////////////////////////////////////////
	case WM_MOUSEMOVE:

		//制御点移動
		if (pt_address.position != cve::Point_Address::Null) {
			//標準モード
			if (g_config.edit_mode == cve::Mode_Normal) {
				g_curve_normal.move_handle(pt_address, pt_graph, false);

				::InvalidateRect(hwnd, NULL, FALSE);
				::PostMessage(g_window_header.hwnd, WM_COMMAND, CVE_CM_VALUE_REDRAW, 0);
			}
			//マルチベジェモード
			else {
				if (pt_address.position == cve::Point_Address::Center)
					g_curve_mb[g_config.current_id].move_point(pt_address.index, pt_graph, false);
				else
					g_curve_mb[g_config.current_id].move_handle(pt_address, pt_graph, false);

				::InvalidateRect(hwnd, NULL, FALSE);
			}
		}

		//制御点ホバー時にカーソルを変更
		if (!is_dragging) {
			cve::Point_Address tmp;

			//標準モード
			if (g_config.edit_mode == cve::Mode_Normal)
				g_curve_normal.pt_in_ctpt(pt_client, &tmp);
			//マルチベジェモード
			else
				g_curve_mb[g_config.current_id].pt_in_ctpt(pt_client, &tmp);

			if (tmp.position != cve::Point_Address::Null)
				::SetCursor(::LoadCursor(NULL, IDC_HAND));


			// ビュー移動・拡大縮小
			if (move_view) {
				// 拡大縮小
				if ((::GetAsyncKeyState(VK_CONTROL) < 0 && move_or_scale == 0) || move_or_scale == 2) {
					double coef_x, coef_y;
					coef_x = MINMAX_LIMIT(std::pow(CVE_GRAPH_SCALE_BASE, pt_client.x - pt_view.x),
						CVE_GRAPH_SCALE_MIN / prev_scale_x, CVE_GRAPH_SCALE_MAX / prev_scale_x);
					coef_y = MINMAX_LIMIT(std::pow(CVE_GRAPH_SCALE_BASE, pt_view.y - pt_client.y),
						CVE_GRAPH_SCALE_MIN / prev_scale_y, CVE_GRAPH_SCALE_MAX / prev_scale_y);
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
					g_view_info.origin.x = prev_o_x + pt_client.x - pt_view.x;
					g_view_info.origin.y = prev_o_y + pt_client.y - pt_view.y;
					if (move_or_scale == 0)
						move_or_scale = 1;
				}
				::SetCursor(::LoadCursor(NULL, IDC_SIZEALL));
				// 再描画
				::InvalidateRect(hwnd, NULL, FALSE);
			}
		}
		// カーブのD&D処理
		else {
			// カーソル
			if (!::PtInRect(&rect_wnd, pt_client))
				::SetCursor(::LoadCursor(g_fp->dll_hinst, MAKEINTRESOURCE(IDC_DRAGGING)));

			// 枠を描画
			POINT sc_pt = pt_client;
			::ClientToScreen(hwnd, &sc_pt);
			cve::Rectangle old_rect;

			// 更新
			int old_id = obj_buttons.update(&sc_pt, &old_rect.rect);
			
			// 先ほどまでハイライトしていたRECTと現在ホバーしているRECTが異なる場合
			if (old_id != obj_buttons.id) {
				// 先ほどまで何らかのRECTをハイライトしていた場合
				if (old_id >= 0)
					obj_buttons.invalidate(&old_rect.rect);

				// 現在何らかのRECTにホバーしている場合
				if (obj_buttons.is_hovered())
					obj_buttons.highlight();
			}
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
			g_view_info.origin.x += (float)(GET_Y_LPARAM(wparam) * CVE_GRAPH_WHEEL_COEF_POS);

		// Shiftキーが押されているとき(縦に移動)
		else if (::GetAsyncKeyState(VK_SHIFT) < 0 && GetAsyncKeyState(VK_CONTROL) >= 0)
			g_view_info.origin.y += (float)(GET_Y_LPARAM(wparam) * CVE_GRAPH_WHEEL_COEF_POS);

		// 縮尺の上限下限を設定
		else {
			double coef = std::pow(CVE_GRAPH_SCALE_BASE, GET_Y_LPARAM(wparam) * CVE_GRAPH_WHEEL_COEF_SCALE);
			double scale_after_x, scale_after_y;

			if (LARGER(g_view_info.scale.x, g_view_info.scale.y) > CVE_GRAPH_SCALE_MAX / coef) {
				coef = CVE_GRAPH_SCALE_MAX / LARGER(g_view_info.scale.x, g_view_info.scale.y);
			}
			else if (SMALLER(g_view_info.scale.x, g_view_info.scale.y) < CVE_GRAPH_SCALE_MIN / coef) {
				coef = CVE_GRAPH_SCALE_MIN / SMALLER(g_view_info.scale.x, g_view_info.scale.y);
			}

			scale_after_x = g_view_info.scale.x * coef;
			scale_after_y = g_view_info.scale.y * coef;

			g_view_info.origin.x =
				(g_view_info.origin.x - rect_wnd.right * 0.5f) *
				(float)(scale_after_x / g_view_info.scale.x) + rect_wnd.right * 0.5f;

			g_view_info.origin.y =
				(g_view_info.origin.y - rect_wnd.bottom * 0.5f) *
				(float)(scale_after_y / g_view_info.scale.y) + rect_wnd.bottom * 0.5f;

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
		case CVE_CM_REDRAW:
			::InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		// 標準モードに変更
		case ID_MENU_MODE_NORMAL:
			g_config.edit_mode = cve::Mode_Normal;
			::InvalidateRect(hwnd, NULL, FALSE);
			::SendMessage(g_window_header.hwnd, WM_COMMAND, CVE_CT_EDIT_MODE_NORMAL, 0);
			return 0;

		// マルチベジェモードに変更
		case ID_MENU_MODE_MULTIBEZIER:
			g_config.edit_mode = cve::Mode_Multibezier;
			::InvalidateRect(hwnd, NULL, FALSE);
			::SendMessage(g_window_header.hwnd, WM_COMMAND, CVE_CT_EDIT_MODE_MULTIBEZIER, 0);
			return 0;

		// グラフをフィット
		case ID_MENU_FIT:
		case CVE_CM_FIT:
			g_view_info.fit(rect_wnd);
			::InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		// ハンドルを整列
		case ID_MENU_ALIGNHANDLE:
		case CVE_CT_ALIGN:
			g_config.align_handle = g_config.align_handle ? 0 : 1;
			return 0;

		// カーブを反転
		case ID_MENU_REVERSE:
		case CVE_CM_REVERSE:
			// 標準モード
			if (g_config.edit_mode == cve::Mode_Normal)
				g_curve_normal.reverse_curve();
			// マルチベジェモード
			else
				g_curve_mb[g_config.current_id].reverse_curve();
			
			::InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		// ハンドルを表示
		case ID_MENU_SHOWHANDLE:
		case CVE_CM_SHOW_HANDLE:
			g_config.show_handle = g_config.show_handle ? 0 : 1;

			::InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		// カーブを削除
		case CVE_CM_CLEAR:
		case ID_MENU_DELETE:
		{
			int response = IDOK;
			if (g_config.alert)
				response = ::MessageBox(
					hwnd,
					CVE_STR_WARNING_DELETE,
					CVE_PLUGIN_NAME,
					MB_OKCANCEL | MB_ICONEXCLAMATION
				);

			if (response == IDOK) {
				if (g_config.edit_mode == cve::Mode_Normal) {
					g_curve_normal.clear();

					::PostMessage(g_window_header.hwnd, WM_COMMAND, CVE_CM_VALUE_REDRAW, 0);
				}
				else	
					g_curve_mb[g_config.current_id].clear();

				::InvalidateRect(hwnd, NULL, FALSE);
				// Aviutlを再描画
				::SendMessage(g_fp->hwnd, WM_COMMAND, CVE_CM_REDRAW_AVIUTL, 0);
			}
			return 0;
		}

		// すべてのカーブを削除
		case ID_MENU_DELETE_ALL:
			if (g_config.edit_mode == cve::Mode_Multibezier) {
				int response = IDOK;
				if (g_config.alert && !lparam)
					response = ::MessageBox(
						hwnd,
						CVE_STR_WARNING_DELETE_ALL,
						CVE_PLUGIN_NAME,
						MB_OKCANCEL | MB_ICONEXCLAMATION
					);

				if (response == IDOK || lparam) {
					for (int i = 0; i < CVE_CURVE_MAX; i++) {
						g_curve_mb[i].clear();
						g_curve_mb[i].set_mode(cve::Mode_Multibezier);
					}

					::InvalidateRect(hwnd, NULL, FALSE);
					// Aviutlを再描画
					::SendMessage(g_fp->hwnd, WM_COMMAND, CVE_CM_REDRAW_AVIUTL, 0);
				}
			}
			return 0;

		// カーブのプロパティ
		case ID_MENU_PROPERTY:
		{
			std::string info;
			info = "ID : " + std::to_string(g_config.current_id) + "\n"
				+ "ポイント数 : " + std::to_string(g_curve_mb[g_config.current_id].ctpts.size);
			::MessageBox(
				hwnd,
				info.c_str(),
				CVE_PLUGIN_NAME,
				MB_OK | MB_ICONINFORMATION
			);

			return 0;
		}

		//設定
		case ID_MENU_CONFIG:
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_CONFIG), hwnd, dialogproc_config);
			g_window_main.redraw();
			return 0;

		// 本プラグインについて
		case ID_MENU_ABOUT:
			::MessageBox(hwnd, CVE_STR_ABOUT, CVE_PLUGIN_NAME, MB_OK);
			return 0;

		// 値をコピー
		case ID_MENU_COPY:
		case CVE_CM_COPY:
			if (g_config.edit_mode == cve::Mode_Normal) {
				TCHAR result_str[12];
				int result = g_curve_normal.create_value_1d();
				//文字列へ変換
				::_itoa_s(result, result_str, 12, 10);
				//コピー
				cve::copy_to_clipboard(hwnd, result_str);
			}
			return 0;

		// 値をコピー(4次元)
		case ID_MENU_COPY4D:
			if (g_config.edit_mode == cve::Mode_Normal) {
				std::string str_result = g_curve_normal.create_parameters();
				LPCTSTR result = str_result.c_str();
				
				cve::copy_to_clipboard(hwnd, result);
			}
			return 0;

		// 値を読み取り
		case CVE_CM_READ:
		case ID_MENU_READ:
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_READ), hwnd, dialogproc_read);
			::InvalidateRect(hwnd, NULL, FALSE);
			
			return 0;

		// 保存ボタン
		case ID_MENU_SAVE:
		case CVE_CM_SAVE:
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_SAVE), hwnd, dialogproc_save);

			return 0;

		// ヘルプ
		case ID_MENU_HELP:
			::ShellExecute(0, "open", CVE_PLUGIN_LINK_HELP, NULL, NULL, SW_SHOWNORMAL);
			return 0;

		// セパレータの位置を初期化
		case ID_MENU_RESET_SEPARATOR:
			g_config.separator = CVE_SEPARATOR_WIDTH;
			::SendMessage(g_window_main.hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);
			return 0;

		// 前のIDに移動
		case ID_MENU_ID_BACK:
			::SendMessage(g_window_header.hwnd, WM_COMMAND, CVE_CM_ID_BACK, 0);
			return 0;

		// 次のIDに移動
		case ID_MENU_ID_NEXT:
			::SendMessage(g_window_header.hwnd, WM_COMMAND, CVE_CM_ID_NEXT, 0);
			return 0;

		case ID_MENU_VERTICAL:
			g_config.layout_mode = cve::Config::Vertical;

			::SendMessage(g_window_main.hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);
			g_view_info.fit(rect_wnd);
			return 0;

		case ID_MENU_HORIZONTAL:
			g_config.layout_mode = cve::Config::Horizontal;

			::SendMessage(g_window_main.hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);
			g_view_info.fit(rect_wnd);
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

	static HMENU menu;
	static MENUITEMINFO minfo;

	static cve::Bitmap_Buffer bitmap_buffer;
	static cve::Button	copy,
						read,
						save,
						clear,
						fit,
						mode,
						id_back,
						id_next;

	static cve::Button_Value value;
	static cve::Button_ID id_id;

	cve::Rectangle		rect_lower_buttons,
						rect_id_buttons;

	RECT				rect_copy,
						rect_read,
						rect_save,
						rect_clear,
						rect_fit,
						rect_mode,
						rect_id_back,
						rect_id_id,
						rect_id_next;

	POINT			pt_client = {
		GET_X_LPARAM(lparam),
		GET_Y_LPARAM(lparam)
	};

	LPCTSTR str_mode[] = {
		CVE_STR_MODE_NORMAL,
		CVE_STR_MODE_MULTIBEZIER,
		CVE_STR_MODE_ELASTIC,
		CVE_STR_MODE_BOUNCE,
		CVE_STR_MODE_VALUE
	};


	::GetClientRect(hwnd, &rect_wnd);

	// 下部のボタンが並んだRECT
	rect_lower_buttons.set(
		CVE_MARGIN,
		CVE_MARGIN + CVE_MARGIN_CONTROL + CVE_CT_UPPER_H,
		rect_wnd.right - CVE_MARGIN,
		CVE_MARGIN + CVE_MARGIN_CONTROL + CVE_CT_LOWER_H + CVE_CT_UPPER_H
	);

	// Value/IDスイッチが並んだRECT
	rect_mode = {
		CVE_MARGIN,
		CVE_MARGIN,
		(rect_wnd.right - CVE_MARGIN) / 2,
		CVE_MARGIN + CVE_CT_UPPER_H
	};

	// IDのボタンが並んだRECT
	rect_id_buttons.set(
		(rect_wnd.right + CVE_MARGIN) / 2,
		CVE_MARGIN,
		rect_wnd.right - CVE_MARGIN,
		CVE_MARGIN + CVE_CT_UPPER_H
	);

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

	rect_lower_buttons.divide(lower_buttons, NULL, 5);
	rect_id_buttons.divide(id_buttons, NULL, 3);

	switch (msg) {
	case WM_CLOSE:
		bitmap_buffer.exit();
		return 0;

	case WM_CREATE:
		bitmap_buffer.init(hwnd);
		bitmap_buffer.set_size(rect_wnd);

		// メニュー
		menu = ::GetSubMenu(LoadMenu(g_fp->dll_hinst, MAKEINTRESOURCE(IDR_MENU_MODE)), 0);
		minfo.cbSize = sizeof(MENUITEMINFO);

		// モード選択ボタン
		mode.initialize(
			hwnd,
			"CTRL_MODE",
			NULL,
			cve::Button::String,
			NULL, NULL,
			str_mode[g_config.edit_mode],
			CVE_CT_EDIT_MODE,
			rect_mode,
			CVE_EDGE_ALL
		);

		// 値
		value.initialize(
			hwnd,
			"CTRL_VALUE",
			NULL,
			cve::Button::String,
			NULL, NULL,
			NULL,
			CVE_CM_VALUE,
			rect_id_buttons.rect,
			CVE_EDGE_ALL
		);

		// 前のIDのカーブに移動
		id_back.initialize(
			hwnd,
			"CTRL_ID_BACK",
			"前のIDのカーブに移動",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_BACK),
			MAKEINTRESOURCE(IDI_BACK_LIGHT),
			NULL,
			CVE_CM_ID_BACK,
			rect_id_back,
			CVE_EDGE_ALL
		);

		// ID表示ボタン
		id_id.initialize(
			hwnd,
			"CTRL_ID_ID",
			NULL,
			cve::Button::String,
			NULL, NULL,
			NULL,
			NULL,
			rect_id_id,
			CVE_EDGE_ALL
		);

		// 次のIDのカーブに移動
		id_next.initialize(
			hwnd,
			"CTRL_ID_NEXT",
			"次のIDのカーブに移動",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_NEXT),
			MAKEINTRESOURCE(IDI_NEXT_LIGHT),
			NULL,
			CVE_CM_ID_NEXT,
			rect_id_next,
			CVE_EDGE_ALL
		);

		// コピー
		copy.initialize(
			hwnd,
			"CTRL_COPY",
			"カーブの値をコピー",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_COPY),
			MAKEINTRESOURCE(IDI_COPY_LIGHT),
			NULL,
			CVE_CM_COPY,
			rect_copy,
			CVE_EDGE_ALL
		);

		// 読み取り
		read.initialize(
			hwnd,
			"CTRL_READ",
			"カーブの値を読み取り",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_READ),
			MAKEINTRESOURCE(IDI_READ_LIGHT),
			NULL,
			CVE_CM_READ,
			rect_read,
			CVE_EDGE_ALL
		);

		// 保存
		save.initialize(
			hwnd,
			"CTRL_SAVE",
			"カーブをプリセットとして保存",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_SAVE),
			MAKEINTRESOURCE(IDI_SAVE_LIGHT),
			NULL,
			CVE_CM_SAVE,
			rect_save,
			CVE_EDGE_ALL
		);

		// 初期化
		clear.initialize(
			hwnd,
			"CTRL_CLEAR",
			"カーブを初期化",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_CLEAR),
			MAKEINTRESOURCE(IDI_CLEAR_LIGHT),
			NULL,
			CVE_CM_CLEAR,
			rect_clear,
			CVE_EDGE_ALL
		);

		// フィット
		fit.initialize(
			hwnd,
			"CTRL_FIT",
			"グラフをフィット",
			cve::Button::Icon,
			MAKEINTRESOURCE(IDI_FIT),
			MAKEINTRESOURCE(IDI_FIT_LIGHT),
			NULL,
			CVE_CM_FIT,
			rect_fit,
			CVE_EDGE_ALL
		);

		if (g_config.edit_mode == cve::Mode_Multibezier) {
			value.hide();
		}
		else {
			id_id.hide();
			id_back.hide();
			id_next.hide();
		}
		return 0;

	case WM_SIZE:
		bitmap_buffer.set_size(rect_wnd);

		mode.move(rect_mode);

		value.move(rect_id_buttons.rect);

		id_back.move(rect_id_back);
		id_id.move(rect_id_id);
		id_next.move(rect_id_next);

		copy.move(rect_copy);
		read.move(rect_read);
		save.move(rect_save);
		clear.move(rect_clear);
		fit.move(rect_fit);
		return 0;

	case WM_PAINT:
		bitmap_buffer.draw_panel();
		return 0;

	case WM_COMMAND:
		switch (wparam) {
		case CVE_CM_REDRAW:
			::InvalidateRect(hwnd, NULL, FALSE);
			mode.redraw();

			value.redraw();

			id_back.redraw();
			id_id.redraw();
			id_next.redraw();

			copy.redraw();
			read.redraw();
			save.redraw();
			clear.redraw();
			fit.redraw();
			return 0;

		// 編集モード選択ボタン
		case CVE_CT_EDIT_MODE:
		{
			minfo.fMask = MIIM_STATE;

			int edit_mode_menu_id[] = {
				ID_MODE_NORMAL,
				ID_MODE_MULTIBEZIER
			};

			// モードのチェック
			for (int i = 0; i < sizeof(edit_mode_menu_id) / sizeof(int); i++) {
				minfo.fState = g_config.edit_mode == i ? MFS_CHECKED : MFS_UNCHECKED;
				SetMenuItemInfo(menu, edit_mode_menu_id[i], FALSE, &minfo);
			}

			::ClientToScreen(hwnd, &pt_client);
			::TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN, pt_client.x, pt_client.y, 0, hwnd, NULL);

			return 0;
		}

		// 標準モード
		case CVE_CT_EDIT_MODE_NORMAL:
		case ID_MODE_NORMAL:
			g_config.edit_mode = cve::Mode_Normal;
			::SendMessage(mode.hwnd, WM_COMMAND, CVE_CM_CHANGE_LABEL, (LPARAM)str_mode[g_config.edit_mode]);
			value.show();
			id_id.hide();
			id_back.hide();
			id_next.hide();
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);

			return 0;

		// マルチベジェモード
		case CVE_CT_EDIT_MODE_MULTIBEZIER:
		case ID_MODE_MULTIBEZIER:
			g_config.edit_mode = cve::Mode_Multibezier;
			::SendMessage(mode.hwnd, WM_COMMAND, CVE_CM_CHANGE_LABEL, (LPARAM)str_mode[g_config.edit_mode]);
			value.hide();
			id_id.show();
			id_back.show();
			id_next.show();
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);

			return 0;

		// 前のIDのカーブに移動
		case CVE_CM_ID_BACK:
			if (g_config.edit_mode == cve::Mode_Multibezier) {
				g_config.current_id--;
				g_config.current_id = MINMAX_LIMIT(g_config.current_id, 0, CVE_CURVE_MAX - 1);
				g_curve_mb_previous = g_curve_mb[g_config.current_id];
				id_id.redraw();
				g_window_editor.redraw();
			}
			return 0;

		// 次のIDのカーブに移動
		case CVE_CM_ID_NEXT:
			if (g_config.edit_mode == cve::Mode_Multibezier) {
				g_config.current_id++;
				g_config.current_id = MINMAX_LIMIT(g_config.current_id, 0, CVE_CURVE_MAX - 1);
				g_curve_mb_previous = g_curve_mb[g_config.current_id];
				id_id.redraw();
				g_window_editor.redraw();
			}
			return 0;

		case CVE_CM_CHANGE_ID:
			if (g_config.edit_mode == cve::Mode_Multibezier) {
				g_config.current_id = MINMAX_LIMIT(lparam, 0, CVE_CURVE_MAX - 1);
				g_curve_mb_previous = g_curve_mb[g_config.current_id];
				id_id.redraw();
				g_window_editor.redraw();
			}
			return 0;

		case CVE_CM_VALUE_REDRAW:
			value.redraw();
			return 0;

		case CVE_CM_VALUE:
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_PARAM), hwnd, dialogproc_param_normal);
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);
			return 0;

		case CVE_CM_COPY:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_COPY, 0);
			return 0;

		case CVE_CM_READ:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_READ, 0);
			return 0;

		case CVE_CM_SAVE:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_SAVE, 0);
			return 0;

		case CVE_CM_CLEAR:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_CLEAR, 0);
			return 0;

		case CVE_CM_FIT:
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_FIT, 0);
			return 0;
		}
		return 0;

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ（ライブラリ）
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_library(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
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

		g_window_preset_list.create(
			hwnd,
			"WINDOW_PRESET_LIST",
			wndproc_preset_list,
			NULL,
			CS_DBLCLKS,
			rect_preset_list.rect,
			NULL
		);

		return 0;

	case WM_SIZE:
		bitmap_buffer.set_size(rect_wnd);
		g_window_preset_list.move(rect_preset_list.rect);
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
		for (int i = 0; i < g_presets_custom.size; i++) {
			g_presets_custom[i].move(rect_wnd.right, i);
		}

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
			for (int i = 0; i < g_presets_custom.size; i++) {
				g_presets_custom[i].move(rect_wnd.right, i);
			}
			return 0;
		}
		return 0;

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
}