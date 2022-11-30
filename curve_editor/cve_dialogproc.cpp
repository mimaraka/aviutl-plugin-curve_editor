//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (ダイアログプロシージャ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"

#define CVE_REGEX_VALUE					R"(^((\d+ *, *)|(\d*\.\d* *, *))((-?\d+ *, *)|(-?\d*\.\d* *, *))((\d+ *, *)|(\d*\.\d* *, *))((-?\d+ *)|(-?\d*\.\d* *))$)"
#define CVE_REGEX_FLOW_1				R"()"
#define CVE_REGEX_FLOW_2				R"(^\s*\[\s*(\{\s*"name"\s*:\s*".*"\s*,\s*"curve"\s*:\s*\[\s*(\s*-?\d\.?\d+\s*,){3}\s*-?\d\.?\d+\s*\]\s*\},)+\s*\{\s*"name"\s*:\s*".*"\s*,\s*"curve"\s*:\s*\[\s*(\s*-?\d\.?\d+\s*,){3}\s*-?\d\.?\d+\s*\]\s*\}\s*\]\s*$)"
#define CVE_REGEX_CEP					R"(^(\s*\{\s*".*"(\s*\[\s*-?\d?\.?\d+\s*,\s*-?\d?\.?\d+\s*\]\s*)+\s*\}\s*)+$)"



//---------------------------------------------------------------------
//		ダイアログプロシージャ（設定ダイアログ）
//---------------------------------------------------------------------
BOOL CALLBACK dialogproc_config(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static HWND combo;
	static COLORREF cust_colors[16];
	static cve::Button bt_color_curve;

	RECT color_curve = {
		306,
		63,
		363,
		86
	};

	POINT			pt_client = {
		GET_X_LPARAM(lparam),
		GET_Y_LPARAM(lparam)
	};

	switch (msg) {
	case WM_CLOSE:
		::EndDialog(hwnd, 1);
		return 0;

	case WM_INITDIALOG:
		bt_color_curve.initialize(
			hwnd,
			"DIALOG_CTRL_COLOR_CURVE",
			NULL,
			cve::Button::Null,
			NULL, NULL,
			nullptr,
			CVE_CT_COLOR_CURVE,
			color_curve,
			CVE_EDGE_ALL
		);
		::SendMessage(bt_color_curve.hwnd, WM_COMMAND, CVE_CM_CHANGE_COLOR, (LPARAM)g_config.curve_color);

		if (g_config.trace)
			::SendMessage(
				::GetDlgItem(hwnd, IDC_PREVIOUSCURVE),
				BM_SETCHECK,
				BST_CHECKED, 0
			);

		if (g_config.alert)
			::SendMessage(
				::GetDlgItem(hwnd, IDC_ALERT),
				BM_SETCHECK,
				BST_CHECKED, 0
			);

		if (g_config.auto_copy)
			::SendMessage(
				::GetDlgItem(hwnd, IDC_AUTOCOPY),
				BM_SETCHECK,
				BST_CHECKED, 0
			);

		combo = ::GetDlgItem(hwnd, IDC_THEME);
		::SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)"ダーク");
		::SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)"ライト");
		::SendMessage(combo, CB_SETCURSEL, g_config.theme, 0);

		return 0;

	case WM_SIZE:
		bt_color_curve.move(color_curve);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			g_config.trace = ::SendMessage(
				::GetDlgItem(hwnd, IDC_PREVIOUSCURVE),
				BM_GETCHECK, 0, 0
			);
			g_config.alert = ::SendMessage(
				::GetDlgItem(hwnd, IDC_ALERT),
				BM_GETCHECK, 0, 0
			);
			g_config.auto_copy = ::SendMessage(
				::GetDlgItem(hwnd, IDC_AUTOCOPY),
				BM_GETCHECK, 0, 0
			);
			g_config.theme = ::SendMessage(
				combo,
				CB_GETCURSEL, 0, 0
			);

			g_fp->exfunc->ini_save_int(g_fp, "theme", g_config.theme);
			g_fp->exfunc->ini_save_int(g_fp, "show_previous_curve", g_config.trace);
			g_fp->exfunc->ini_save_int(g_fp, "show_alerts", g_config.alert);
			g_fp->exfunc->ini_save_int(g_fp, "auto_copy", g_config.auto_copy);
			g_fp->exfunc->ini_save_int(g_fp, "curve_color", g_config.curve_color);

			::EndDialog(hwnd, 1);

			return 0;

		case IDCANCEL:
			::EndDialog(hwnd, 1);

			return 0;

		case CVE_CT_COLOR_CURVE:
		{
			CHOOSECOLOR cc;

			cc.lStructSize = sizeof(CHOOSECOLOR);
			cc.hwndOwner = hwnd;
			cc.rgbResult = g_config.curve_color;
			cc.lpCustColors = cust_colors;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			::ChooseColor(&cc);

			g_config.curve_color = cc.rgbResult;

			::InvalidateRect(hwnd, NULL, FALSE);
			::SendMessage(bt_color_curve.hwnd, WM_COMMAND, CVE_CM_CHANGE_COLOR, (LPARAM)g_config.curve_color);
		}
			return 0;
		}
	}
	return 0;
}



//---------------------------------------------------------------------
//		ダイアログプロシージャ（カーブのパラメータの設定）
//---------------------------------------------------------------------
BOOL CALLBACK dialogproc_param_normal(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	TCHAR buffer[30];
	std::regex re(CVE_REGEX_VALUE);

	switch (msg) {
	case WM_CLOSE:
		::EndDialog(hwnd, 1);
		return 0;

	case WM_INITDIALOG:
		::SetFocus(::GetDlgItem(hwnd, IDC_EDIT_VALUE));
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			::GetDlgItemText(hwnd, IDC_EDIT_VALUE, buffer, 30);
			if (std::regex_match(buffer, re)) {
				std::string str = buffer;
				std::vector<std::string> vec = cve::split(buffer, ',');

				float values[4];

				values[0] = MINMAX_LIMIT(std::stof(vec[0]), 0, 1);
				values[1] = MINMAX_LIMIT(
					std::stof(vec[1]),
					CVE_CURVE_VALUE_MIN_Y,
					CVE_CURVE_VALUE_MAX_Y
				);
				values[2] = MINMAX_LIMIT(std::stof(vec[2]), 0, 1);
				values[3] = MINMAX_LIMIT(
					std::stof(vec[3]),
					CVE_CURVE_VALUE_MIN_Y,
					CVE_CURVE_VALUE_MAX_Y
				);

				g_curve_normal.ctpts[0].pt_right.x = (int)(values[0] * CVE_GRAPH_RESOLUTION);
				g_curve_normal.ctpts[0].pt_right.y = (int)(values[1] * CVE_GRAPH_RESOLUTION);
				g_curve_normal.ctpts[1].pt_left.x = (int)(values[2] * CVE_GRAPH_RESOLUTION);
				g_curve_normal.ctpts[1].pt_left.y = (int)(values[3] * CVE_GRAPH_RESOLUTION);

				::EndDialog(hwnd, 1);
			}
			else if (g_config.alert)
				::MessageBox(hwnd, CVE_STR_ERROR_INPUT_INVALID, CVE_PLUGIN_NAME, MB_OK | MB_ICONINFORMATION);

			return 0;

		case IDCANCEL:
			::EndDialog(hwnd, 1);
			return 0;
		}
	}
	return 0;
}



//---------------------------------------------------------------------
//		ダイアログプロシージャ(カーブ読み取り)
//---------------------------------------------------------------------
BOOL CALLBACK dialogproc_read(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// エディットコントロール
	HWND edit;
	TCHAR buffer[12];
	int value;
	// 値の正規表現
	std::regex re(R"(^-?\d+$)");
	std::string str;

	switch (msg) {
	case WM_INITDIALOG:
		edit = ::GetDlgItem(hwnd, IDC_EDIT_READ);
		::SendMessage(edit, EM_SETLIMITTEXT, 11, 0);
		::SetFocus(edit);

		return 0;

	case WM_CLOSE:
		::EndDialog(hwnd, 1);
		return 0;

	case WM_KEYDOWN:
		if (wparam == VK_RETURN)
			::SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDOK, 0), 0);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			::GetDlgItemText(hwnd, IDC_EDIT_READ, buffer, 11);
			if (std::regex_match(buffer, re)) {
				str = buffer;
				try {
					value = std::stoi(str);
				}
				catch (std::out_of_range& e) {
					if (g_config.alert)
						MessageBox(hwnd, CVE_STR_ERROR_OUTOFRANGE, CVE_PLUGIN_NAME, MB_OK | MB_ICONERROR);

					return 0;
				}
				if (!ISINRANGEEQ(value, -2147483647, 2122746761)) {
					if (g_config.alert)
						::MessageBox(hwnd, CVE_STR_ERROR_OUTOFRANGE, CVE_PLUGIN_NAME, MB_OK | MB_ICONERROR);

					return 0;
				}
				if (g_config.edit_mode == cve::Mode_Normal)
					g_curve_normal.read_number(value);
				else
					g_curve_mb[g_config.current_id.multibezier].read_number(value);

				::EndDialog(hwnd, 1);
			}
			else if (g_config.alert)
				::MessageBox(hwnd, CVE_STR_ERROR_INPUT_INVALID, CVE_PLUGIN_NAME, MB_OK | MB_ICONERROR);

			return 0;
		case IDCANCEL:
			::EndDialog(hwnd, 1);
			return 0;
		}
	}
	return 0;
}



//---------------------------------------------------------------------
//		ダイアログプロシージャ（カーブ保存ダイアログ）
//---------------------------------------------------------------------
BOOL CALLBACK dialogproc_save(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	HWND edit;
	char buffer[CVE_PRESET_NAME_MAX] = "";

	switch (msg) {
	case WM_CLOSE:
		::EndDialog(hwnd, 1);
		return 0;

	case WM_INITDIALOG:
		edit = ::GetDlgItem(hwnd, IDC_EDIT_SAVE);
		::SendMessage(edit, EM_SETLIMITTEXT, CVE_PRESET_NAME_MAX, 0);
		::SetFocus(edit);

		return 0;

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
		{
			::GetDlgItemText(hwnd, IDC_EDIT_SAVE, buffer, CVE_PRESET_NAME_MAX);

			switch (g_config.edit_mode) {
			case cve::Mode_Normal:
			{
				const cve::Preset<cve::Curve_Normal> preset_normal;
				g_presets_normal_custom.PushBack(preset_normal);
				g_presets_normal_custom[g_presets_normal_custom.size - 1].initialize(g_window_preset_list.hwnd, g_curve_normal, buffer);
				break;
			}

			case cve::Mode_Multibezier:
			{
				const cve::Preset<cve::Curve_Multibezier> preset_mb;
				g_presets_mb_custom.PushBack(preset_mb);
				g_presets_mb_custom[g_presets_mb_custom.size - 1].initialize(g_window_preset_list.hwnd, g_curve_mb[g_config.current_id.multibezier], buffer);
				break;
			}

			case cve::Mode_Elastic:
			{
				const cve::Preset<cve::Curve_Elastic> preset_elastic;
				g_presets_elastic_custom.PushBack(preset_elastic);
				g_presets_elastic_custom[g_presets_elastic_custom.size - 1].initialize(g_window_preset_list.hwnd, g_curve_elastic, buffer);
				break;
			}

			case cve::Mode_Bounce:
			{
				const cve::Preset<cve::Curve_Bounce> preset_bounce;
				g_presets_bounce_custom.PushBack(preset_bounce);
				g_presets_bounce_custom[g_presets_bounce_custom.size - 1].initialize(g_window_preset_list.hwnd, g_curve_bounce, buffer);
				break;
			}

			default:
				break;
			}

			::SendMessage(g_window_preset_list.hwnd, WM_COMMAND, CVE_CM_PRESET_MOVE, 0);

			::EndDialog(hwnd, 1);

			return 0;
		}

		case IDCANCEL:
			::EndDialog(hwnd, 1);

			return 0;
		}
		return 0;
	}
	return 0;
}



//---------------------------------------------------------------------
//		ダイアログプロシージャ（指定したIDに移動）
//---------------------------------------------------------------------
BOOL CALLBACK dialogproc_id(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// エディットコントロール
	HWND edit;
	TCHAR buffer[5];
	int value;
	std::string str;

	switch (msg) {
	case WM_INITDIALOG:
		edit = ::GetDlgItem(hwnd, IDC_EDIT_ID);
		::SendMessage(edit, EM_SETLIMITTEXT, 4, 0);
		::SetFocus(edit);
		return 0;

	case WM_CLOSE:
		::EndDialog(hwnd, 1);
		return 0;

	case WM_KEYDOWN:
		if (wparam == VK_RETURN)
			::SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDOK, 0), 0);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			::GetDlgItemText(hwnd, IDC_EDIT_ID, buffer, 5);

			if (strlen(buffer) == 0) {
				if (g_config.alert)
					::MessageBox(hwnd, CVE_STR_ERROR_INPUT_INVALID, CVE_PLUGIN_NAME, MB_OK | MB_ICONERROR);

				return 0;
			}

			str = buffer;
			value = std::stoi(str);

			if (!ISINRANGEEQ(value, 0, CVE_CURVE_MAX - 1)) {
				if (g_config.alert)
					::MessageBox(hwnd, CVE_STR_ERROR_OUTOFRANGE, CVE_PLUGIN_NAME, MB_OK | MB_ICONERROR);

				return 0;
			}

			::SendMessage(g_window_header.hwnd, WM_COMMAND, CVE_CM_CHANGE_ID, (LPARAM)value);
			::EndDialog(hwnd, 1);
			return 0;
		case IDCANCEL:
			::EndDialog(hwnd, 1);
			return 0;
		}
	}
	return 0;
}