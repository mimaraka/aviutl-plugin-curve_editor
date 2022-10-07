//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (ダイアログのプロシージャ)
//		VC++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		ダイアログプロシージャ（設定ダイアログ）
//---------------------------------------------------------------------
BOOL CALLBACK wndproc_daialog_settings(HWND hDlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static HDC hdc, hdc_mem;
	static HWND hCombo, hCombo2;
	static HBITMAP bitmap;
	switch (msg) {
	case WM_CLOSE:
		DeleteObject(bitmap);
		EndDialog(hDlg, 1);
		return 0;

	case WM_INITDIALOG:
		if (g_config.trace) SendMessage(GetDlgItem(hDlg, IDC_PREVIOUSCURVE), BM_SETCHECK, BST_CHECKED, 0);
		if (g_config.alert) SendMessage(GetDlgItem(hDlg, IDC_ALERT), BM_SETCHECK, BST_CHECKED, 0);
		if (g_config.auto_copy) SendMessage(GetDlgItem(hDlg, IDC_AUTOCOPY), BM_SETCHECK, BST_CHECKED, 0);
		hCombo = GetDlgItem(hDlg, IDC_THEME);
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"ダーク");
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"ライト");
		SendMessage(hCombo, CB_SETCURSEL, g_config.theme, 0);

		return 0;

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			g_config.trace = SendMessage(GetDlgItem(hDlg, IDC_PREVIOUSCURVE), BM_GETCHECK, 0, 0);
			g_config.alert = SendMessage(GetDlgItem(hDlg, IDC_ALERT), BM_GETCHECK, 0, 0);
			g_config.auto_copy = SendMessage(GetDlgItem(hDlg, IDC_AUTOCOPY), BM_GETCHECK, 0, 0);
			g_config.theme = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
			g_fp->exfunc->ini_save_int(g_fp, "theme", g_config.theme);
			g_fp->exfunc->ini_save_int(g_fp, "show_previous_curve", g_config.trace);
			g_fp->exfunc->ini_save_int(g_fp, "show_alerts", g_config.alert);
			g_fp->exfunc->ini_save_int(g_fp, "auto_copy", g_config.auto_copy);
			EndDialog(hDlg, 1);
			return 0;

		case IDCANCEL:
			EndDialog(hDlg, 1);
			return 0;
		}
	}
	return 0;
}



//---------------------------------------------------------------------
//		ダイアログプロシージャ（カーブ値の設定）
//---------------------------------------------------------------------
BOOL CALLBACK wndproc_daialog_value(HWND hDlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
	TCHAR buffer[30];
	std::regex re(R"(^((\d+ *, *)|(\d*\.\d* *, *))((-?\d+ *, *)|(-?\d*\.\d* *, *))((\d+ *, *)|(\d*\.\d* *, *))((-?\d+ *)|(-?\d*\.\d* *))$)");
	switch (msg) {
	case WM_CLOSE:
		EndDialog(hDlg, 1);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			GetDlgItemText(hDlg, IDC_EDIT_VALUE, buffer, 30);
			if (std::regex_match(buffer, re)) {
				std::string str = buffer;
				std::vector<std::string> vec = split(buffer, ',');
				g_curve_value.ctpt[0].x = (int)(std::stod(vec[0]) * 1000);
				g_curve_value.ctpt[0].x = (int)(std::stod(vec[0]) * 1000);
				g_curve_value.ctpt[0].y = (int)(std::stod(vec[1]) * 1000);
				g_curve_value.ctpt[1].x = (int)(std::stod(vec[2]) * 1000);
				g_curve_value.ctpt[1].y = (int)(std::stod(vec[3]) * 1000);

				for (int i = 0; i < 2; i++) {
					if (g_curve_value.ctpt[i].y > 3730) g_curve_value.ctpt[i].y = 3730;
					else if (g_curve_value.ctpt[i].y < -2730) g_curve_value.ctpt[i].y = -2730;
				}
				EndDialog(hDlg, 1);
			}
			else if (g_config.alert)MessageBox(hDlg, CE_STR_INVALIDINPUT, CE_PLUGIN_NAME, MB_OK | MB_ICONINFORMATION);
			return 0;
		case IDCANCEL:
			EndDialog(hDlg, 1);
			return 0;
		}
	}
	return 0;
}



//---------------------------------------------------------------------
//		ダイアログプロシージャ(カーブ読み取り)
//---------------------------------------------------------------------
BOOL CALLBACK wndproc_daialog_read(HWND hDlg, UINT msg, WPARAM wparam, LPARAM lparam)
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
		edit = GetDlgItem(hDlg, IDC_EDIT_READ);
		SendMessage(edit, EM_SETLIMITTEXT, 11, 0);
		return 0;

	case WM_CLOSE:
		EndDialog(hDlg, 1);
		return 0;

	case WM_KEYDOWN:
		if (wparam == VK_RETURN)
			SendMessage(hDlg, WM_COMMAND, MAKEWPARAM(IDOK, 0), 0);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			GetDlgItemText(hDlg, IDC_EDIT_READ, buffer, 11);
			if (std::regex_match(buffer, re)) {
				str = buffer;
				try {
					value = std::stoi(str);
				}
				catch (std::out_of_range& e) {
					if (g_config.alert) MessageBox(hDlg, CE_STR_OUTOFRANGE, CE_PLUGIN_NAME, MB_OK | MB_ICONERROR);
					return 0;
				}
				if ((value < -2147483647 || 2122746761 < value) && g_config.alert) {
					MessageBox(hDlg, CE_STR_OUTOFRANGE, CE_PLUGIN_NAME, MB_OK | MB_ICONERROR);
					return 0;
				}
				g_curve_value.read_value_1d(value);
				EndDialog(hDlg, 1);
			}
			else if (g_config.alert)
				MessageBox(hDlg, CE_STR_INVALIDINPUT, CE_PLUGIN_NAME, MB_OK | MB_ICONERROR);
			return 0;
		case IDCANCEL:
			EndDialog(hDlg, 1);
			return 0;
		}
	}
	return 0;
}



//---------------------------------------------------------------------
//		ダイアログプロシージャ（カーブ保存ダイアログ）
//---------------------------------------------------------------------
BOOL CALLBACK wndproc_daialog_save(HWND hDlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//4次元カーブ!Dを生成
	HDC hdc;
	static HFONT hfValues;
	HWND edit;
	LPCTSTR lpsResult;
	std::string strBuffer;
	TCHAR buffer[64];
	strBuffer = g_curve_value.create_value_4d();
	lpsResult = strBuffer.c_str();

	switch (msg) {
	case WM_CLOSE:
		EndDialog(hDlg, 1);
		return 0;
	case WM_INITDIALOG:
		edit = GetDlgItem(hDlg, IDC_EDIT_SAVE);
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
		hdc = GetDC(hDlg);
		SelectObject(hdc, hfValues);
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, 100, 13, lpsResult, strlen(lpsResult));
		DeleteObject(hfValues);
		ReleaseDC(hDlg, hdc);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			ce::Preset_Value additem;
			GetDlgItemText(hDlg, IDC_EDIT_SAVE, buffer, 64);
			if (strlen(buffer) < 64 && strlen(buffer) != 0) {
				additem = { buffer, g_curve_value.ctpt[0].x, g_curve_value.ctpt[0].y, g_curve_value.ctpt[1].x, g_curve_value.ctpt[1].y };
				g_presets_value.emplace_back(additem);
				EndDialog(hDlg, 1);
			}
			else if (strlen(buffer) == 0 && g_config.alert) MessageBox(hDlg, CE_STR_INPUTANAME, CE_PLUGIN_NAME, MB_OK | MB_ICONINFORMATION);
			return 0;
		case IDCANCEL:
			EndDialog(hDlg, 1);
			return 0;
		}
		return 0;
	}
	return 0;
}