//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル(ウィンドウプロシージャ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_wndproc.hpp"



//---------------------------------------------------------------------
//		ウィンドウプロシージャ（ベース）
//---------------------------------------------------------------------
BOOL wndproc_base(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, void* editp, FILTER* fp)
{
	static HWND hwnd_main;
	RECT rect_wnd;
	GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	// ウィンドウ作成時
	case WM_FILTER_INIT:
		SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_CLIPCHILDREN);
		hwnd_main = create_child(
			hwnd, wndproc_main, "Wnd_Main", WS_CLIPCHILDREN,
			0, 0, rect_wnd.right, rect_wnd.bottom
		);
		g_window.base = hwnd;
		return 0;

	case WM_SIZE:
		MoveWindow(hwnd_main, 0, 0, rect_wnd.right, rect_wnd.bottom, TRUE);
		return 0;

	case WM_GETMINMAXINFO:
		MINMAXINFO* mmi;
		mmi = (MINMAXINFO*)lparam;
		mmi->ptMaxTrackSize.x = CE_MAX_W;
		mmi->ptMaxTrackSize.y = CE_MAX_H;
		return 0;

	case WM_KEYDOWN:
		switch (wparam) {
		case 82: //[R]
			if (g_window.graph) SendMessage(g_window.graph, WM_COMMAND, CE_CM_REVERSE, 0);
			return 0;

		case 67: //[C]
			if (GetAsyncKeyState(VK_CONTROL) < 0)
				SendMessage(g_window.graph, WM_COMMAND, CE_CT_APPLY, 0);
			return 0;

		case 83: //[S]
			if (GetAsyncKeyState(VK_CONTROL) < 0)
				SendMessage(g_window.graph, WM_COMMAND, CE_CT_SAVE, 0);
			else
				SendMessage(g_window.graph, WM_COMMAND, CE_CM_SHOWHANDLE, 0);
			return 0;

		case VK_LEFT: //[<]	
			if (g_config.mode) {
				g_config.current_id--;
				g_config.current_id = MINMAXLIM(g_config.current_id, 0, CE_CURVE_MAX - 1);
				g_curve_id_previous = g_curve_id[g_config.current_id];
				if (g_window.graph) SendMessage(g_window.graph, WM_COMMAND, CE_CM_REDRAW, 0);
			}
			return 0;

		case VK_RIGHT: //[>]
			if (g_config.mode) {
				g_config.current_id++;
				g_config.current_id = MINMAXLIM(g_config.current_id, 0, CE_CURVE_MAX - 1);
				g_curve_id_previous = g_curve_id[g_config.current_id];
				if (g_window.graph) SendMessage(g_window.graph, WM_COMMAND, CE_CM_REDRAW, 0);
			}
			return 0;

		case 70: //[F]
			if (g_window.graph) SendMessage(g_window.graph, WM_COMMAND, CE_CT_FIT, 0);
			return 0;

		case 65: //[A]
			if (g_window.graph) SendMessage(g_window.graph, WM_COMMAND, CE_CT_ALIGN, 0);
			return 0;

		case VK_DELETE:
			if (g_window.graph) SendMessage(g_window.graph, WM_COMMAND, CE_CM_DELETE, 0);
			return 0;
		}
		return 0;
	}
	return 0;
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ（メイン）
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_main(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static BOOL bSepr = FALSE;
	POINT cl_pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
	RECT rect_wnd, rect_sepr;
	//HANDLE
	static HWND hwnd_editor, hwnd_footer;
	static HDC hdc, hdc_mem;
	static HBITMAP bitmap;

	GetClientRect(hwnd, &rect_wnd);

	rect_sepr = {
		0,
		g_config.separator - CE_SEPR_W,
		rect_wnd.right,
		g_config.separator + CE_SEPR_W
	};

	switch (msg) {
	case WM_CLOSE:
		DeleteDC(hdc_mem);
		DeleteObject(bitmap);
		return 0;

	case WM_CREATE:
		hdc = GetDC(hwnd);
		hdc_mem = CreateCompatibleDC(hdc);
		bitmap = CreateCompatibleBitmap(hdc, CE_MAX_W, CE_MAX_H);
		SelectObject(hdc_mem, bitmap);
		ReleaseDC(hwnd, hdc);

		//エディタパネル
		hwnd_editor = create_child(
			hwnd, wndproc_editor, "Wnd_Editor", WS_CLIPCHILDREN,
			0, 0, rect_wnd.right, g_config.separator - CE_SEPR_W
		);

		//フッタパネル
		hwnd_footer = create_child(
			hwnd, wndproc_footer, "Wnd_Side", WS_CLIPCHILDREN,
			0, g_config.separator + CE_SEPR_W, rect_wnd.right, rect_wnd.bottom
		);

		g_window.main = hwnd;
		return 0;

	case WM_PAINT:
		draw_main(hwnd, hdc_mem, &rect_wnd, &rect_sepr);
		return 0;

	case WM_SIZE:
		MoveWindow(hwnd_editor, 0, 0, rect_wnd.right, g_config.separator - CE_SEPR_W, TRUE);
		MoveWindow(hwnd_footer, 0, g_config.separator + CE_SEPR_W, rect_wnd.right, rect_wnd.bottom, TRUE);
		return 0;

	case WM_LBUTTONDOWN:
		if (PtInRect(&rect_sepr, cl_pt)) {
			bSepr = TRUE;
			SetCursor(LoadCursor(NULL, IDC_SIZENS));
			SetCapture(hwnd);
		}
		return 0;

	case WM_LBUTTONUP:
		bSepr = FALSE;
		ReleaseCapture();
		return 0;

	case WM_MOUSEMOVE:
		if (PtInRect(&rect_sepr, cl_pt)) SetCursor(LoadCursor(NULL, IDC_SIZENS));
		if (bSepr) {
			g_config.separator = MINMAXLIM(cl_pt.y, CE_SEPR_W, rect_wnd.bottom - CE_SEPR_W);
			MoveWindow(hwnd_editor, 0, 0, rect_wnd.right, g_config.separator - CE_SEPR_W, TRUE);
			MoveWindow(hwnd_footer, 0, g_config.separator + CE_SEPR_W, rect_wnd.right, rect_wnd.bottom, TRUE);
			InvalidateRect(hwnd, NULL, FALSE);
		}
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ（エディタパネル）
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_editor(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//int
	int result;
	//LPTSTR, std::string
	LPCTSTR lpcsResult;
	std::string strBuffer;

	//POINT
	POINT ctpt_tmp;
	POINT cl_pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
	POINT gr_pt;

	//RECT
	RECT rect_wnd;

	//HANDLE
	static HDC hdc, hdc_mem;
	static HBITMAP bitmap;
	static HMENU menu;
	static HWND hwnd_parent, hwnd_graph;

	//その他
	static ce::Point_Address address;
	static ce::Control
		prev_play{ g_theme[g_config.theme].bg_window },
		prev_dur{ g_theme[g_config.theme].bg_window },
		fit{ g_theme[g_config.theme].bg_window },
		id_back{ g_theme[g_config.theme].bg_window },
		id_next{ g_theme[g_config.theme].bg_window },
		id{ g_theme[g_config.theme].bg_window },
		id_delete{ g_theme[g_config.theme].bg_window };


	//クライアント領域の矩形を取得
	GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	case WM_CLOSE:
		DeleteDC(hdc_mem);
		DeleteObject(bitmap);
		return 0;

		///////////////////////////////////////////
		//		WM_CREATE
		//		(ウィンドウが作成されたとき)
		///////////////////////////////////////////
	case WM_CREATE:
		//Load CompatibleItems
		hdc = GetDC(hwnd);
		hdc_mem = CreateCompatibleDC(hdc);
		bitmap = CreateCompatibleBitmap(hdc, CE_MAX_W, CE_MAX_H);
		SelectObject(hdc_mem, bitmap);
		ReleaseDC(hwnd, hdc);

		////メニュー
		//menu = GetSubMenu(LoadMenu(g_fp->dll_hinst, MAKEINTRESOURCE(g_config.lang ? IDR_MENU2 : IDR_MENU1)), 0);

		/*prev_play.create(hwnd, NULL, "prev_play", CE_ICON_PREV, &g_theme_dark.bg_others, CT_PREV,
			rect_wnd.left + CE_MRG, rect_wnd.bottom - CE_MRG - CT_EDITOR_HEIGHT,
			CT_EDITOR_HEIGHT, CT_EDITOR_HEIGHT
			);*/

		//グラフパネル
		hwnd_graph = create_child(
			hwnd, wndproc_graph, "Wnd_Graph", WS_CLIPCHILDREN,
			CE_MRG, CE_MRG, rect_wnd.right - CE_MRG * 2, rect_wnd.bottom - CE_MRG * 2
		);
		hwnd_parent = GetParent(hwnd);
		g_window.editor = hwnd;
		return 0;

		///////////////////////////////////////////
		//		WM_SIZE
		//		(サイズが変更されたとき)
		///////////////////////////////////////////
	case WM_SIZE:
		MoveWindow(hwnd_graph, CE_MRG, CE_MRG, rect_wnd.right - CE_MRG * 2, rect_wnd.bottom - CE_MRG * 2, TRUE);
		return 0;

		///////////////////////////////////////////
		//		WM_PAINT
		//		(ウィンドウが描画されたとき)
		///////////////////////////////////////////
	case WM_PAINT:
		draw_panel_editor(hwnd, hdc_mem, &rect_wnd);
		return 0;

		///////////////////////////////////////////
		//		WM_RBUTTONUP
		//		(マウスの右ボタンが離されたとき)
		///////////////////////////////////////////
		//右クリックメニュー
	case WM_RBUTTONUP:
		ClientToScreen(hwnd, &cl_pt);
		TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN, cl_pt.x, cl_pt.y, 0, hwnd, NULL);
		return 0;

		///////////////////////////////////////////
		//		WM_COMMAND
		//		(コマンドを受け取ったとき)
		///////////////////////////////////////////
	case WM_COMMAND:
		//switch (LOWORD(wparam)) {
		//	//COPYボタン
		//case ID_RCLICKMENU_COPYVALUES:
		//case CT_APPLY:
		//	TCHAR chResult[12];
		//	result = create_curve_value_1d();
		//	if (result == OUTOFRANGE) {
		//		if (g_config.bAlerts)
		//			MessageBox(hwnd, g_config.lang ? FLSTR_JA_OUTOFRANGE : FLSTR_OUTOFRANGE, TEXT("Flow"), MB_OK | MB_ICONINFORMATION);
		//		return 0;
		//	}
		//	//文字列へ変換
		//	_itoa_s(result, chResult, 12, 10);
		//	//コピー
		//	copy_to_clipboard(hwnd, chResult);
		//	return 0;

		//	//保存ボタン
		//case ID_RCLICKMENU_SAVEPRESET:
		//case CT_SAVE:
		//	//計算
		//	if (g_curve_value.ctpt[0].y < -2735 || 3735 < g_curve_value.ctpt[0].y || g_curve_value.ctpt[1].y < -2735 || 3735 < g_curve_value.ctpt[1].y) {
		//		if (g_config.bAlerts) MessageBox(hwnd, g_config.lang ? FLSTR_JA_OUTOFRANGE : FLSTR_OUTOFRANGE, "Flow", MB_OK | MB_ICONINFORMATION);
		//		return 0;
		//	}
		//	DialogBox(g_fp->dll_hinst, g_config.lang ? MAKEINTRESOURCE(IDD_SAVE_JA) : MAKEINTRESOURCE(IDD_SAVE), hwnd, wndproc_daialog_save);
		//	return 0;

		//	//Valueパネル
		//case CM_EDITOR_VALUE:
		//	if (!g_config.lang) DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_VALUE), hwnd, wndproc_daialog_value);
		//	else DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_VALUE_JA), hwnd, wndproc_daialog_value);
		//	InvalidateRect(hwnd, NULL, FALSE);
		//	RedrawChildren();
		//	return 0;

		//	//読み取りボタン
		//case CM_EDITOR_READ:
		//	if (!g_config.lang) DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_READ), hwnd, wndproc_daialog_read);
		//	else DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_READ_JA), hwnd, wndproc_daialog_read);
		//	InvalidateRect(hwnd, NULL, FALSE);
		//	return 0;

		//	//About
		//case ID_RCLICKMENU_ABOUT:
		//	if (!g_config.lang) DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_ABOUT), hwnd, wndproc_dialog_about);
		//	else DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_ABOUT_JA), hwnd, wndproc_dialog_about);
		//	return 0;

		//	//設定
		//case ID_RCLICKMENU_PREFERENCES:
		//	if (!g_config.lang) DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_PREF), hwnd, wndproc_daialog_settings);
		//	else DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_PREF_JA), hwnd, wndproc_daialog_settings);
		//	InvalidateRect(hwnd, NULL, FALSE);
		//	SendMessage(hwnd_parent, WM_COMMAND, CE_CM_REDRAW, 0);
		//	RedrawChildren();
		//	return 0;

		//	//値をコピー
		//case ID_RCLICKMENU_COPYVALUES4:
		//	strBuffer = create_curve_value_4d();
		//	lpcsResult = strBuffer.c_str();
		//	copy_to_clipboard(hwnd, lpcsResult);
		//	return 0;

		//	//コントロール再描画
		//case CE_CM_REDRAW:
		//	InvalidateRect(hwnd, NULL, FALSE);
		//	RedrawChildren();
		//	return 0;
		//}
		return 0;

	default:
		return(DefWindowProc(hwnd, msg, wparam, lparam));
	}
	return 0;
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ（グラフパネル）
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_graph(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//int
	static int		move_pt			= 0;			//0:None, 1:Ctpt1, 2:Ctpt2
	static BOOL		move_view		= 0;
	static int		o_x, o_y;
	static int		move_or_scale	= NULL;
	static int		is_dragging		= FALSE;

	//double
	static double	scale_x, scale_y;

	//POINT
	static POINT	pt_view;
	POINT			pt_tmp;							//値反転用
	POINT			cl_pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
	POINT			gr_pt;

	//RECT
	RECT			rect_wnd;

	//ハンドル
	static HDC		hdc, hdc_mem;
	static HBITMAP	bitmap;
	static HMENU	menu;
	static HWND		hwnd_parent;

	//その他
	static ce::Point_Address address;
	static MENUITEMINFO minfo = { 0 };


	//クライアント領域の矩形を取得
	GetClientRect(hwnd, &rect_wnd);
	//グラフ座標
	if (g_disp_info.o.x != NULL)
		gr_pt = to_graph(cl_pt);

	switch (msg) {
	case WM_CLOSE:
		DeleteDC(hdc_mem);
		DeleteObject(bitmap);
		return 0;

		///////////////////////////////////////////
		//		WM_CREATE
		//		(ウィンドウが作成されたとき)
		///////////////////////////////////////////
	case WM_CREATE:
		// コンパティブルデバイスコンテキストを用意
		hdc = GetDC(hwnd);
		hdc_mem = CreateCompatibleDC(hdc);
		bitmap = CreateCompatibleBitmap(hdc, CE_MAX_W, CE_MAX_H);
		SelectObject(hdc_mem, bitmap);
		ReleaseDC(hwnd, hdc);

		// メニュー
		menu = GetSubMenu(LoadMenu(g_fp->dll_hinst, MAKEINTRESOURCE(IDR_MENU1)), 0);

		minfo.cbSize = sizeof(MENUITEMINFO);

		g_curve_value_previous = g_curve_value;
		g_curve_id_previous = g_curve_id[g_config.current_id];

		g_disp_info.o.x = CE_GR_PADDING;
		g_disp_info.o.y = rect_wnd.bottom - CE_GR_PADDING;
		g_disp_info.scale.x = ((double)rect_wnd.right - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RESOLUTION;
		g_disp_info.scale.y = ((double)rect_wnd.bottom - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RESOLUTION;

		hwnd_parent = GetParent(hwnd);
		g_window.graph = hwnd;

		return 0;

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
		draw_panel_graph(hwnd, hdc_mem, &rect_wnd);
		return 0;

		///////////////////////////////////////////
		//		WM_LBUTTONDOWN
		//		(左クリックがされたとき)
		///////////////////////////////////////////
	case WM_LBUTTONDOWN:
		// 値モード&制御点がクリックされたとき
		if (g_config.mode == 0 && g_curve_value.point_in_ctpts(cl_pt) >= 1) {
			move_pt = g_curve_value.point_in_ctpts(cl_pt);
			g_curve_value_previous = g_curve_value;
			SetCursor(LoadCursor(NULL, IDC_HAND));
			SetCapture(hwnd);
		}
		// 値モードでないとき
		else {
			// カーソルが制御点上にあるかどうか
			address = g_curve_id[g_config.current_id].pt_in_ctpt(cl_pt);
			// カーソルが制御点上にあるとき，ハンドルの座標を記憶
			if (address.position != 0) {
				g_curve_id[g_config.current_id].move_point(address, gr_pt, TRUE);
				g_curve_id_previous = g_curve_id[g_config.current_id];
				SetCursor(LoadCursor(NULL, IDC_HAND));
				SetCapture(hwnd);
			}
			// カーブのD&D処理
			else {
				is_dragging = TRUE;
				SetCapture(hwnd);
			}
		}
		return 0;

		///////////////////////////////////////////
		//		WM_LBUTTONUP
		//		(マウスの左ボタンが離されたとき)
		///////////////////////////////////////////
	case WM_LBUTTONUP:
		// Valueモード
		if (!g_config.mode) {
			// 近くにある制御点をカーソルに移動
			if (!move_pt) {
				g_curve_value_previous = g_curve_value;
				int d1 = (int)DISTANCE(to_client(g_curve_value.ctpt[0]), cl_pt);
				int d2 = (int)DISTANCE(to_client(g_curve_value.ctpt[1]), cl_pt);
				if (d1 < d2) {
					g_curve_value.ctpt[0].x = to_graph(cl_pt).x;
					g_curve_value.ctpt[0].y = to_graph(cl_pt).y;
				}
				else if (d1 >= d2) {
					g_curve_value.ctpt[1].x = to_graph(cl_pt).x;
					g_curve_value.ctpt[1].y = to_graph(cl_pt).y;
				}
				InvalidateRect(hwnd, NULL, FALSE);
				if (g_config.auto_copy) SendMessage(hwnd, WM_COMMAND, CE_CT_APPLY, 0);
			}
			// 移動モード解除
			if (move_pt) {
				move_pt = 0;
				if (g_config.auto_copy) SendMessage(hwnd, WM_COMMAND, CE_CT_APPLY, 0);
			}
		}
		// IDモード・カーブのD&D処理
		else {
			address.position = ce::CTPT_NULL;
			is_dragging = FALSE;
		}
		ReleaseCapture();
		return 0;

		///////////////////////////////////////////
		//		WM_MBUTTONDOWN
		//		(マウスの左ボタンがダブルクリックされたとき)
		///////////////////////////////////////////
	case WM_LBUTTONDBLCLK:
		//カーソルが制御点上にあるかどうか
		address = g_curve_id[g_config.current_id].pt_in_ctpt(cl_pt);

		if (address.position == 1)
			g_curve_id[g_config.current_id].delete_point(cl_pt);
		else
			g_curve_id[g_config.current_id].add_point(gr_pt);

		InvalidateRect(hwnd, NULL, FALSE);
		return 0;

		///////////////////////////////////////////
		//		WM_RBUTTONUP
		//		(マウスの右ボタンが離されたとき)
		///////////////////////////////////////////
		//右クリックメニュー
	case WM_RBUTTONUP:
		apply_config_to_menu(menu, minfo);
		ClientToScreen(hwnd, &cl_pt);
		TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN, cl_pt.x, cl_pt.y, 0, hwnd, NULL);
		return 0;

		///////////////////////////////////////////
		//		WM_MBUTTONDOWN
		//		(マウスの中央ボタンが押されたとき)
		///////////////////////////////////////////
	case WM_MBUTTONDOWN:
		move_view	= TRUE;
		pt_view		= cl_pt;
		scale_x		= g_disp_info.scale.x;
		scale_y		= g_disp_info.scale.y;
		o_x			= g_disp_info.o.x;
		o_y			= g_disp_info.o.y;
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
			InvalidateRect(hwnd, NULL, FALSE);
		}
		//IDモード
		if (g_config.mode) {
			if (address.position) {
				g_curve_id[g_config.current_id].move_point(address, gr_pt, FALSE);
				InvalidateRect(hwnd, NULL, FALSE);
			}
		}

		//制御点ホバー時にカーソルを変更
		//Valueモード
		if (!g_config.mode) {
			if (g_curve_value.point_in_ctpts(cl_pt) > 0)
				SetCursor(LoadCursor(NULL, IDC_HAND));
		}
		//IDモード
		else {
			if (g_curve_id[g_config.current_id].pt_in_ctpt(cl_pt).position > 0)
				SetCursor(LoadCursor(NULL, IDC_HAND));
		}

		// ビュー移動・拡大縮小
		if (move_view && !move_pt) {
			// 拡大縮小
			if ((GetAsyncKeyState(VK_CONTROL) < 0 && move_or_scale == 0) || move_or_scale == 2) {
				double ratio_x, ratio_y;
				ratio_x = MINMAXLIM(std::pow(CE_GR_SCALE_INC, cl_pt.x - pt_view.x),
					CE_GR_SCALE_MIN / scale_x, CE_GR_SCALE_MAX / scale_x);
				ratio_y = MINMAXLIM(std::pow(CE_GR_SCALE_INC, pt_view.y - cl_pt.y),
					CE_GR_SCALE_MIN / scale_y, CE_GR_SCALE_MAX / scale_y);
				g_disp_info.scale.x = scale_x * ratio_x;
				g_disp_info.scale.y = scale_y * ratio_y;
				g_disp_info.o.x = rect_wnd.right * 0.5 + ratio_x * (o_x - rect_wnd.right * 0.5);
				g_disp_info.o.y = rect_wnd.bottom * 0.5 + ratio_y * (o_y - rect_wnd.bottom * 0.5);
				if (move_or_scale == 0)
					move_or_scale = 2;
			}
			// 移動
			else {
				g_disp_info.o.x = o_x + cl_pt.x - pt_view.x;
				g_disp_info.o.y = o_y + cl_pt.y - pt_view.y;
				if (move_or_scale == 0)
					move_or_scale = 1;
			}
			SetCursor(LoadCursor(NULL, IDC_SIZEALL));
			// 再描画
			InvalidateRect(hwnd, NULL, FALSE);
		}

		// カーブのD&D処理
		/*if (is_dragging) {
			SetCursor(LoadCursor(g_fp->dll_hinst, MAKEINTRESOURCE(IDC_DRAGGING)));
		}*/

		return 0;

		///////////////////////////////////////////
		//		WM_MOUSEWHEEL
		//		(マウスホイールが回転したとき)
		///////////////////////////////////////////
	case WM_MOUSEWHEEL:
	{
		// Ctrlキーが押されているとき(横に移動)
		if (GetAsyncKeyState(VK_CONTROL) < 0 && GetAsyncKeyState(VK_SHIFT) >= 0)
			g_disp_info.o.x += GET_Y_LPARAM(wparam) * CE_GR_WHEEL_COEF_POS;

		// Shiftキーが押されているとき(縦に移動)
		else if (GetAsyncKeyState(VK_SHIFT) < 0 && GetAsyncKeyState(VK_CONTROL) >= 0)
			g_disp_info.o.y += GET_Y_LPARAM(wparam) * CE_GR_WHEEL_COEF_POS;

		// 縮尺の上限下限を設定
		else {
			double coef = std::pow(CE_GR_SCALE_INC, GET_Y_LPARAM(wparam) * CE_GR_WHEEL_COEF_SCALE);
			double scale_after_x, scale_after_y;

			if (max(g_disp_info.scale.x, g_disp_info.scale.y) > CE_GR_SCALE_MAX / coef) {
				coef = CE_GR_SCALE_MAX / max(g_disp_info.scale.x, g_disp_info.scale.y);
			}
			else if (min(g_disp_info.scale.x, g_disp_info.scale.y) < CE_GR_SCALE_MIN / coef) {
				coef = CE_GR_SCALE_MIN / min(g_disp_info.scale.x, g_disp_info.scale.y);
			}

			scale_after_x = g_disp_info.scale.x * coef;
			scale_after_y = g_disp_info.scale.y * coef;

			g_disp_info.o.x = (g_disp_info.o.x - rect_wnd.right * 0.5) * (scale_after_x / g_disp_info.scale.x) + rect_wnd.right * 0.5;
			g_disp_info.o.y = (g_disp_info.o.y - rect_wnd.bottom * 0.5) * (scale_after_y / g_disp_info.scale.y) + rect_wnd.bottom * 0.5;

			g_disp_info.scale.x = scale_after_x;
			g_disp_info.scale.y = scale_after_y;
		}
		// 再描画
		InvalidateRect(hwnd, NULL, FALSE);
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
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		case ID_MENU_MODE_VALUE:
			g_config.mode = 0;
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		case ID_MENU_MODE_ID:
			g_config.mode = 1;
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		case ID_MENU_FIT:
		case CE_CT_FIT:
			g_disp_info.o.x = CE_GR_PADDING;
			g_disp_info.o.y = rect_wnd.bottom - CE_GR_PADDING;
			g_disp_info.scale.x = ((double)rect_wnd.right - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RESOLUTION;
			g_disp_info.scale.y = ((double)rect_wnd.bottom - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RESOLUTION;
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		case ID_MENU_ALIGNHANDLE:
		case CE_CT_ALIGN:
			g_config.align_handle = g_config.align_handle ? 0 : 1;
			return 0;

		case ID_MENU_REVERSE:
		case CE_CM_REVERSE:
			// IDモード
			if (g_config.mode == 1)
				g_curve_id[g_config.current_id].reverse_curve();
			// Valueモード
			else {
				pt_tmp = g_curve_value.ctpt[0];
				g_curve_value.ctpt[0].x = CE_GR_RESOLUTION - g_curve_value.ctpt[1].x;
				g_curve_value.ctpt[0].y = CE_GR_RESOLUTION - g_curve_value.ctpt[1].y;
				g_curve_value.ctpt[1].x = CE_GR_RESOLUTION - pt_tmp.x;
				g_curve_value.ctpt[1].y = CE_GR_RESOLUTION - pt_tmp.y;
			}
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		case ID_MENU_SHOWHANDLE:
		case CE_CM_SHOWHANDLE:
			g_config.show_handle = g_config.show_handle ? 0 : 1;
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		case ID_MENU_DELETE:
		case CE_CM_DELETE:
		{
			int response = IDOK;
			if (g_config.alert)
				response = MessageBox(hwnd, "編集中のカーブを初期化します。よろしいですか？", CE_FILTER_NAME, MB_OKCANCEL);
			if (response == IDOK) {
				if (g_config.mode)
					g_curve_id[g_config.current_id].clear();
				else
					g_curve_value.init();

				InvalidateRect(hwnd, NULL, FALSE);
			}
			return 0;
		}

		case ID_MENU_PROPERTY:
		{
			std::string info;
			info = "ID : " + std::to_string(g_config.current_id) + "\n"
				+ "ポイント数 : " + std::to_string(g_curve_id[g_config.current_id].ctpts.size);
			MessageBox(hwnd, info.c_str(), CE_FILTER_NAME, MB_OK | MB_ICONINFORMATION);
			return 0;
		}
		}
		return 0;

	default:
		return(DefWindowProc(hwnd, msg, wparam, lparam));
	}
	return 0;
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ（フッタパネル）
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_footer(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static HDC hdc, hdc_mem;
	static HBITMAP bitmap;
	RECT rect_wnd;
	static HWND hwnd_test;

	GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
	case WM_CLOSE:
		DeleteDC(hdc_mem);
		DeleteObject(bitmap);
		return 0;

	case WM_CREATE:
		hdc = GetDC(hwnd);
		hdc_mem = CreateCompatibleDC(hdc);
		bitmap = CreateCompatibleBitmap(hdc, CE_MAX_W, CE_MAX_H);
		SelectObject(hdc_mem, bitmap);
		ReleaseDC(hwnd, hdc);

		return 0;

	case WM_PAINT:
		draw_footer(hwnd, hdc_mem, &rect_wnd);
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ（ライブラリ）
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_library(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	POINT			cl_pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
	HDC				hdc;
	static HDC		hdc_mem;
	static HBITMAP	bitmap;
	RECT			rect_wnd;

	GetClientRect(hwnd, &rect_wnd);


	switch (msg) {
	case WM_CLOSE:
		DeleteDC(hdc_mem);
		DeleteObject(bitmap);
		return 0;

	case WM_CREATE:
		hdc		= GetDC(hwnd);
		hdc_mem = CreateCompatibleDC(hdc);
		bitmap	= CreateCompatibleBitmap(hdc, CE_MAX_W, CE_MAX_H);
		SelectObject(hdc_mem, bitmap);
		ReleaseDC(hwnd, hdc);

		return 0;

	case WM_PAINT:
		draw_panel_library(hwnd, hdc_mem, &rect_wnd);
		return 0;

	case WM_SIZE:

		return 0;

	case WM_COMMAND:
		/*switch (wparam) {
		case CE_CM_REDRAW:
			InvalidateRect(hwnd, NULL, FALSE);
			SendMessage(buttons.hDefault, WM_COMMAND, CE_CM_REDRAW, 0);
			SendMessage(buttons.hUser, WM_COMMAND, CE_CM_REDRAW, 0);
			return 0;
		}*/
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}