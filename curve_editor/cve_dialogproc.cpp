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

	RECT rect_color_curve = {
		263, 64,
		291, 85
	};


	switch (msg) {
	case WM_CLOSE:
		::EndDialog(hwnd, 1);
		return 0;

	case WM_INITDIALOG:
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

		if (g_config.notify_latest_version)
			::SendMessage(
				::GetDlgItem(hwnd, IDC_LATEST_VERSION),
				BM_SETCHECK,
				BST_CHECKED, 0
			);

		if (g_config.auto_apply)
			::SendMessage(
				::GetDlgItem(hwnd, IDC_AUTO_APPLY),
				BM_SETCHECK,
				BST_CHECKED, 0
			);

		combo = ::GetDlgItem(hwnd, IDC_THEME);
		::SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)"ダーク");
		::SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)"ライト");
		::SendMessage(combo, CB_SETCURSEL, g_config.theme, 0);

		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = ::BeginPaint(hwnd, &ps);
		HBRUSH brush = ::CreateSolidBrush(g_config.curve_color);
		::SelectObject(hdc, brush);
		::Rectangle(hdc, rect_color_curve.left, rect_color_curve.top, rect_color_curve.right, rect_color_curve.bottom);
		::DeleteObject(brush);
		::EndPaint(hwnd, &ps);

		return 0;
	}

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
			g_config.notify_latest_version = ::SendMessage(
				::GetDlgItem(hwnd, IDC_LATEST_VERSION),
				BM_GETCHECK, 0, 0
			);
			g_config.auto_apply = ::SendMessage(
				::GetDlgItem(hwnd, IDC_AUTO_APPLY),
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
			g_fp->exfunc->ini_save_int(g_fp, "notify_latest_version", g_config.notify_latest_version);
			g_fp->exfunc->ini_save_int(g_fp, "auto_apply", g_config.auto_apply);

			::EndDialog(hwnd, 1);

			return 0;

		case IDCANCEL:
			::EndDialog(hwnd, 1);

			return 0;

		case IDC_CURVE_COLOR:
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

				g_curve_bezier.ctpts[0].pt_right.x = (int)(values[0] * CVE_GRAPH_RESOLUTION);
				g_curve_bezier.ctpts[0].pt_right.y = (int)(values[1] * CVE_GRAPH_RESOLUTION);
				g_curve_bezier.ctpts[1].pt_left.x = (int)(values[2] * CVE_GRAPH_RESOLUTION);
				g_curve_bezier.ctpts[1].pt_left.y = (int)(values[3] * CVE_GRAPH_RESOLUTION);

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
				catch (std::out_of_range&) {
					if (g_config.alert)
						MessageBox(hwnd, CVE_STR_ERROR_OUTOFRANGE, CVE_PLUGIN_NAME, MB_OK | MB_ICONERROR);

					return 0;
				}
				// -2147483647 ~  -12368443：ベジェが使用
				//   -12368442 ~         -1：IDが使用
				//           0             ：不使用
				//           1 ~   12368442：IDが使用
				//    12368443 ~ 2147483646：ベジェが使用
				//  2147483647             ：不使用

				// -2147483647 ~  -11213203：不使用
				//   -11213202 ~  -10211202：バウンスが使用
				//   -10211201 ~         -1：振動が使用
				//           0             ：不使用
				//           1 ~   10211201：振動が使用
				//    10211202 ~   11213202：バウンスが使用
				//    11213203 ~ 2147483647：不使用
				
				switch (g_config.edit_mode) {
				case cve::Mode_Bezier:
					if (!(ISINRANGEEQ(value, -2147483647, -12368443) || ISINRANGEEQ(value, 12368443, 2147483646))) {
						if (g_config.alert)
							::MessageBox(hwnd, CVE_STR_ERROR_OUTOFRANGE, CVE_PLUGIN_NAME, MB_OK | MB_ICONERROR);

						return 0;
					}
					g_curve_bezier.read_number(value);
					break;

				case cve::Mode_Multibezier:
					if (!(ISINRANGEEQ(value, -2147483647, -12368443) || ISINRANGEEQ(value, 12368443, 2147483646))) {
						if (g_config.alert)
							::MessageBox(hwnd, CVE_STR_ERROR_OUTOFRANGE, CVE_PLUGIN_NAME, MB_OK | MB_ICONERROR);

						return 0;
					}
					g_curve_mb[g_config.current_id.multibezier - 1].read_number(value);
					break;

				case cve::Mode_Value:

					break;

				case cve::Mode_Elastic:
					if (!(ISINRANGEEQ(value, -10211201, -1) || ISINRANGEEQ(value, 1, 10211201))) {
						if (g_config.alert)
							::MessageBox(hwnd, CVE_STR_ERROR_OUTOFRANGE, CVE_PLUGIN_NAME, MB_OK | MB_ICONERROR);

						return 0;
					}
					g_curve_elastic.read_number(value);
					break;

				case cve::Mode_Bounce:
					if (!(ISINRANGEEQ(value, -11213202, -10211202) || ISINRANGEEQ(value, 10211202, 11213202))) {
						if (g_config.alert)
							::MessageBox(hwnd, CVE_STR_ERROR_OUTOFRANGE, CVE_PLUGIN_NAME, MB_OK | MB_ICONERROR);

						return 0;
					}
					g_curve_bounce.read_number(value);
					break;
				}

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
			case cve::Mode_Bezier:
			{
				const cve::Preset<cve::Curve_Bezier> preset_normal;
				g_presets_normal_custom.PushBack(preset_normal);
				g_presets_normal_custom[g_presets_normal_custom.size - 1].initialize(g_window_preset_list.hwnd, g_curve_bezier, buffer);
				break;
			}

			case cve::Mode_Multibezier:
			{
				const cve::Preset<cve::Curve_Multibezier> preset_mb;
				g_presets_mb_custom.PushBack(preset_mb);
				g_presets_mb_custom[g_presets_mb_custom.size - 1].initialize(g_window_preset_list.hwnd, g_curve_mb[g_config.current_id.multibezier - 1], buffer);
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

			if (!ISINRANGEEQ(value, 1, CVE_CURVE_MAX)) {
				if (g_config.alert)
					::MessageBox(hwnd, CVE_STR_ERROR_OUTOFRANGE, CVE_PLUGIN_NAME, MB_OK | MB_ICONERROR);

				return 0;
			}

			::SendMessage(g_window_menu.hwnd, WM_COMMAND, CVE_CM_CHANGE_ID, (LPARAM)value);
			::EndDialog(hwnd, 1);
			return 0;
		case IDCANCEL:
			::EndDialog(hwnd, 1);
			return 0;
		}
	}
	return 0;
}



BOOL CALLBACK dialogproc_latest_version(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	DWORD thread_id;

	switch (msg) {
	case WM_INITDIALOG:
		::CreateThread(NULL, 0, cve::check_version, (LPVOID)hwnd, 0, &thread_id);
		return 0;
	}
	return 0;
}