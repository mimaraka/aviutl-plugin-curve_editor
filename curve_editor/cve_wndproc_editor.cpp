//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル(ウィンドウプロシージャ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include <yulib/extra.h>
#include "cve_header.hpp"



// ビュー変更用クラス
class Change_View {
	static constexpr double GRAPH_SCALE_BASE = 1.01;
public:
	static constexpr int MOVING = 1;
	static constexpr int SCALING = 2;
	int mode;
	aului::Point<double> scale_prev;
	aului::Point<double> origin_prev;
	aului::Point<LONG> pt_client_prev;

	Change_View() :
		mode(NULL)
	{}

	bool start(const POINT& pt_client)
	{
		if (!mode) {
			if (::GetAsyncKeyState(VK_CONTROL) < 0)
				mode = SCALING;
			else
				mode = MOVING;

			pt_client_prev = aului::Point<LONG>(pt_client);
			scale_prev = g_view_info.scale;
			origin_prev = g_view_info.origin;

			return true;
		}
		else return false;
	}

	bool move(const POINT& pt_client, const RECT& rect_wnd)
	{
		// スケーリング
		if (mode == SCALING) {
			double coef_x, coef_y;
			coef_x = std::clamp(std::pow(GRAPH_SCALE_BASE, pt_client.x - pt_client_prev.x),
				cve::Graph_View_Info::SCALE_MIN / scale_prev.x, cve::Graph_View_Info::SCALE_MAX / scale_prev.x);
			coef_y = std::clamp(std::pow(GRAPH_SCALE_BASE, pt_client_prev.y - pt_client.y),
				cve::Graph_View_Info::SCALE_MIN / scale_prev.y, cve::Graph_View_Info::SCALE_MAX / scale_prev.y);
			if (::GetAsyncKeyState(VK_SHIFT) < 0) {
				coef_x = coef_y = std::max(coef_x, coef_y);
				scale_prev.x = scale_prev.y = std::max(scale_prev.x, scale_prev.y);
			}
			g_view_info.scale.x = scale_prev.x * coef_x;
			g_view_info.scale.y = scale_prev.y * coef_y;
			g_view_info.origin.x = rect_wnd.right * 0.5f + (float)(coef_x * (origin_prev.x - rect_wnd.right * 0.5f));
			g_view_info.origin.y = rect_wnd.bottom * 0.5f + (float)(coef_y * (origin_prev.y - rect_wnd.bottom * 0.5f));

			return true;
		}
		// 移動
		else if (mode == MOVING) {
			g_view_info.origin = origin_prev + pt_client - pt_client_prev;

			return true;
		}
		else return false;
	}

	bool end()
	{
		if (mode) {
			mode = NULL;
			return true;
		}
		else return false;
	}

	bool is_moving()
	{
		return mode;
	}
};



//---------------------------------------------------------------------
//		ウィンドウプロシージャ（エディタパネル）
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_editor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// constexpr
	constexpr double GRAPH_WHEEL_COEF_SCALE = 0.1;
	constexpr double GRAPH_WHEEL_COEF_POS = 0.2;
	constexpr double GRAPH_SCALE_BASE = 1.01;

	//int
	static bool		is_dragging		= false;

	POINT			pt_client = {
		GET_X_LPARAM(lparam),
		GET_Y_LPARAM(lparam)
	};

	POINT			pt_graph = { 0 };

	//RECT
	RECT			rect_wnd;

	//ハンドル
	static HMENU	menu;
	static HWND		hwnd_obj;
	static HWND		hwnd_exedit;
	static DWORD	thread_id;

	//その他
	static cve::Point_Address				pt_address;
	static cve::My_Direct2d_Paint_Object	paint_object;
	static cve::Obj_Dialog_Buttons			obj_buttons;
	static MENUITEMINFO						minfo;
	static Change_View						change_view;


	//クライアント領域の矩形を取得
	::GetClientRect(hwnd, &rect_wnd);

	//グラフ座標
	if (g_view_info.origin.x != NULL)
		pt_graph = to_graph(aului::Point<LONG>(pt_client)).to_win32_point();


	switch (msg) {
		///////////////////////////////////////////
		//		WM_CREATE
		//		(ウィンドウが作成されたとき)
		///////////////////////////////////////////
	case WM_CREATE:
	{
		paint_object.init(hwnd, g_p_factory, g_p_write_factory);

		// メニュー
		menu = ::GetSubMenu(LoadMenu(g_fp->dll_hinst, MAKEINTRESOURCE(IDR_MENU_EDITOR)), 0);

		minfo.cbSize = sizeof(MENUITEMINFO);

		if (g_config.notify_update)
			::CreateThread(NULL, 0, cve::check_version, NULL, 0, &thread_id);

		g_curve_bezier_trace = g_curve_bezier;
		g_curve_bezier_multi_trace = g_curve_bezier_multi[g_config.current_id.multi - 1];
		g_curve_elastic_trace = g_curve_elastic;
		g_curve_bounce_trace = g_curve_bounce;
		g_curve_bezier_value_trace = g_curve_bezier_value[g_config.current_id.value - 1];

		g_view_info.fit(rect_wnd);

		return 0;
	}

	case WM_CLOSE:
		paint_object.exit();
		return 0;

		///////////////////////////////////////////
		//		WM_SIZE
		//		(ウィンドウのサイズが変更されたとき)
		///////////////////////////////////////////
	case WM_SIZE:
		paint_object.resize();
		return 0;

		///////////////////////////////////////////
		//		WM_PAINT
		//		(ウィンドウが描画されたとき)
		///////////////////////////////////////////
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		::BeginPaint(hwnd, &ps);
		paint_object.draw_panel_editor();
		::EndPaint(hwnd, &ps);
		return 0;
	}

		///////////////////////////////////////////
		//		WM_LBUTTONDOWN
		//		(左クリックがされたとき)
		///////////////////////////////////////////
	case WM_LBUTTONDOWN:
	{
		// 編集モードで振り分け
		switch (g_config.edit_mode) {
			// ベジェモードのとき
		case cve::Mode_Bezier:
			g_curve_bezier.pt_on_ctpt(pt_client, &pt_address);

			// 何らかの制御点をクリックしていた場合
			if (pt_address.position != cve::Point_Address::Null) {
				cve::trace_curve();
				::InvalidateRect(hwnd, NULL, FALSE);
				::SetCursor(LoadCursor(NULL, IDC_HAND));
				::SetCapture(hwnd);

				return 0;
			}
			break;

			// ベジェ(複数)モードのとき
		case cve::Mode_Bezier_Multi:
		{
			bool prioritize_point = ::GetAsyncKeyState(VK_CONTROL) < 0;
			g_curve_bezier_multi[g_config.current_id.multi - 1].pt_on_ctpt(pt_client, &pt_address, prioritize_point);

			// 何らかの制御点をクリックしていた場合
			if (pt_address.position != cve::Point_Address::Null) {
				// ハンドルの座標を記憶
				if (pt_address.position == cve::Point_Address::Center)
					g_curve_bezier_multi[g_config.current_id.multi - 1].move_point(pt_address.index, pt_graph, true);
				else
					g_curve_bezier_multi[g_config.current_id.multi - 1].move_handle(pt_address, pt_graph, true);

				cve::trace_curve();
				::InvalidateRect(hwnd, NULL, FALSE);
				::SetCursor(LoadCursor(NULL, IDC_HAND));
				::SetCapture(hwnd);

				return 0;
			}
			break;
		}

			// 値指定モードのとき
		case cve::Mode_Bezier_Value:
			break;

			// 振動モードのとき
		case cve::Mode_Elastic:
			g_curve_elastic.pt_on_ctpt(pt_client, &pt_address);

			if (pt_address.position != cve::Point_Address::Null) {
				cve::trace_curve();
				::InvalidateRect(hwnd, NULL, FALSE);
				::SetCursor(LoadCursor(NULL, IDC_HAND));
				::SetCapture(hwnd);

				return 0;
			}
			break;

			// バウンスモードのとき
		case cve::Mode_Bounce:
			g_curve_bounce.pt_on_ctpt(pt_client, &pt_address);

			if (pt_address.position != cve::Point_Address::Null) {
				cve::trace_curve();
				::InvalidateRect(hwnd, NULL, FALSE);
				::SetCursor(LoadCursor(NULL, IDC_HAND));
				::SetCapture(hwnd);

				return 0;
			}
			break;
		}

		// ビュー移動中でなく、どのポイントにもホバーしていないとき
		if (!change_view.is_moving() && pt_address.position == cve::Point_Address::Null) {
			// ビュー移動・拡大縮小準備
			// Altキーが押されているとき
			if (::GetAsyncKeyState(VK_MENU) < 0) {
				change_view.start(pt_client);
				::SetCursor(::LoadCursor(NULL, IDC_SIZEALL));
			}
			// カーブのD&D処理
			else
				is_dragging = true;
		}

		::SetCapture(hwnd);
		return 0;
	}
		

		///////////////////////////////////////////
		//		WM_LBUTTONUP
		//		(マウスの左ボタンが離されたとき)
		///////////////////////////////////////////
	case WM_LBUTTONUP:
		// オートコピー
		if (g_config.edit_mode == cve::Mode_Bezier || g_config.edit_mode == cve::Mode_Elastic || g_config.edit_mode == cve::Mode_Bounce) {
			if (g_config.auto_copy && pt_address.position != cve::Point_Address::Null)
				::SendMessage(hwnd, WM_COMMAND, CVE_CM_COPY, 0);
		}

		// カーブのD&D処理
		if (is_dragging) {
			// Altキーを押していない場合
			if (obj_buttons.is_hovered() && ::GetAsyncKeyState(VK_MENU) >= 0) {
				g_config.hooked_popup = TRUE;
				g_config.hooked_dialog = TRUE;
				
				obj_buttons.click();
				obj_buttons.invalidate(NULL);
			}
		}

		change_view.end();

		pt_address.position = cve::Point_Address::Null;
		is_dragging = false;

		// IDタイプのモードのとき
		if (g_config.edit_mode == cve::Mode_Bezier_Multi || g_config.edit_mode == cve::Mode_Bezier_Value) {

			// 自動適用(AviUtlを再描画)
			if (g_config.auto_apply)
				::SendMessage(g_fp->hwnd, WM_COMMAND, CVE_CM_REDRAW_AVIUTL, 0);
		}

		// マウスキャプチャを開放
		::ReleaseCapture();

		return 0;

		///////////////////////////////////////////
		//		WM_MBUTTONDOWN
		//		(マウスの左ボタンがダブルクリックされたとき)
		///////////////////////////////////////////
	case WM_LBUTTONDBLCLK:
		// モード振り分け
		switch (g_config.edit_mode) {
			// ベジェ(複数)モードのとき
		case cve::Mode_Bezier_Multi:
			//カーソルが制御点・ハンドル上にあるかどうか
			g_curve_bezier_multi[g_config.current_id.multi - 1].pt_on_ctpt(pt_client, &pt_address, true);

			if (pt_address.position == cve::Point_Address::Center) {
				g_curve_bezier_multi[g_config.current_id.multi - 1].delete_point(pt_client);
				pt_address.position = cve::Point_Address::Null;
			}
			else if (pt_address.position == cve::Point_Address::Left && g_config.linearize) {
				g_curve_bezier_multi[g_config.current_id.multi - 1].ctpts[pt_address.index].pt_left = g_curve_bezier_multi[g_config.current_id.multi - 1].ctpts[pt_address.index].pt_center;
				g_curve_bezier_multi[g_config.current_id.multi - 1].ctpts[pt_address.index - 1].pt_right = g_curve_bezier_multi[g_config.current_id.multi - 1].ctpts[pt_address.index - 1].pt_center;
				pt_address.position = cve::Point_Address::Null;
			}
			else if (pt_address.position == cve::Point_Address::Right && g_config.linearize) {
				g_curve_bezier_multi[g_config.current_id.multi - 1].ctpts[pt_address.index].pt_right = g_curve_bezier_multi[g_config.current_id.multi - 1].ctpts[pt_address.index].pt_center;
				g_curve_bezier_multi[g_config.current_id.multi - 1].ctpts[pt_address.index + 1].pt_left = g_curve_bezier_multi[g_config.current_id.multi - 1].ctpts[pt_address.index + 1].pt_center;
				pt_address.position = cve::Point_Address::Null;
			}
			else if (aului::in_range((int)pt_graph.x, 0, CVE_GRAPH_RESOLUTION, true)) {
				cve::trace_curve();
				g_curve_bezier_multi[g_config.current_id.multi - 1].add_point(pt_graph);
				// ダブルクリックしたまま制御点の移動を行えるようにするため
				::SendMessage(hwnd, WM_LBUTTONDOWN, wparam, lparam);
			}

			::InvalidateRect(hwnd, NULL, FALSE);
			break;

			// 値指定モードのとき
		case cve::Mode_Bezier_Value:
			//カーソルが制御点・ハンドル上にあるかどうか
			g_curve_bezier_value[g_config.current_id.value - 1].pt_on_ctpt(pt_client, &pt_address);

			if (pt_address.position == cve::Point_Address::Center)
				g_curve_bezier_value[g_config.current_id.value - 1].delete_point(pt_client);
			else if (aului::in_range((int)pt_graph.x, 0, CVE_GRAPH_RESOLUTION, true)) {
				cve::trace_curve();
				g_curve_bezier_value[g_config.current_id.value - 1].add_point(pt_graph);
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
		if (pt_address.position == cve::Point_Address::Null && !is_dragging && !change_view.is_moving()) {
			change_view.start(pt_client);
			::SetCursor(::LoadCursor(NULL, IDC_SIZEALL));
			::SetCapture(hwnd);
		}
		return 0;

		///////////////////////////////////////////
		//		WM_MBUTTONUP
		//		(マウスの中央ボタンが離されたとき)
		///////////////////////////////////////////
	case WM_MBUTTONUP:
		change_view.end();
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
				// ベジェモードのとき
			case cve::Mode_Bezier:
				g_curve_bezier.move_handle(pt_address, pt_graph, false);

				::InvalidateRect(hwnd, NULL, FALSE);
				::PostMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_PARAM_REDRAW, 0);
				break;

				// ベジェ(複数)モードのとき
			case cve::Mode_Bezier_Multi:
				if (pt_address.position == cve::Point_Address::Center)
					g_curve_bezier_multi[g_config.current_id.multi - 1].move_point(pt_address.index, pt_graph, false);
				else
					g_curve_bezier_multi[g_config.current_id.multi - 1].move_handle(pt_address, pt_graph, false);

				::InvalidateRect(hwnd, NULL, FALSE);
				break;

				// 値指定モードのとき
			case cve::Mode_Bezier_Value:

				break;

				// 振動モードのとき
			case cve::Mode_Elastic:
				g_curve_elastic.move_handle(pt_address, pt_graph);

				::InvalidateRect(hwnd, NULL, FALSE);
				::PostMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_PARAM_REDRAW, 0);
				break;

				// バウンスモードのとき
			case cve::Mode_Bounce:
				g_curve_bounce.move_handle(pt_graph);

				::InvalidateRect(hwnd, NULL, FALSE);
				::PostMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_PARAM_REDRAW, 0);
				break;
			}
		}

		// 制御点ホバー時にカーソルを変更
		// ドラッグ中でないとき
		if (!is_dragging) {
			cve::Point_Address tmp = { 0 };

			// モード振り分け
			switch (g_config.edit_mode) {
				// ベジェモードのとき
			case cve::Mode_Bezier:
				g_curve_bezier.pt_on_ctpt(pt_client, &tmp);
				break;

				// ベジェ(複数)モードのとき
			case cve::Mode_Bezier_Multi:
				g_curve_bezier_multi[g_config.current_id.multi - 1].pt_on_ctpt(pt_client, &tmp);
				break;

				// 値指定モードのとき
			/*case cve::Mode_Bezier_Value:

				break;*/

				// 振動モードのとき
			case cve::Mode_Elastic:
				g_curve_elastic.pt_on_ctpt(pt_client, &tmp);
				break;

				// バウンスモードのとき
			case cve::Mode_Bounce:
				g_curve_bounce.pt_on_ctpt(pt_client, &tmp);
				break;

			default:
				return 0;
			}

			if (tmp.position != cve::Point_Address::Null)
				::SetCursor(::LoadCursor(NULL, IDC_HAND));

			if (change_view.is_moving()) {
				change_view.move(pt_client, rect_wnd);
				::SetCursor(::LoadCursor(NULL, IDC_SIZEALL));
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
			aului::Window_Rectangle old_rect;

			// 更新
			int old_id = obj_buttons.update(&sc_pt, &old_rect.rect);
			
			// 先ほどまでハイライトしていたRECTと現在ホバーしているRECTが異なる場合
			if (old_id != obj_buttons.id) {
				// 先ほどまで何らかのRECTをハイライトしていた場合
				if (old_id >= 0)
					obj_buttons.invalidate(&old_rect.rect);

				// 現在何らかのRECTにホバーしている場合
				if (obj_buttons.is_hovered() && ::GetAsyncKeyState(VK_MENU) >= 0)
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
			g_view_info.origin.x += (float)(GET_Y_LPARAM(wparam) * GRAPH_WHEEL_COEF_POS);

		// Shiftキーが押されているとき(縦に移動)
		else if (::GetAsyncKeyState(VK_SHIFT) < 0 && GetAsyncKeyState(VK_CONTROL) >= 0)
			g_view_info.origin.y += (float)(GET_Y_LPARAM(wparam) * GRAPH_WHEEL_COEF_POS);

		// 縮尺の上限下限を設定
		else {
			double coef = std::pow(GRAPH_SCALE_BASE, GET_Y_LPARAM(wparam) * GRAPH_WHEEL_COEF_SCALE);
			double scale_after_x, scale_after_y;

			if (std::max(g_view_info.scale.x, g_view_info.scale.y) > cve::Graph_View_Info::SCALE_MAX / coef) {
				coef = cve::Graph_View_Info::SCALE_MAX / std::max(g_view_info.scale.x, g_view_info.scale.y);
			}
			else if (std::min(g_view_info.scale.x, g_view_info.scale.y) < cve::Graph_View_Info::SCALE_MIN / coef) {
				coef = cve::Graph_View_Info::SCALE_MIN / std::min(g_view_info.scale.x, g_view_info.scale.y);
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
		//		WM_REDRAW
		//		(再描画が指示されたとき)
		///////////////////////////////////////////
	case aului::Window::WM_REDRAW:
		::InvalidateRect(hwnd, NULL, FALSE);
		return 0;

		///////////////////////////////////////////
		//		WM_COMMAND
		//		(コマンド)
		///////////////////////////////////////////
	case WM_COMMAND:
	{
		const int edit_mode_menu_id[] = {
			ID_MENU_MODE_BEZIER,
			ID_MENU_MODE_MULTIBEZIER,
			ID_MENU_MODE_VALUE,
			ID_MENU_MODE_ELASTIC,
			ID_MENU_MODE_BOUNCE,
		};

		// 編集モードを変更
		for (int i = 0; i < sizeof(edit_mode_menu_id) / sizeof(int); i++) {
			if (wparam == edit_mode_menu_id[i]) {
				g_config.edit_mode = (cve::Edit_Mode)i;
				::InvalidateRect(hwnd, NULL, FALSE);
				::SendMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CT_EDIT_MODE_BEZIER + i, 0);
				return 0;
			}
		}
		

		switch (wparam) {
			// 拡張編集のウィンドウハンドルを取得
		case CVE_CM_GET_EXEDIT:
		{
			// フックの準備
			g_config.hooked_popup = false;
			g_config.hooked_dialog = false;

			char exedit_path[MAX_PATH];
			FILTER* fp_exedit = auls::Exedit_GetFilter(g_fp);

			// 拡張編集プラグインが存在する場合
			if (fp_exedit) {
				::GetModuleFileName(fp_exedit->dll_hinst, exedit_path, sizeof(exedit_path));

				// TrackPopupMenu()をフック
				TrackPopupMenu_original = (decltype(TrackPopupMenu_original))yulib::RewriteFunction(
					exedit_path,
					"TrackPopupMenu",
					TrackPopupMenu_hooked
				);

				// DialogBoxParamA()をフック
				DialogBox_original = (decltype(DialogBox_original))yulib::RewriteFunction(
					exedit_path,
					"DialogBoxParamA",
					DialogBox_hooked
				);
			}
			// 存在しない場合
			else
				::MessageBox(g_fp->hwnd, CVE_STR_ERROR_EXEDIT_NOT_FOUND, CVE_FILTER_NAME, MB_OK);

			// 拡張編集とオブジェクト設定ダイアログのウィンドウハンドルの取得
			hwnd_exedit = auls::Exedit_GetWindow(g_fp);
			hwnd_obj = auls::ObjDlg_GetWindow(hwnd_exedit);

			obj_buttons.init(hwnd_obj);

			return 0;
		}

			// グラフをフィット
		case ID_MENU_FIT:
		case CVE_CM_FIT:
			g_view_info.fit(rect_wnd);
			::InvalidateRect(hwnd, NULL, FALSE);
			return 0;

			// ハンドルを整列
		case ID_MENU_ALIGNHANDLE:
		case CVE_CT_ALIGN:
			if (g_config.edit_mode == cve::Mode_Bezier_Multi || g_config.edit_mode == cve::Mode_Bezier_Value)
				g_config.align_handle = g_config.align_handle ? 0 : 1;
			return 0;

			// カーブを反転
		case ID_MENU_REVERSE:
		case CVE_CM_REVERSE:
			cve::trace_curve();

			switch (g_config.edit_mode) {
			case cve::Mode_Bezier:
				g_curve_bezier.reverse_curve();
				break;

			case cve::Mode_Bezier_Multi:
				g_curve_bezier_multi[g_config.current_id.multi - 1].reverse_curve();
				break;

			case cve::Mode_Elastic:
				g_curve_elastic.reverse_curve();
				break;

			case cve::Mode_Bounce:
				g_curve_bounce.reverse_curve();
				break;
			}

			::InvalidateRect(hwnd, NULL, FALSE);
			::PostMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_PARAM_REDRAW, 0);
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
			if (g_config.show_popup)
				response = ::MessageBox(
					hwnd,
					CVE_STR_WARNING_DELETE,
					CVE_FILTER_NAME,
					MB_OKCANCEL | MB_ICONEXCLAMATION
				);

			if (response == IDOK) {
				cve::trace_curve();

				switch (g_config.edit_mode) {
				case cve::Mode_Bezier:
					g_curve_bezier.clear();
					::PostMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_PARAM_REDRAW, 0);
					break;

				case cve::Mode_Bezier_Multi:
					g_curve_bezier_multi[g_config.current_id.multi - 1].clear();
					break;

				case cve::Mode_Bezier_Value:
					break;

				case cve::Mode_Elastic:
					g_curve_elastic.init();
					::PostMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_PARAM_REDRAW, 0);
					break;

				case cve::Mode_Bounce:
					g_curve_bounce.init();
					::PostMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_PARAM_REDRAW, 0);
					break;
				}

				::InvalidateRect(hwnd, NULL, FALSE);
				// Aviutlを再描画
				::SendMessage(g_fp->hwnd, WM_COMMAND, CVE_CM_REDRAW_AVIUTL, 0);
			}
			return 0;
		}

		// すべてのカーブを削除
		case ID_MENU_DELETE_ALL:
			if (g_config.edit_mode == cve::Mode_Bezier_Multi) {
				int response = IDOK;
				if (g_config.show_popup && !lparam)
					response = ::MessageBox(
						hwnd,
						CVE_STR_WARNING_DELETE_ALL,
						CVE_FILTER_NAME,
						MB_OKCANCEL | MB_ICONEXCLAMATION
					);

				if (response == IDOK || lparam) {
					for (int i = 0; i < CVE_CURVE_MAX; i++) {
						g_curve_bezier_multi[i].clear();
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
			info = "ID :\t\t" + std::to_string(g_config.current_id.multi) + "\n"
				+ "ポイント数 :\t" + std::to_string(g_curve_bezier_multi[g_config.current_id.multi - 1].ctpts.size);
			::MessageBox(
				hwnd,
				info.c_str(),
				CVE_FILTER_NAME,
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
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_ABOUT), hwnd, dialogproc_about);
			return 0;

			// カーブの数値・IDをコピー
		case ID_MENU_COPY:
		case CVE_CM_COPY:
		{
			TCHAR result_str[12];
			int result;

			switch (g_config.edit_mode) {
			case cve::Mode_Bezier:
				result = g_curve_bezier.create_number();
				break;

			case cve::Mode_Bezier_Multi:
				result = g_config.current_id.multi;
				break;

			case cve::Mode_Bezier_Value:
				result = g_config.current_id.value;
				break;

			case cve::Mode_Elastic:
				result = g_curve_elastic.create_number();
				break;

			case cve::Mode_Bounce:
				result = g_curve_bounce.create_number();
				break;

			default:
				return 0;
			}
			//文字列へ変換
			::_itoa_s(result, result_str, 12, 10);
			//コピー
			cve::copy_to_clipboard(hwnd, result_str);

			return 0;
		}

			// 値をコピー(4次元)
		case ID_MENU_COPY4D:
			if (g_config.edit_mode == cve::Mode_Bezier) {
				std::string str_result = g_curve_bezier.create_parameters();
				LPCTSTR result = str_result.c_str();

				cve::copy_to_clipboard(hwnd, result);
			}
			return 0;

			// 値を読み取り
		case CVE_CM_READ:
		case ID_MENU_READ:
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_READ), hwnd, dialogproc_read);
			g_window_main.redraw();

			return 0;

			// 保存ボタン
		case ID_MENU_SAVE:
		case CVE_CM_SAVE:
#ifdef _DEBUG
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_SAVE), hwnd, dialogproc_save);
#else
			::MessageBox(hwnd, "プリセット機能は現在未実装です。完成までもうしばらくお待ちください。", CVE_FILTER_NAME, MB_ICONINFORMATION);
#endif
			return 0;
			
			// ヘルプ
		case ID_MENU_HELP:
			::ShellExecute(NULL, "open", CVE_FILTER_LINK_HELP, NULL, NULL, SW_SHOWNORMAL);
			return 0;

			// 前のIDに移動
		case ID_MENU_ID_BACK:
			::SendMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_ID_BACK, 0);
			return 0;

			// 次のIDに移動
		case ID_MENU_ID_NEXT:
			::SendMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_ID_NEXT, 0);
			return 0;

			// レイアウトを縦向きにする
		case ID_MENU_VERTICAL:
			g_config.layout_mode = cve::Config::Vertical;

			g_window_main.redraw();
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_FIT, 0);
			return 0;

			// レイアウトを横向きにする
		case ID_MENU_HORIZONTAL:
			g_config.layout_mode = cve::Config::Horizontal;

			g_window_main.redraw();
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_FIT, 0);
			return 0;

			// 適用モードを通常にする
		case ID_MENU_APPLY_NORMAL:
			g_config.apply_mode = cve::Config::Normal;
			return 0;

			// 適用モードを中間点無視にする
		case ID_MENU_APPLY_IGNOREMID:
			g_config.apply_mode = cve::Config::Ignore_Mid_Point;
			return 0;
		}

		return 0;
	}

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}