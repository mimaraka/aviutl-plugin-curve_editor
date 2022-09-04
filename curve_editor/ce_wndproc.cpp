//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル(ウィンドウプロシージャ)
//		VC++ 2022
//----------------------------------------------------------------------------------

#include "ce_wndproc.hpp"

//デバッグ
#define MSGBOX(str) MessageBox(hwnd, str, "Debug", MB_ICONINFORMATION)

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
			if (g_window.graph) SendMessage(g_window.graph, WM_COMMAND, CE_WM_REVERSE, 0);
			return 0;

		case 67: //[C]
			if (GetAsyncKeyState(VK_CONTROL) < 0)
				SendMessage(g_window.graph, WM_COMMAND, CE_CT_APPLY, 0);
			return 0;

		case 83: //[S]
			if (GetAsyncKeyState(VK_CONTROL) < 0)
				SendMessage(g_window.graph, WM_COMMAND, CE_CT_SAVE, 0);
			else
				SendMessage(g_window.graph, WM_COMMAND, CE_WM_SHOWHANDLE, 0);
			return 0;

		case 37: //[<]	
			g_config.id_current--;
			g_config.id_current = MINMAXLIM(g_config.id_current, 0, CE_CURVE_MAX - 1);
			if (g_window.graph) SendMessage(g_window.graph, WM_COMMAND, CE_WM_REDRAW, 0);
			return 0;

		case 39: //[>]
			g_config.id_current++;
			g_config.id_current = MINMAXLIM(g_config.id_current, 0, CE_CURVE_MAX - 1);
			if (g_window.graph) SendMessage(g_window.graph, WM_COMMAND, CE_WM_REDRAW, 0);
			return 0;

		case 70: //[F]
			if (g_window.graph) SendMessage(g_window.graph, WM_COMMAND, CE_CT_FIT, 0);
			return 0;

		case 65: //[A]
			if (g_window.graph) SendMessage(g_window.graph, WM_COMMAND, CE_CT_ALIGN, 0);
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
	static HWND hwnd_editor, hwnd_side;
	static HDC hdc, hdc_mem;
	static HBITMAP bitmap;

	GetClientRect(hwnd, &rect_wnd);

	rect_sepr = {
		g_config.separator - CE_SEPR_W,
		0,
		g_config.separator + CE_SEPR_W,
		rect_wnd.bottom
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

		//サイドパネル
		hwnd_side = create_child(
			hwnd, wndproc_side, "Wnd_Side", WS_CLIPCHILDREN,
			CE_WD_POS_SIDE(rect_wnd)
		);

		//エディタ
		hwnd_editor = create_child(
			hwnd, wndproc_editor, "Wnd_Editor", WS_CLIPCHILDREN,
			CE_WD_POS_EDT(rect_wnd)
		);

		g_window.main = hwnd;
		return 0;

	case WM_PAINT:
		draw_main(hwnd, hdc_mem, &rect_wnd);
		return 0;

	case WM_SIZE:
		MoveWindow(hwnd_side, CE_WD_POS_SIDE(rect_wnd), TRUE);
		MoveWindow(hwnd_editor, CE_WD_POS_EDT(rect_wnd), TRUE);
		return 0;

	case WM_LBUTTONDOWN:
		if (PtInRect(&rect_sepr, cl_pt)) {
			bSepr = TRUE;
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));
			SetCapture(hwnd);
		}
		return 0;

	case WM_LBUTTONUP:
		bSepr = FALSE;
		ReleaseCapture();
		return 0;

	case WM_MOUSEMOVE:
		if (PtInRect(&rect_sepr, cl_pt)) SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		if (bSepr) {
			g_config.separator = MINMAXLIM(cl_pt.x, CE_WD_SIDE_MINW + CE_SEPR_W, rect_wnd.right - CE_SEPR_W);
			MoveWindow(hwnd_side, CE_WD_POS_SIDE(rect_wnd), TRUE);
			MoveWindow(hwnd_editor, CE_WD_POS_EDT(rect_wnd), TRUE);
			InvalidateRect(hwnd, NULL, FALSE);
		}
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ（サイドパネル）
//---------------------------------------------------------------------
LRESULT CALLBACK wndproc_side(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static HDC hdc, hdc_mem;
	static HBITMAP bitmap;
	RECT rect_wnd;
	static HWND hwnd_test;
	ce::Control
		mode_value{ g_theme[g_config.theme].bg_window },
		mode_id{ g_theme[g_config.theme].bg_window },
		apply{ g_theme[g_config.theme].bt_apply },
		save{ g_theme[g_config.theme].bg_window },
		read{ g_theme[g_config.theme].bg_window },
		align{ g_theme[g_config.theme].bg_window },
		pref{ g_theme[g_config.theme].bg_window };

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

		hwnd_test = create_child(hwnd, WndProc_Test, "test", NULL, CE_CT_POS_APPLY(rect_wnd));

		//↓これを出すとバグる

		/*mode_value.create(
			hwnd, NULL,
			"Ct_Mode_Value",
			"Value",
			CE_CT_MODE_VALUE,
			CE_CT_POS_MODE_VALUE(rect_wnd),
			CE_EDGE_RT
		);

		mode_id.create(
			hwnd, NULL,
			"Ct_Mode_ID",
			"ID",
			CE_CT_MODE_ID,
			CE_CT_POS_MODE_ID(rect_wnd),
			CE_EDGE_LT
		);

		apply.create(
			hwnd, NULL,
			"Ct_Apply",
			"Apply",
			CE_CT_APPLY,
			CE_CT_POS_APPLY(rect_wnd),
			NULL
		);

		save.create(
			hwnd, NULL,
			"Ct_Save",
			CE_ICON_SAVE,
			CE_CT_SAVE,
			CE_CT_POS_SAVE(rect_wnd),
			CE_EDGE_LB
		);

		read.create(
			hwnd, NULL,
			"Ct_Read",
			CE_ICON_READ,
			CE_CT_READ,
			CE_CT_POS_READ(rect_wnd),
			NULL
		);

		align.create(
			hwnd, NULL,
			"Ct_Align",
			CE_ICON_ALIGN,
			CE_CT_ALIGN,
			CE_CT_POS_ALIGN(rect_wnd),
			NULL
		);

		pref.create(
			hwnd, NULL,
			"Ct_Pref",
			CE_ICON_PREF,
			CE_CT_PREF,
			CE_CT_POS_PREF(rect_wnd),
			CE_EDGE_RB
		);*/

		return 0;

	case WM_PAINT:
		draw_side(hwnd, hdc_mem, &rect_wnd);
		return 0;

	case WM_SIZE:
		MoveWindow(hwnd_test, CE_CT_POS_APPLY(rect_wnd), TRUE);
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
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

		//グラフ
		hwnd_graph = create_child(
			hwnd, wndproc_graph, "Wnd_Graph", WS_CLIPCHILDREN,
			CE_WD_POS_GRAPH(rect_wnd)
		);
		hwnd_parent = GetParent(hwnd);
		g_window.editor = hwnd;
		return 0;

		///////////////////////////////////////////
		//		WM_SIZE
		//		(サイズが変更されたとき)
		///////////////////////////////////////////
	case WM_SIZE:
		MoveWindow(hwnd_graph, CE_WD_POS_GRAPH(rect_wnd), TRUE);
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
		//	result = create_value_1d();
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
		//	if (g_curve_value.control_point[0].y < -2735 || 3735 < g_curve_value.control_point[0].y || g_curve_value.control_point[1].y < -2735 || 3735 < g_curve_value.control_point[1].y) {
		//		if (g_config.bAlerts) MessageBox(hwnd, g_config.lang ? FLSTR_JA_OUTOFRANGE : FLSTR_OUTOFRANGE, "Flow", MB_OK | MB_ICONINFORMATION);
		//		return 0;
		//	}
		//	DialogBox(g_fp->dll_hinst, g_config.lang ? MAKEINTRESOURCE(IDD_SAVE_JA) : MAKEINTRESOURCE(IDD_SAVE), hwnd, wndproc_daialog_save);
		//	return 0;

		//	//制御点を反転
		//case CM_REVERSE:
		//	pt_tmp = g_curve_value.control_point[0];
		//	g_curve_value.control_point[0].x = CE_GR_RES - g_curve_value.control_point[1].x;
		//	g_curve_value.control_point[0].y = CE_GR_RES - g_curve_value.control_point[1].y;
		//	g_curve_value.control_point[1].x = CE_GR_RES - pt_tmp.x;
		//	g_curve_value.control_point[1].y = CE_GR_RES - pt_tmp.y;
		//	InvalidateRect(hwnd, NULL, FALSE);
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
		//	SendMessage(hwnd_parent, WM_COMMAND, CE_WM_REDRAW, 0);
		//	RedrawChildren();
		//	return 0;

		//	//値をコピー
		//case ID_RCLICKMENU_COPYVALUES4:
		//	strBuffer = create_value_4d();
		//	lpcsResult = strBuffer.c_str();
		//	copy_to_clipboard(hwnd, lpcsResult);
		//	return 0;

		//	//コントロール再描画
		//case CE_WM_REDRAW:
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
	static int		move_pt		= 0;			//0:None, 1:Ctpt1, 2:Ctpt2
	static BOOL		move_view	= 0;
	static int		o_x, o_y;

	//double
	static double	scale_x, scale_y;

	//POINT
	static POINT	pt_trace[] = { {0, 0}, {0, 0} };
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
		//Load CompatibleItems
		hdc		= GetDC(hwnd);
		hdc_mem = CreateCompatibleDC(hdc);
		bitmap	= CreateCompatibleBitmap(hdc, CE_MAX_W, CE_MAX_H);
		SelectObject(hdc_mem, bitmap);
		ReleaseDC(hwnd, hdc);

		//メニュー
		//menu = GetSubMenu(LoadMenu(g_fp->dll_hinst, MAKEINTRESOURCE(g_config.lang ? IDR_MENU2 : IDR_MENU1)), 0);

		pt_trace[0] = g_curve_value.control_point[0];
		pt_trace[1] = g_curve_value.control_point[1];

		g_disp_info.o.x = CE_GR_PADDING;
		g_disp_info.o.y = rect_wnd.bottom - CE_GR_PADDING;
		g_disp_info.scale.x = ((double)rect_wnd.right - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RES;
		g_disp_info.scale.y = ((double)rect_wnd.bottom - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RES;

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
		draw_panel_graph(hwnd, hdc_mem, pt_trace, &rect_wnd);
		return 0;

		///////////////////////////////////////////
		//		WM_LBUTTONDOWN
		//		(左クリックがされたとき)
		///////////////////////////////////////////
	case WM_LBUTTONDOWN:
		//値モード
		if (!g_config.mode) {
			//制御点2がクリックされたとき
			if (g_curve_value.point_in_control_points(cl_pt) == 2) {
				move_pt = 2;
				pt_trace[0] = g_curve_value.control_point[0];
				pt_trace[1] = g_curve_value.control_point[1];
				SetCursor(LoadCursor(NULL, IDC_HAND));
				SetCapture(hwnd);
			}
			//制御点1がクリックされたとき
			else if (g_curve_value.point_in_control_points(cl_pt) == 1) {
				move_pt = 1;
				pt_trace[0] = g_curve_value.control_point[0];
				pt_trace[1] = g_curve_value.control_point[1];
				SetCursor(LoadCursor(NULL, IDC_HAND));
				SetCapture(hwnd);
			}
		}
		//IDモード
		else {
			//カーソルが制御点上にあるかどうか
			address = g_curve_id[g_config.id_current].pt_in_control_points(cl_pt);
			//カーソルが制御点上にあるとき，ハンドルの座標を記憶
			if (address.position != 0) {
				g_curve_id[g_config.id_current].move_point(address, gr_pt, TRUE);
				SetCursor(LoadCursor(NULL, IDC_HAND));
				SetCapture(hwnd);
			}
		}
		return 0;

		///////////////////////////////////////////
		//		WM_LBUTTONUP
		//		(マウスの左ボタンが離されたとき)
		///////////////////////////////////////////
	case WM_LBUTTONUP:
		//Valueモード
		if (!g_config.mode) {
			//近くにある制御点をカーソルに移動
			if (!move_pt) {
				pt_trace[0] = g_curve_value.control_point[0]; pt_trace[1] = g_curve_value.control_point[1];
				int d1 = (int)DISTANCE(to_client(g_curve_value.control_point[0]), cl_pt);
				int d2 = (int)DISTANCE(to_client(g_curve_value.control_point[1]), cl_pt);
				if (d1 < d2) {
					g_curve_value.control_point[0].x = to_graph(cl_pt).x;
					g_curve_value.control_point[0].y = to_graph(cl_pt).y;
				}
				else if (d1 >= d2) {
					g_curve_value.control_point[1].x = to_graph(cl_pt).x;
					g_curve_value.control_point[1].y = to_graph(cl_pt).y;
				}
				InvalidateRect(hwnd, NULL, FALSE);
				if (g_config.auto_copy) SendMessage(hwnd, WM_COMMAND, CE_CT_APPLY, 0);
			}
			//移動モード解除
			if (move_pt) {
				move_pt = 0;
				if (g_config.auto_copy) SendMessage(hwnd, WM_COMMAND, CE_CT_APPLY, 0);
			}
		}
		//IDモード
		else address.position = ce::CTPT_NULL;
		ReleaseCapture();
		return 0;

		///////////////////////////////////////////
		//		WM_MBUTTONDOWN
		//		(マウスの左ボタンがダブルクリックされたとき)
		///////////////////////////////////////////
	case WM_LBUTTONDBLCLK:
		//カーソルが制御点上にあるかどうか
		address = g_curve_id[g_config.id_current].pt_in_control_points(cl_pt);

		if (address.position == 1)
			g_curve_id[g_config.id_current].delete_point(cl_pt);
		else
			g_curve_id[g_config.id_current].add_point(gr_pt);

		InvalidateRect(hwnd, NULL, FALSE);
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
		SetCapture(hwnd);
		return 0;

		///////////////////////////////////////////
		//		WM_MBUTTONUP
		//		(マウスの中央ボタンが離されたとき)
		///////////////////////////////////////////
	case WM_MBUTTONUP:
		move_view = FALSE;
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
				g_curve_id[g_config.id_current].move_point(address, gr_pt, FALSE);
				InvalidateRect(hwnd, NULL, FALSE);
			}
		}

		//制御点ホバー時にカーソルを変更
		//Valueモード
		if (!g_config.mode) {
			if (g_curve_value.point_in_control_points(cl_pt) > 0)
				SetCursor(LoadCursor(NULL, IDC_HAND));
		}
		//IDモード
		else {
			if (g_curve_id[g_config.id_current].pt_in_control_points(cl_pt).position > 0)
				SetCursor(LoadCursor(NULL, IDC_HAND));
		}

		//ビュー移動・拡大縮小
		if (move_view && !move_pt) {
			if (GetAsyncKeyState(VK_CONTROL) < 0) {
				double ratio_x, ratio_y;
				ratio_x = MINMAXLIM(std::pow(CE_GR_SCALE_INC, cl_pt.x - pt_view.x),
					CE_GR_SCALE_MIN / scale_x, CE_GR_SCALE_MAX / scale_x);
				ratio_y = MINMAXLIM(std::pow(CE_GR_SCALE_INC, pt_view.y - cl_pt.y),
					CE_GR_SCALE_MIN / scale_y, CE_GR_SCALE_MAX / scale_y);
				g_disp_info.scale.x = scale_x * ratio_x;
				g_disp_info.scale.y = scale_y * ratio_y;
				g_disp_info.o.x = rect_wnd.right * 0.5 + ratio_x * (o_x - rect_wnd.right * 0.5);
				g_disp_info.o.y = rect_wnd.bottom * 0.5 + ratio_y * (o_y - rect_wnd.bottom * 0.5);
			}
			else {
				g_disp_info.o.x = o_x + cl_pt.x - pt_view.x;
				g_disp_info.o.y = o_y + cl_pt.y - pt_view.y;
			}
			InvalidateRect(hwnd, NULL, FALSE);
		}

		return 0;

		///////////////////////////////////////////
		//		WM_MOUSEWHEEL
		//		(マウスホイールが回転したとき)
		///////////////////////////////////////////
	case WM_MOUSEWHEEL:
		g_disp_info.scale.x = MINMAXLIM(g_disp_info.scale.x * std::pow(CE_GR_SCALE_INC, GET_Y_LPARAM(wparam) * CE_GR_WHEEL_RATIO), CE_GR_SCALE_MIN, CE_GR_SCALE_MAX);
		g_disp_info.scale.y = MINMAXLIM(g_disp_info.scale.y * std::pow(CE_GR_SCALE_INC, GET_Y_LPARAM(wparam) * CE_GR_WHEEL_RATIO), CE_GR_SCALE_MIN, CE_GR_SCALE_MAX);
		g_disp_info.o.x = (g_disp_info.o.x - rect_wnd.right * 0.5) * std::pow(CE_GR_SCALE_INC, GET_Y_LPARAM(wparam) * CE_GR_WHEEL_RATIO) + rect_wnd.right * 0.5;
		g_disp_info.o.y = (g_disp_info.o.y - rect_wnd.bottom * 0.5) * std::pow(CE_GR_SCALE_INC, GET_Y_LPARAM(wparam) * CE_GR_WHEEL_RATIO) + rect_wnd.bottom * 0.5;
		
		InvalidateRect(hwnd, NULL, FALSE);
		return 0;

		///////////////////////////////////////////
		//		WM_COMMAND
		//		(コマンド)
		///////////////////////////////////////////
	case WM_COMMAND:
		switch (wparam) {
		//再描画
		case CE_WM_REDRAW:
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		case CE_CT_FIT:
			g_disp_info.o.x = CE_GR_PADDING;
			g_disp_info.o.y = rect_wnd.bottom - CE_GR_PADDING;
			g_disp_info.scale.x = ((double)rect_wnd.right - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RES;
			g_disp_info.scale.y = ((double)rect_wnd.bottom - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RES;
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		case CE_CT_ALIGN:
			g_config.align_mode = g_config.align_mode ? 0 : 1;
			return 0;

		case CE_WM_REVERSE:
			g_curve_id[g_config.id_current].reverse_points();
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;

		case CE_WM_SHOWHANDLE:
			g_config.show_handle = g_config.show_handle ? 0 : 1;
			InvalidateRect(hwnd, NULL, FALSE);
			return 0;
		}
		return 0;

	default:
		return(DefWindowProc(hwnd, msg, wparam, lparam));
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
		case CE_WM_REDRAW:
			InvalidateRect(hwnd, NULL, FALSE);
			SendMessage(buttons.hDefault, WM_COMMAND, CE_WM_REDRAW, 0);
			SendMessage(buttons.hUser, WM_COMMAND, CE_WM_REDRAW, 0);
			return 0;
		}*/
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}