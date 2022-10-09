//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (ダイアログプロシージャ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		ダイアログプロシージャ（設定ダイアログ）
//---------------------------------------------------------------------
BOOL CALLBACK dialogproc_settings(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static HDC hdc, hdc_mem;
	static HWND hCombo, hCombo2;
	static HBITMAP bitmap;
	switch (msg) {
	case WM_CLOSE:
		DeleteObject(bitmap);
		EndDialog(hwnd, 1);
		return 0;

	case WM_INITDIALOG:
		if (g_config.trace) SendMessage(GetDlgItem(hwnd, IDC_PREVIOUSCURVE), BM_SETCHECK, BST_CHECKED, 0);
		if (g_config.alert) SendMessage(GetDlgItem(hwnd, IDC_ALERT), BM_SETCHECK, BST_CHECKED, 0);
		if (g_config.auto_copy) SendMessage(GetDlgItem(hwnd, IDC_AUTOCOPY), BM_SETCHECK, BST_CHECKED, 0);
		hCombo = GetDlgItem(hwnd, IDC_THEME);
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"ダーク");
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"ライト");
		SendMessage(hCombo, CB_SETCURSEL, g_config.theme, 0);

		return 0;

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			g_config.trace = SendMessage(GetDlgItem(hwnd, IDC_PREVIOUSCURVE), BM_GETCHECK, 0, 0);
			g_config.alert = SendMessage(GetDlgItem(hwnd, IDC_ALERT), BM_GETCHECK, 0, 0);
			g_config.auto_copy = SendMessage(GetDlgItem(hwnd, IDC_AUTOCOPY), BM_GETCHECK, 0, 0);
			g_config.theme = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
			g_fp->exfunc->ini_save_int(g_fp, "theme", g_config.theme);
			g_fp->exfunc->ini_save_int(g_fp, "show_previous_curve", g_config.trace);
			g_fp->exfunc->ini_save_int(g_fp, "show_alerts", g_config.alert);
			g_fp->exfunc->ini_save_int(g_fp, "auto_copy", g_config.auto_copy);
			EndDialog(hwnd, 1);
			return 0;

		case IDCANCEL:
			EndDialog(hwnd, 1);
			return 0;
		}
	}
	return 0;
}



//---------------------------------------------------------------------
//		ダイアログプロシージャ（カーブ値の設定）
//---------------------------------------------------------------------
BOOL CALLBACK dialogproc_value(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	TCHAR buffer[30];
	std::regex re(R"(^((\d+ *, *)|(\d*\.\d* *, *))((-?\d+ *, *)|(-?\d*\.\d* *, *))((\d+ *, *)|(\d*\.\d* *, *))((-?\d+ *)|(-?\d*\.\d* *))$)");

	switch (msg) {
	case WM_CLOSE:
		EndDialog(hwnd, 1);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			GetDlgItemText(hwnd, IDC_EDIT_VALUE, buffer, 30);
			if (std::regex_match(buffer, re)) {
				std::string str = buffer;
				std::vector<std::string> vec = split(buffer, ',');

				g_curve_value.ctpt[0].x = (int)(std::stod(vec[0]) * CE_GR_RESOLUTION);
				g_curve_value.ctpt[0].x = (int)(std::stod(vec[0]) * CE_GR_RESOLUTION);
				g_curve_value.ctpt[0].y = (int)(std::stod(vec[1]) * CE_GR_RESOLUTION);
				g_curve_value.ctpt[1].x = (int)(std::stod(vec[2]) * CE_GR_RESOLUTION);
				g_curve_value.ctpt[1].y = (int)(std::stod(vec[3]) * CE_GR_RESOLUTION);

				for (int i = 0; i < 2; i++) {
					if (g_curve_value.ctpt[i].y > 3.73 * CE_GR_RESOLUTION) g_curve_value.ctpt[i].y = (int)(3.73 * CE_GR_RESOLUTION);
					else if (g_curve_value.ctpt[i].y < -2.73 * CE_GR_RESOLUTION) g_curve_value.ctpt[i].y = (int)(-2.73 * CE_GR_RESOLUTION);
				}
				EndDialog(hwnd, 1);
			}
			else if (g_config.alert)MessageBox(hwnd, CE_STR_INVALIDINPUT, CE_PLUGIN_NAME, MB_OK | MB_ICONINFORMATION);
			return 0;

		case IDCANCEL:
			EndDialog(hwnd, 1);
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
		edit = GetDlgItem(hwnd, IDC_EDIT_READ);
		SendMessage(edit, EM_SETLIMITTEXT, 11, 0);
		return 0;

	case WM_CLOSE:
		EndDialog(hwnd, 1);
		return 0;

	case WM_KEYDOWN:
		if (wparam == VK_RETURN)
			SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDOK, 0), 0);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			GetDlgItemText(hwnd, IDC_EDIT_READ, buffer, 11);
			if (std::regex_match(buffer, re)) {
				str = buffer;
				try {
					value = std::stoi(str);
				}
				catch (std::out_of_range& e) {
					if (g_config.alert) MessageBox(hwnd, CE_STR_OUTOFRANGE, CE_PLUGIN_NAME, MB_OK | MB_ICONERROR);
					return 0;
				}
				if ((value < -2147483647 || 2122746761 < value) && g_config.alert) {
					MessageBox(hwnd, CE_STR_OUTOFRANGE, CE_PLUGIN_NAME, MB_OK | MB_ICONERROR);
					return 0;
				}
				g_curve_value.read_value_1d(value);
				EndDialog(hwnd, 1);
			}
			else if (g_config.alert)
				MessageBox(hwnd, CE_STR_INVALIDINPUT, CE_PLUGIN_NAME, MB_OK | MB_ICONERROR);
			return 0;
		case IDCANCEL:
			EndDialog(hwnd, 1);
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
	//4次元カーブ!Dを生成
	HDC hdc;
	static HFONT hfValues;
	HWND edit;
	LPCTSTR lpsResult;
	std::string strBuffer;
	//TCHAR buffer[64];
	strBuffer = g_curve_value.create_value_4d();
	lpsResult = strBuffer.c_str();

	switch (msg) {
	case WM_CLOSE:
		EndDialog(hwnd, 1);
		return 0;
	case WM_INITDIALOG:
		edit = GetDlgItem(hwnd, IDC_EDIT_SAVE);
		SendMessage(edit, EM_SETLIMITTEXT, 64, 0);
		return 0;
	case WM_PAINT:
		hfValues = CreateFont(
			16, 0,
			0, 0,
			FW_NORMAL,
			FALSE, FALSE, FALSE,
			ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			0,
			NULL
		);
		hdc = GetDC(hwnd);
		SelectObject(hdc, hfValues);
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, 100, 13, lpsResult, strlen(lpsResult));
		DeleteObject(hfValues);
		ReleaseDC(hwnd, hdc);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			/*ce::Preset additem;
			GetDlgItemText(hwnd, IDC_EDIT_SAVE, buffer, 64);
			if (strlen(buffer) < 64 && strlen(buffer) != 0) {
				additem = { buffer, g_curve_value.ctpt[0].x, g_curve_value.ctpt[0].y, g_curve_value.ctpt[1].x, g_curve_value.ctpt[1].y };
				g_presets.emplace_back(additem);
				EndDialog(hwnd, 1);
			}
			else if (strlen(buffer) == 0 && g_config.alert) MessageBox(hwnd, CE_STR_INPUTANAME, CE_PLUGIN_NAME, MB_OK | MB_ICONINFORMATION);*/
			return 0;
		case IDCANCEL:
			EndDialog(hwnd, 1);
			return 0;
		}
		return 0;
	}
	return 0;
}