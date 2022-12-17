//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C�� (�t�B���^�֐�)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include <yulib/extra.h>
#include "cve_header.hpp"



//---------------------------------------------------------------------
//		FILTER�\���̂̃|�C���^���擾
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void)
{
	return &g_filter;
}



//---------------------------------------------------------------------
//		���������Ɏ��s�����֐�
//---------------------------------------------------------------------
BOOL filter_initialize(FILTER* fp)
{
	g_fp = fp;

	// aviutl.ini����ݒ��ǂݍ���
	ini_load_configs(fp);
	
	// �t�b�N�̏���
	g_config.is_hooked_popup = false;
	g_config.is_hooked_dialog = false;

	char exedit_path[1024];
	FILTER* fp_exedit = auls::Exedit_GetFilter(fp);

	// �g���ҏW�v���O�C�������݂���ꍇ
	if (fp_exedit) {
		::GetModuleFileName(fp_exedit->dll_hinst, exedit_path, sizeof(exedit_path));

		// TrackPopupMenu()���t�b�N
		TrackPopupMenu_original = (decltype(TrackPopupMenu_original))yulib::RewriteFunction(
			exedit_path,
			"TrackPopupMenu",
			TrackPopupMenu_hooked
		);

		// DialogBoxParamA()���t�b�N
		DialogBox_original = (decltype(DialogBox_original))yulib::RewriteFunction(
			exedit_path,
			"DialogBoxParamA",
			DialogBox_hooked
		);
	}
	// ���݂��Ȃ��ꍇ
	else
		::MessageBox(fp->hwnd, CVE_STR_ERROR_EXEDIT_NOT_FOUND, CVE_PLUGIN_NAME, MB_OK);

	// Direct2D�̏�����
	cve::d2d_init();

	return TRUE;
}



//---------------------------------------------------------------------
//		�I�����Ɏ��s�����֐�
//---------------------------------------------------------------------
BOOL filter_exit(FILTER* fp)
{
	// aviutl.ini�ɐݒ����������
	ini_write_configs(fp);

	// Direct2D�I�u�W�F�N�g�J��
	if (NULL != g_render_target) {
		g_render_target->Release();
	}
	if (NULL != g_d2d1_factory) {
		g_d2d1_factory->Release();
	}
	return TRUE;
}



//---------------------------------------------------------------------
//		�v���W�F�N�g�t�@�C���ǂݍ��ݎ��Ɏ��s�����֐�
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
//		�v���W�F�N�g�t�@�C���ۑ����Ɏ��s�����֐�
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
//		aviutl.ini����ݒ��ǂݍ���
//---------------------------------------------------------------------
void ini_load_configs(FILTER* fp)
{
	g_config.theme = fp->exfunc->ini_load_int(fp, "theme", 0);
	g_config.trace = fp->exfunc->ini_load_int(fp, "show_previous_curve", true);
	g_config.alert = fp->exfunc->ini_load_int(fp, "show_alerts", true);
	g_config.auto_copy = fp->exfunc->ini_load_int(fp, "auto_copy", false);
	g_curve_normal.ctpts[0].pt_right.x = MINMAX_LIMIT(fp->exfunc->ini_load_int(fp, "x1", (int)(CVE_GRAPH_RESOLUTION * CVE_POINT_DEFAULT_1)), 0, CVE_GRAPH_RESOLUTION);
	g_curve_normal.ctpts[0].pt_right.y = fp->exfunc->ini_load_int(fp, "y1", (int)(CVE_GRAPH_RESOLUTION * CVE_POINT_DEFAULT_1));
	g_curve_normal.ctpts[1].pt_left.x = MINMAX_LIMIT(fp->exfunc->ini_load_int(fp, "x2", (int)(CVE_GRAPH_RESOLUTION * CVE_POINT_DEFAULT_2)), 0, CVE_GRAPH_RESOLUTION);
	g_curve_normal.ctpts[1].pt_left.y = fp->exfunc->ini_load_int(fp, "y2", (int)(CVE_GRAPH_RESOLUTION * CVE_POINT_DEFAULT_2));

	g_config.separator = fp->exfunc->ini_load_int(fp, "separator", CVE_SEPARATOR_WIDTH);
	g_config.edit_mode = (cve::Edit_Mode)fp->exfunc->ini_load_int(fp, "edit_mode", cve::Mode_Bezier);
	g_config.layout_mode = (cve::Config::Layout_Mode)fp->exfunc->ini_load_int(fp, "layout_mode", cve::Config::Vertical);
	g_config.apply_mode = (cve::Config::Apply_Mode)fp->exfunc->ini_load_int(fp, "apply_mode", cve::Config::Normal);
	g_config.align_handle = fp->exfunc->ini_load_int(fp, "align_handle", true);
	g_config.show_handle = fp->exfunc->ini_load_int(fp, "show_handle", true);
	g_config.preset_size = fp->exfunc->ini_load_int(fp, "preset_size", CVE_DEF_PRESET_SIZE);
	g_config.curve_color = fp->exfunc->ini_load_int(fp, "curve_color", CVE_CURVE_COLOR_DEFAULT);
	g_config.notify_latest_version = fp->exfunc->ini_load_int(fp, "notify_latest_version", false);
}



//---------------------------------------------------------------------
//		aviutl.ini����ݒ����������
//---------------------------------------------------------------------
void ini_write_configs(FILTER* fp)
{
	fp->exfunc->ini_save_int(fp, "x1", g_curve_normal.ctpts[0].pt_right.x);
	fp->exfunc->ini_save_int(fp, "y1", g_curve_normal.ctpts[0].pt_right.y);
	fp->exfunc->ini_save_int(fp, "x2", g_curve_normal.ctpts[1].pt_left.x);
	fp->exfunc->ini_save_int(fp, "y2", g_curve_normal.ctpts[1].pt_left.y);
	fp->exfunc->ini_save_int(fp, "separator", g_config.separator);
	fp->exfunc->ini_save_int(fp, "edit_mode", g_config.edit_mode);
	fp->exfunc->ini_save_int(fp, "layout_mode", g_config.layout_mode);
	fp->exfunc->ini_save_int(fp, "align_handle", g_config.align_handle);
	fp->exfunc->ini_save_int(fp, "show_handle", g_config.show_handle);
}



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W��
//---------------------------------------------------------------------
BOOL filter_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, void* editp, FILTER* fp)
{
	RECT rect_wnd;
	::GetClientRect(hwnd, &rect_wnd);


	switch (msg) {
		// �E�B���h�E�쐬��
	case WM_FILTER_INIT:
		// WS_CLIPCHILDREN��ǉ�
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