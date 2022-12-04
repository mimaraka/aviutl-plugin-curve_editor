//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル(ウィンドウプロシージャ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



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
	static DWORD	thread_id;

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

		if (g_config.notify_latest_version)
			::CreateThread(NULL, 0, cve::check_version, NULL, 0, &thread_id);

		g_curve_normal_previous = g_curve_normal;
		g_curve_mb_previous = g_curve_mb[g_config.current_id.multibezier];
		g_curve_elastic_previous = g_curve_elastic;
		g_curve_bounce_previous = g_curve_bounce;
		g_curve_value_previous = g_curve_value[g_config.current_id.value];

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

		// 編集モードで振り分け
		switch (g_config.edit_mode) {
			// 標準モードのとき
		case cve::Mode_Normal:
			g_curve_normal.pt_in_ctpt(pt_client, &pt_address);

			// 何らかの制御点をクリックしていた場合
			if (pt_address.position != cve::Point_Address::Null) {
				g_curve_normal_previous = g_curve_normal;
				::SetCursor(LoadCursor(NULL, IDC_HAND));
				::SetCapture(hwnd);

				return 0;
			}
			break;

			// マルチベジェモードのとき
		case cve::Mode_Multibezier:
			g_curve_mb[g_config.current_id.multibezier].pt_in_ctpt(pt_client, &pt_address);

			// 何らかの制御点をクリックしていた場合
			if (pt_address.position != cve::Point_Address::Null) {
				// ハンドルの座標を記憶
				if (pt_address.position == cve::Point_Address::Center)
					g_curve_mb[g_config.current_id.multibezier].move_point(pt_address.index, pt_graph, true);
				else
					g_curve_mb[g_config.current_id.multibezier].move_handle(pt_address, pt_graph, true);

				g_curve_mb_previous = g_curve_mb[g_config.current_id.multibezier];
				::SetCursor(LoadCursor(NULL, IDC_HAND));
				::SetCapture(hwnd);

				return 0;
			}
			break;

			// 振動モードのとき
		case cve::Mode_Elastic:
			g_curve_elastic.pt_in_ctpt(pt_client, &pt_address);

			if (pt_address.position != cve::Point_Address::Null) {
				g_curve_elastic_previous = g_curve_elastic;

				::SetCursor(LoadCursor(NULL, IDC_HAND));
				::SetCapture(hwnd);

				return 0;
			}
			break;

			// 弾性モードのとき
		case cve::Mode_Bounce:
			break;

			// 数値指定モードのとき
		case cve::Mode_Value:
			break;
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
		// オートコピー
		if (g_config.edit_mode == cve::Mode_Normal) {
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
		// モード振り分け
		switch (g_config.edit_mode) {
			// マルチベジェモードのとき
		case cve::Mode_Multibezier:
			//カーソルが制御点上にあるかどうか
			g_curve_mb[g_config.current_id.multibezier].pt_in_ctpt(pt_client, &pt_address);

			if (pt_address.position == cve::Point_Address::Center)
				g_curve_mb[g_config.current_id.multibezier].delete_point(pt_client);
			else if (ISINRANGEEQ(pt_graph.x, 0, CVE_GRAPH_RESOLUTION)) {
				g_curve_mb_previous = g_curve_mb[g_config.current_id.multibezier];
				g_curve_mb[g_config.current_id.multibezier].add_point(pt_graph);
			}

			::InvalidateRect(hwnd, NULL, FALSE);
			break;

			// 数値指定モードのとき
		case cve::Mode_Value:
			//カーソルが制御点上にあるかどうか
			g_curve_value[g_config.current_id.value].pt_in_ctpt(pt_client, &pt_address);

			if (pt_address.position == cve::Point_Address::Center)
				g_curve_value[g_config.current_id.value].delete_point(pt_client);
			else if (ISINRANGEEQ(pt_graph.x, 0, CVE_GRAPH_RESOLUTION)) {
				g_curve_value_previous = g_curve_value[g_config.current_id.value];
				g_curve_value[g_config.current_id.value].add_point(pt_graph);
			}

			::InvalidateRect(hwnd, NULL, FALSE);
			break;
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
			// モード振り分け
			switch (g_config.edit_mode) {
				// 標準モードのとき
			case cve::Mode_Normal:
				g_curve_normal.move_handle(pt_address, pt_graph, false);

				::InvalidateRect(hwnd, NULL, FALSE);
				::PostMessage(g_window_header.hwnd, WM_COMMAND, CVE_CM_PARAM_REDRAW, 0);
				break;

				// マルチベジェモードのとき
			case cve::Mode_Multibezier:
				if (pt_address.position == cve::Point_Address::Center)
					g_curve_mb[g_config.current_id.multibezier].move_point(pt_address.index, pt_graph, false);
				else
					g_curve_mb[g_config.current_id.multibezier].move_handle(pt_address, pt_graph, false);

				::InvalidateRect(hwnd, NULL, FALSE);
				break;

				// 振動モードのとき
			case cve::Mode_Elastic:
				g_curve_elastic.move_handle(pt_graph.y);

				::InvalidateRect(hwnd, NULL, FALSE);
				::PostMessage(g_window_header.hwnd, WM_COMMAND, CVE_CM_PARAM_REDRAW, 0);
				break;

				// 弾性モードのとき
			case cve::Mode_Bounce:

				break;

				// 数値指定モードのとき
			case cve::Mode_Value:

				break;
			}
		}

		//制御点ホバー時にカーソルを変更
		if (!is_dragging) {
			cve::Point_Address tmp;

			// モード振り分け
			switch (g_config.edit_mode) {
				// 標準モードのとき
			case cve::Mode_Normal:
				g_curve_normal.pt_in_ctpt(pt_client, &tmp);
				break;

				// マルチベジェモードのとき
			case cve::Mode_Multibezier:
				g_curve_mb[g_config.current_id.multibezier].pt_in_ctpt(pt_client, &tmp);
				break;

				// 振動モードのとき
			case cve::Mode_Elastic:
				g_curve_elastic.pt_in_ctpt(pt_client, &tmp);
				break;

				// 弾性モードのとき
			case cve::Mode_Bounce:
				//g_curve_bounce.pt_in_ctpt(pt_client, &tmp);
				break;

				// 数値指定モードのとき
			case cve::Mode_Value:

				break;
			}

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
				if (obj_buttons.is_hovered() && ::GetAsyncKeyState(VK_MENU) >= 0) {
					if (::GetAsyncKeyState(VK_CONTROL) < 0)
						obj_buttons.highlight(1);
					else
						obj_buttons.highlight(0);
				}
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
	{
		const int edit_mode_menu_id[] = {
			ID_MENU_MODE_NORMAL,
			ID_MENU_MODE_MULTIBEZIER,
			ID_MENU_MODE_ELASTIC,
			ID_MENU_MODE_BOUNCE,
			ID_MENU_MODE_VALUE
		};

		// 編集モードを変更
		for (const int& el : edit_mode_menu_id) {
			if (wparam == el) {
				g_config.edit_mode = (cve::Edit_Mode)(wparam - ID_MENU_MODE_NORMAL);
				::InvalidateRect(hwnd, NULL, FALSE);
				::SendMessage(g_window_header.hwnd, WM_COMMAND, CVE_CT_EDIT_MODE_NORMAL + wparam - ID_MENU_MODE_NORMAL, 0);
				return 0;
			}
		}
		

		switch (wparam) {
			// 再描画
		case CVE_CM_REDRAW:
			::InvalidateRect(hwnd, NULL, FALSE);
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
				g_curve_mb[g_config.current_id.multibezier].reverse_curve();

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

					::PostMessage(g_window_header.hwnd, WM_COMMAND, CVE_CM_PARAM_REDRAW, 0);
				}
				else
					g_curve_mb[g_config.current_id.multibezier].clear();

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
			info = "ID : " + std::to_string(g_config.current_id.multibezier) + "\n"
				+ "ポイント数 : " + std::to_string(g_curve_mb[g_config.current_id.multibezier].ctpts.size);
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
				int result = g_curve_normal.create_number();
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

			// レイアウトを縦向きにする
		case ID_MENU_VERTICAL:
			g_config.layout_mode = cve::Config::Vertical;

			::SendMessage(g_window_main.hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_FIT, 0);
			return 0;

			// レイアウトを横向きにする
		case ID_MENU_HORIZONTAL:
			g_config.layout_mode = cve::Config::Horizontal;

			::SendMessage(g_window_main.hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_FIT, 0);
			return 0;

		case ID_MENU_LATEST_VERSION:
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_LATEST_VERSION), hwnd, dialogproc_latest_version);
			return 0;
		}

		return 0;
	}

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}