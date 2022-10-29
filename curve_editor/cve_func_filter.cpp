//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (フィルタ関数)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include <yulib/extra.h>
#include "cve_header.hpp"



//---------------------------------------------------------------------
//		FILTER構造体のポインタを取得
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void)
{
	return &g_filter;
}



//---------------------------------------------------------------------
//		初期化時に実行される関数
//---------------------------------------------------------------------
BOOL filter_initialize(FILTER* fp)
{
	g_fp = fp;

	for (int i = 0; i < CVE_CURVE_MAX; i++) {
		g_curve_id[i].set_mode(cve::Mode_ID);
	}
	g_curve_id_previous.set_mode(cve::Mode_ID);


	// aviutl.iniから設定を読み込み
	ini_load_configs(fp);
	
	// フックの準備
	g_config.is_hooked_popup = false;
	g_config.is_hooked_dialog = false;

	char exedit_path[1024];
	FILTER* fp_exedit = auls::Exedit_GetFilter(fp);

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
	else
		::MessageBox(fp->hwnd, CVE_STR_ERROR_EXEDIT_NOT_FOUND, CVE_PLUGIN_NAME, MB_OK);


	// Direct2D初期化
	cve::d2d_init();
	return TRUE;
}



//---------------------------------------------------------------------
//		終了時に実行される関数
//---------------------------------------------------------------------
BOOL filter_exit(FILTER* fp)
{
	ini_write_configs(fp);
	if (NULL != g_render_target) {
		g_render_target->Release();
	}
	if (NULL != g_d2d1_factory) {
		g_d2d1_factory->Release();
	}
	return TRUE;
}



//---------------------------------------------------------------------
//		プロジェクトファイル読み込み時に実行される関数
//---------------------------------------------------------------------
BOOL on_project_load(FILTER* fp, void* editp, void* data, int size)
{
	static cve::Static_Array<cve::Curve_Points, CVE_POINT_MAX> point_data[CVE_CURVE_MAX];

	if (data) {
		memcpy(point_data, data, size);

		for (int i = 0; i < CVE_CURVE_MAX; i++)
			g_curve_id[i].ctpts = point_data[i];

		g_curve_id_previous = g_curve_id[g_config.current_id];

		if (g_window_editor.hwnd) {
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);

			for (int i = 0; i < CVE_CURVE_MAX; i++) {
				if (!g_curve_id[i].is_data_valid()) {
					int response = IDOK;

					response = ::MessageBox(
						fp->hwnd,
						CVE_STR_ERROR_DATA_INVALID,
						CVE_PLUGIN_NAME,
						MB_OKCANCEL | MB_ICONEXCLAMATION
					);

					if (response == IDOK)
						::PostMessage(g_window_editor.hwnd, WM_COMMAND, ID_MENU_DELETE_ALL, TRUE);

					break;
				}
			}
		}


		
	}
	return TRUE;
}



//---------------------------------------------------------------------
//		プロジェクトファイル保存時に実行される関数
//---------------------------------------------------------------------
BOOL on_project_save(FILTER* fp, void* editp, void* data, int* size)
{
	static cve::Static_Array<cve::Curve_Points, CVE_POINT_MAX> point_data[CVE_CURVE_MAX] = {};

	for (int i = 0; i < CVE_CURVE_MAX; i++)
		point_data[i] = g_curve_id[i].ctpts;

	if (!data) {
		*size = sizeof(point_data);
	}
	else {
		memcpy(data, point_data, sizeof(point_data));
	}
	return TRUE;
}



//---------------------------------------------------------------------
//		aviutl.iniから設定を読み込み
//---------------------------------------------------------------------
void ini_load_configs(FILTER* fp)
{
	g_config.theme = fp->exfunc->ini_load_int(fp, "theme", 0);
	g_config.trace = fp->exfunc->ini_load_int(fp, "show_previous_curve", true);
	g_config.alert = fp->exfunc->ini_load_int(fp, "show_alerts", true);
	g_config.auto_copy = fp->exfunc->ini_load_int(fp, "auto_copy", false);
	g_config.current_id = fp->exfunc->ini_load_int(fp, "id", 0);
	g_curve_value.ctpts[0].pt_right.x = MINMAX_LIMIT(fp->exfunc->ini_load_int(fp, "x1", (int)(CVE_GRAPH_RESOLUTION * CVE_CURVE_DEF_1)), 0, CVE_GRAPH_RESOLUTION);
	g_curve_value.ctpts[0].pt_right.y = fp->exfunc->ini_load_int(fp, "y1", (int)(CVE_GRAPH_RESOLUTION * CVE_CURVE_DEF_1));
	g_curve_value.ctpts[1].pt_left.x = MINMAX_LIMIT(fp->exfunc->ini_load_int(fp, "x2", (int)(CVE_GRAPH_RESOLUTION * CVE_CURVE_DEF_2)), 0, CVE_GRAPH_RESOLUTION);
	g_curve_value.ctpts[1].pt_left.y = fp->exfunc->ini_load_int(fp, "y2", (int)(CVE_GRAPH_RESOLUTION * CVE_CURVE_DEF_2));
	g_config.separator = fp->exfunc->ini_load_int(fp, "separator", CVE_SEPARATOR_W);
	g_config.mode = (cve::Mode)fp->exfunc->ini_load_int(fp, "mode", 0);
	g_config.align_handle = fp->exfunc->ini_load_int(fp, "align_handle", true);
	g_config.show_handle = fp->exfunc->ini_load_int(fp, "show_handle", true);
	g_config.preset_size = fp->exfunc->ini_load_int(fp, "preset_size", CVE_DEF_PRESET_SIZE);
	g_config.curve_color = fp->exfunc->ini_load_int(fp, "curve_color", CVE_CURVE_COLOR_DEFAULT);
}



//---------------------------------------------------------------------
//		aviutl.iniから設定を書き込み
//---------------------------------------------------------------------
void ini_write_configs(FILTER* fp)
{
	fp->exfunc->ini_save_int(fp, "x1", g_curve_value.ctpts[0].pt_right.x);
	fp->exfunc->ini_save_int(fp, "y1", g_curve_value.ctpts[0].pt_right.y);
	fp->exfunc->ini_save_int(fp, "x2", g_curve_value.ctpts[1].pt_left.x);
	fp->exfunc->ini_save_int(fp, "y2", g_curve_value.ctpts[1].pt_left.y);
	fp->exfunc->ini_save_int(fp, "separator", g_config.separator);
	fp->exfunc->ini_save_int(fp, "mode", g_config.mode);
	fp->exfunc->ini_save_int(fp, "align_handle", g_config.align_handle);
	fp->exfunc->ini_save_int(fp, "show_handle", g_config.show_handle);
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ
//---------------------------------------------------------------------
BOOL filter_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, void* editp, FILTER* fp)
{
	RECT rect_wnd;
	::GetClientRect(hwnd, &rect_wnd);

	switch (msg) {
		// ウィンドウ作成時
	case WM_FILTER_INIT:
		// WS_CLIPCHILDRENを追加
		::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_CLIPCHILDREN);

		g_window_main.create(
			hwnd,
			"WINDOW_MAIN",
			wndproc_main,
			NULL,
			NULL,
			rect_wnd,
			NULL
		);
		return 0;

	case WM_SIZE:
		g_window_main.move(rect_wnd);
		return 0;

	case WM_GETMINMAXINFO:
		MINMAXINFO* mmi;
		mmi = (MINMAXINFO*)lparam;
		mmi->ptMaxTrackSize.x = CVE_MAX_W;
		mmi->ptMaxTrackSize.y = CVE_MAX_H;
		mmi->ptMinTrackSize.y = g_config.separator + CVE_SEPARATOR_W + CVE_HEADER_H + CVE_NON_CLIENT_H;
		return 0;

	case WM_KEYDOWN:
		return cve::on_keydown(wparam);

	case WM_COMMAND:
		switch (wparam) {
		case CVE_CM_REDRAW_AVIUTL:
			return TRUE;
		}
	}
	return 0;
}