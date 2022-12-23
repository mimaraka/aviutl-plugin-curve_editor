//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (フィルタ関数)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

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

	// aviutl.iniから設定を読み込み
	ini_load_configs(fp);

	// Direct2Dの初期化
	cve::d2d_init();

	return TRUE;
}



//---------------------------------------------------------------------
//		終了時に実行される関数
//---------------------------------------------------------------------
BOOL filter_exit(FILTER* fp)
{
	// aviutl.iniに設定を書き込み
	ini_write_configs(fp);

	// Direct2Dオブジェクト開放
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
			g_curve_mb[i].ctpts = point_data[i];

		g_curve_mb_previous = g_curve_mb[g_config.current_id.multibezier - 1];

		if (g_window_editor.hwnd) {
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_REDRAW, 0);

			for (int i = 0; i < CVE_CURVE_MAX; i++) {
				if (!g_curve_mb[i].is_data_valid()) {
					int response = IDOK;

					response = ::MessageBox(
						fp->hwnd,
						CVE_STR_WARNING_DATA_INVALID,
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
		point_data[i] = g_curve_mb[i].ctpts;

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
	g_config.auto_apply = fp->exfunc->ini_load_int(fp, "auto_apply", true);
	g_curve_bezier.read_number(fp->exfunc->ini_load_int(fp, "num_bezier", CVE_NUM_BEZIER_LINER));
	g_curve_elastic.read_number(fp->exfunc->ini_load_int(fp, "num_elastic", CVE_NUM_ELASTIC_DEFAULT));
	g_curve_bounce.read_number(fp->exfunc->ini_load_int(fp, "num_bounce", CVE_NUM_BOUNCE_DEFAULT));
	g_config.separator = fp->exfunc->ini_load_int(fp, "separator", CVE_SEPARATOR_WIDTH);
	g_config.edit_mode = (cve::Edit_Mode)fp->exfunc->ini_load_int(fp, "edit_mode", cve::Mode_Bezier);
	g_config.layout_mode = (cve::Config::Layout_Mode)fp->exfunc->ini_load_int(fp, "layout_mode", cve::Config::Vertical);
	g_config.apply_mode = (cve::Config::Apply_Mode)fp->exfunc->ini_load_int(fp, "apply_mode", cve::Config::Normal);
	g_config.align_handle = fp->exfunc->ini_load_int(fp, "align_handle", true);
	g_config.show_handle = fp->exfunc->ini_load_int(fp, "show_handle", true);
	g_config.preset_size = fp->exfunc->ini_load_int(fp, "preset_size", CVE_DEF_PRESET_SIZE);
	g_config.curve_color = fp->exfunc->ini_load_int(fp, "curve_color", CVE_CURVE_COLOR_DEFAULT);
	g_config.notify_update = fp->exfunc->ini_load_int(fp, "notify_update", false);
}



//---------------------------------------------------------------------
//		aviutl.iniから設定を書き込み
//---------------------------------------------------------------------
void ini_write_configs(FILTER* fp)
{
	fp->exfunc->ini_save_int(fp, "num_bezier", g_curve_bezier.create_number());
	fp->exfunc->ini_save_int(fp, "num_elastic", g_curve_elastic.create_number());
	fp->exfunc->ini_save_int(fp, "num_bounce", g_curve_bounce.create_number());
	fp->exfunc->ini_save_int(fp, "separator", g_config.separator);
	fp->exfunc->ini_save_int(fp, "edit_mode", g_config.edit_mode);
	fp->exfunc->ini_save_int(fp, "layout_mode", g_config.layout_mode);
	fp->exfunc->ini_save_int(fp, "apply_mode", g_config.apply_mode);
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

	case WM_FILTER_CHANGE_WINDOW:
		::SendMessage(g_window_editor.hwnd, WM_COMMAND, CVE_CM_GET_EXEDIT, 0);
		return 0;

	case WM_SIZE:
		g_window_main.move(rect_wnd);
		return 0;

	case WM_GETMINMAXINFO:
		MINMAXINFO* mmi;
		mmi = (MINMAXINFO*)lparam;
		mmi->ptMaxTrackSize.x = CVE_MAX_W;
		mmi->ptMaxTrackSize.y = CVE_MAX_H;
		mmi->ptMinTrackSize.y = g_config.separator + CVE_SEPARATOR_WIDTH + CVE_HEADER_H + CVE_NON_CLIENT_H;
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