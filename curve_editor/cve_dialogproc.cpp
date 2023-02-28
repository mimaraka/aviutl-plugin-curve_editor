//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (ダイアログプロシージャ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"

#define CVE_REGEX_FLOW_1				R"()"
#define CVE_REGEX_FLOW_2				R"(^\s*\[\s*(\{\s*"name"\s*:\s*".*"\s*,\s*"curve"\s*:\s*\[\s*(\s*-?\d\.?\d+\s*,){3}\s*-?\d\.?\d+\s*\]\s*\},)+\s*\{\s*"name"\s*:\s*".*"\s*,\s*"curve"\s*:\s*\[\s*(\s*-?\d\.?\d+\s*,){3}\s*-?\d\.?\d+\s*\]\s*\}\s*\]\s*$)"
#define CVE_REGEX_CEP					R"(^(\s*\{\s*".*"(\s*\[\s*-?\d?\.?\d+\s*,\s*-?\d?\.?\d+\s*\]\s*)+\s*\}\s*)+$)"



//---------------------------------------------------------------------
//		ダイアログプロシージャ（設定ダイアログ）
//---------------------------------------------------------------------
void load_configs(HWND hwnd, HWND* combo_theme)
{
	if (g_config.trace)
		::SendMessage(
			::GetDlgItem(hwnd, IDC_PREVIOUSCURVE),
			BM_SETCHECK,
			BST_CHECKED, 0
		);

	if (g_config.show_popup)
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

	if (g_config.notify_update)
		::SendMessage(
			::GetDlgItem(hwnd, IDC_NOTIFY_UPDATE),
			BM_SETCHECK,
			BST_CHECKED, 0
		);

	if (g_config.reverse_wheel)
		::SendMessage(
			::GetDlgItem(hwnd, IDC_REVERSE_WHEEL),
			BM_SETCHECK,
			BST_CHECKED, 0
		);

	if (g_config.auto_apply)
		::SendMessage(
			::GetDlgItem(hwnd, IDC_AUTO_APPLY),
			BM_SETCHECK,
			BST_CHECKED, 0
		);

	if (g_config.linearize)
		::SendMessage(
			::GetDlgItem(hwnd, IDC_LINEARIZE),
			BM_SETCHECK,
			BST_CHECKED, 0
		);

	*combo_theme = ::GetDlgItem(hwnd, IDC_THEME);
	if (!::SendMessage(*combo_theme, CB_GETCOUNT, 0, 0)) {
		::SendMessage(*combo_theme, CB_ADDSTRING, 0, (LPARAM)"ダーク");
		::SendMessage(*combo_theme, CB_ADDSTRING, 0, (LPARAM)"ライト");
	}
	::SendMessage(*combo_theme, CB_SETCURSEL, g_config.theme, 0);
}

void write_configs(HWND hwnd, HWND combo_theme)
{
	g_config.trace = ::SendMessage(
		::GetDlgItem(hwnd, IDC_PREVIOUSCURVE),
		BM_GETCHECK, 0, 0
	);
	g_config.show_popup = ::SendMessage(
		::GetDlgItem(hwnd, IDC_ALERT),
		BM_GETCHECK, 0, 0
	);
	g_config.auto_copy = ::SendMessage(
		::GetDlgItem(hwnd, IDC_AUTOCOPY),
		BM_GETCHECK, 0, 0
	);
	g_config.notify_update = ::SendMessage(
		::GetDlgItem(hwnd, IDC_NOTIFY_UPDATE),
		BM_GETCHECK, 0, 0
	);
	g_config.reverse_wheel = ::SendMessage(
		::GetDlgItem(hwnd, IDC_REVERSE_WHEEL),
		BM_GETCHECK, 0, 0
	);
	g_config.auto_apply = ::SendMessage(
		::GetDlgItem(hwnd, IDC_AUTO_APPLY),
		BM_GETCHECK, 0, 0
	);
	g_config.linearize = ::SendMessage(
		::GetDlgItem(hwnd, IDC_LINEARIZE),
		BM_GETCHECK, 0, 0
	);
	g_config.theme = ::SendMessage(
		combo_theme,
		CB_GETCURSEL, 0, 0
	);

	// テーマ
	g_fp->exfunc->ini_save_int(g_fp,
		CVE_INI_KEY_THEME,
		g_config.theme
	);
	// 1ステップ前のカーブを表示
	g_fp->exfunc->ini_save_int(g_fp,
		CVE_INI_KEY_SHOW_TRACE,
		g_config.trace
	);
	// ポップアップを表示
	g_fp->exfunc->ini_save_int(g_fp,
		CVE_INI_KEY_SHOW_POPUP,
		g_config.show_popup
	);
	// 自動コピー
	g_fp->exfunc->ini_save_int(g_fp,
		CVE_INI_KEY_AUTO_COPY,
		g_config.auto_copy
	);
	// カーブの色
	g_fp->exfunc->ini_save_int(g_fp,
		CVE_INI_KEY_CURVE_COLOR,
		g_config.curve_color.colorref()
	);
	// アップデートを通知
	g_fp->exfunc->ini_save_int(g_fp,
		CVE_INI_KEY_NOTIFY_UPDATE,
		g_config.notify_update
	);
	// ホイールを反転
	g_fp->exfunc->ini_save_int(g_fp,
		CVE_INI_KEY_REVERSE_WHEEL,
		g_config.reverse_wheel
	);
	// 自動で適用
	g_fp->exfunc->ini_save_int(g_fp,
		CVE_INI_KEY_AUTO_APPLY,
		g_config.auto_apply
	);
	// ベジェを直線にする
	g_fp->exfunc->ini_save_int(g_fp,
		CVE_INI_KEY_LINEARIZE,
		g_config.linearize
	);
}

BOOL CALLBACK dialogproc_config(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static HWND combo_theme;
	static COLORREF cust_colors[16];

	RECT rect_color_curve = {
		477, 65,
		510, 85
	};

	switch (msg) {
	case WM_CLOSE:
		::EndDialog(hwnd, 1);
		return 0;

	case WM_INITDIALOG:
		::load_configs(hwnd, &combo_theme);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = ::BeginPaint(hwnd, &ps);
		HBRUSH brush = ::CreateSolidBrush(g_config.curve_color.colorref());
		::SelectObject(hdc, brush);
		::Rectangle(hdc, rect_color_curve.left, rect_color_curve.top, rect_color_curve.right, rect_color_curve.bottom);
		::DeleteObject(brush);
		::EndPaint(hwnd, &ps);

		return 0;
	}

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			::write_configs(hwnd, combo_theme);

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
			cc.rgbResult = g_config.curve_color.colorref();
			cc.lpCustColors = cust_colors;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			::ChooseColor(&cc);

			g_config.curve_color = cc.rgbResult;

			::InvalidateRect(hwnd, NULL, FALSE);
			return 0;
		}
		
		case IDC_RESET_CONFIGS:
		{
			int response = IDOK;
			if (g_config.show_popup) {
				response = ::MessageBox(
					hwnd,
					CVE_STR_WARNING_RESET_CONFIGS,
					CVE_FILTER_NAME,
					MB_OKCANCEL | MB_ICONWARNING
				);
			}
			if (response == IDOK) {
				g_config.reset_configs();
				::load_configs(hwnd, &combo_theme);
				::write_configs(hwnd, combo_theme);
				::InvalidateRect(hwnd, NULL, FALSE);
			}
			return 0;
		}
		}
	}
	return 0;
}



//---------------------------------------------------------------------
//		ダイアログプロシージャ（ベジェのパラメータの設定）
//---------------------------------------------------------------------
BOOL CALLBACK dialogproc_bezier_param(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	constexpr int MAX_INPUT = 256;
	TCHAR buffer[MAX_INPUT];

	switch (msg) {
	case WM_CLOSE:
		::EndDialog(hwnd, 1);
		return 0;

	case WM_INITDIALOG:
	{
		HWND edit = ::GetDlgItem(hwnd, IDC_EDIT_VALUE);
		::SendMessage(edit, EM_SETLIMITTEXT, MAX_INPUT - 1, 0);
		::SetFocus(edit);

		return 0;
	}

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			::GetDlgItemText(hwnd, IDC_EDIT_VALUE, buffer, MAX_INPUT);

			if (g_curve_bezier.read_parameters(buffer))
				::EndDialog(hwnd, 1);
			else if (g_config.show_popup)
				::MessageBox(hwnd, CVE_STR_ERROR_INPUT_INVALID, CVE_FILTER_NAME, MB_OK | MB_ICONINFORMATION);

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
					if (g_config.show_popup)
						MessageBox(hwnd, CVE_STR_ERROR_OUTOFRANGE, CVE_FILTER_NAME, MB_OK | MB_ICONERROR);

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
					if (!(aului::in_range(value, -2147483647, -12368443, true) || aului::in_range(value, 12368443, 2147483646, true))) {
						if (g_config.show_popup)
							::MessageBox(hwnd, CVE_STR_ERROR_OUTOFRANGE, CVE_FILTER_NAME, MB_OK | MB_ICONERROR);

						return 0;
					}
					g_curve_bezier.read_number(value);
					break;

				case cve::Mode_Bezier_Multi:
					if (!(aului::in_range(value, -2147483647, -12368443, true) || aului::in_range(value, 12368443, 2147483646, true))) {
						if (g_config.show_popup)
							::MessageBox(hwnd, CVE_STR_ERROR_OUTOFRANGE, CVE_FILTER_NAME, MB_OK | MB_ICONERROR);

						return 0;
					}
					g_curve_bezier_multi[g_config.current_id.multi - 1].read_number(value);
					break;

				case cve::Mode_Elastic:
					if (!(aului::in_range(value, -10211201, -1, true) || aului::in_range(value, 1, 10211201, true))) {
						if (g_config.show_popup)
							::MessageBox(hwnd, CVE_STR_ERROR_OUTOFRANGE, CVE_FILTER_NAME, MB_OK | MB_ICONERROR);

						return 0;
					}
					g_curve_elastic.read_number(value);
					break;

				case cve::Mode_Bounce:
					if (!(aului::in_range(value, -11213202, -10211202, true) || aului::in_range(value, 10211202, 11213202, true))) {
						if (g_config.show_popup)
							::MessageBox(hwnd, CVE_STR_ERROR_OUTOFRANGE, CVE_FILTER_NAME, MB_OK | MB_ICONERROR);

						return 0;
					}
					g_curve_bounce.read_number(value);
					break;
				}

				::EndDialog(hwnd, 1);
			}
			else if (g_config.show_popup)
				::MessageBox(hwnd, CVE_STR_ERROR_INPUT_INVALID, CVE_FILTER_NAME, MB_OK | MB_ICONERROR);

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
				const cve::Preset<cve::Curve_Bezier> preset_bezier;
				g_presets_bezier_custom.PushBack(preset_bezier);
				g_presets_bezier_custom[g_presets_bezier_custom.size - 1].init(g_window_preset_list.hwnd, g_curve_bezier, buffer);
				break;
			}

			case cve::Mode_Bezier_Multi:
			{
				const cve::Preset<cve::Curve_Bezier_Multi> preset_bezier_multi;
				g_presets_bezier_multi_custom.PushBack(preset_bezier_multi);
				g_presets_bezier_multi_custom[g_presets_bezier_multi_custom.size - 1].init(g_window_preset_list.hwnd, g_curve_bezier_multi[g_config.current_id.multi - 1], buffer);
				break;
			}

			case cve::Mode_Elastic:
			{
				const cve::Preset<cve::Curve_Elastic> preset_elastic;
				g_presets_elastic_custom.PushBack(preset_elastic);
				g_presets_elastic_custom[g_presets_elastic_custom.size - 1].init(g_window_preset_list.hwnd, g_curve_elastic, buffer);
				break;
			}

			case cve::Mode_Bounce:
			{
				const cve::Preset<cve::Curve_Bounce> preset_bounce;
				g_presets_bounce_custom.PushBack(preset_bounce);
				g_presets_bounce_custom[g_presets_bounce_custom.size - 1].init(g_window_preset_list.hwnd, g_curve_bounce, buffer);
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

			if (::strlen(buffer) == 0) {
				if (g_config.show_popup)
					::MessageBox(hwnd, CVE_STR_ERROR_INPUT_INVALID, CVE_FILTER_NAME, MB_OK | MB_ICONERROR);

				return 0;
			}

			str = buffer;
			value = std::stoi(str);

			if (!aului::in_range(value, 1, CVE_CURVE_MAX, true)) {
				if (g_config.show_popup)
					::MessageBox(hwnd, CVE_STR_ERROR_OUTOFRANGE, CVE_FILTER_NAME, MB_OK | MB_ICONERROR);

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



//---------------------------------------------------------------------
//		ダイアログプロシージャ（Curve Editorについて）
//---------------------------------------------------------------------
BOOL CALLBACK dialogproc_about(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case WM_CLOSE:
		::EndDialog(hwnd, 1);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = ::BeginPaint(hwnd, &ps);
		HFONT font_title = ::CreateFont(
			64, 0,
			0, 0,
			FW_REGULAR,
			FALSE, FALSE, FALSE,
			SHIFTJIS_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			NULL,
			CVE_FONT_SEMIBOLD
		);
		HFONT font_description = ::CreateFont(
			18, 0,
			0, 0,
			FW_REGULAR,
			FALSE, FALSE, FALSE,
			SHIFTJIS_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			NULL,
			CVE_FONT_REGULAR
		);
		RECT rect_title = {
			10, 0,
			300, 60
		};

		RECT rect_description = {
			10, 65,
			300, 110
		};
		LPCTSTR version = "バージョン : " CVE_FILTER_VERSION "\n" "Developed by " CVE_FILTER_DEVELOPER;

		::SelectObject(hdc, font_title);
		::SetBkMode(hdc, TRANSPARENT);
		::DrawText(hdc, CVE_FILTER_NAME, ::strlen(CVE_FILTER_NAME), &rect_title, DT_SINGLELINE);
		::DeleteObject(font_title);
		::SelectObject(hdc, font_description);
		::DrawText(hdc, version, ::strlen(version), &rect_description, NULL);
		::DeleteObject(font_description);
		::EndPaint(hwnd, &ps);

		return 0;
	}

	case WM_COMMAND:
		switch (wparam) {
		case IDOK:
			::EndDialog(hwnd, 1);
			return 0;

		case IDC_ABOUT_LICENSE:
			::ShellExecute(NULL, "open", CVE_FILTER_LINK_LICENSE, NULL, NULL, SW_SHOWNORMAL);
			return 0;

		case IDC_CHECK_UPDATE:
			::DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_LATEST_VERSION), hwnd, dialogproc_check_update);
			return 0;

		case IDC_REPORT_BUGS:
			::ShellExecute(NULL, "open", CVE_FILTER_LINK_FORM, NULL, NULL, SW_SHOWNORMAL);
		}
	}
	return 0;
}



//---------------------------------------------------------------------
//		ダイアログプロシージャ（アップデートをチェック）
//---------------------------------------------------------------------
BOOL CALLBACK dialogproc_check_update(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	DWORD thread_id;

	switch (msg) {
	case WM_INITDIALOG:
		::CreateThread(NULL, 0, cve::check_version, (LPVOID)hwnd, 0, &thread_id);
		return 0;
	}
	return 0;
}