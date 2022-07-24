//----------------------------------------------------------------------------------
//		curve editor
//		sourse filr (dlgproc)
//		(Visual C++ 2019)
//----------------------------------------------------------------------------------

#include "ce_header.hpp"

#define TOPLEFTX 20
#define TOPLEFTY 33
#define BOTTOMRIGHTX 276
#define BOTTOMRIGHTY 264
#define CTGSIZE 236


//---------------------------------------------------------------------
//		Dialog Procedure (Preferences)
//---------------------------------------------------------------------
BOOL CALLBACK dlgProc_pref(HWND hDlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static HDC hdc, hdc_mem;
	static HWND hCombo, hCombo2;
	static HBITMAP bitmap;
	int cursel;
	static int prevsel = g_cfg.lang;
	switch (msg) {
	case WM_CLOSE:
		DeleteObject(bitmap);
		EndDialog(hDlg, 1);
		return 0;

	case WM_INITDIALOG:
		bitmap = LoadBitmap(g_fp->dll_hinst, MAKEINTRESOURCE(IMG_FLOW));
		if (g_cfg.trace) SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_SETCHECK, BST_CHECKED, 0);
		if (g_cfg.bAlerts) SendMessage(GetDlgItem(hDlg, IDC_CHECK4), BM_SETCHECK, BST_CHECKED, 0);
		if (g_cfg.auto_copy) SendMessage(GetDlgItem(hDlg, IDC_CHECK5), BM_SETCHECK, BST_CHECKED, 0);
		hCombo = GetDlgItem(hDlg, IDC_COMBO1);
		hCombo2 = GetDlgItem(hDlg, IDC_COMBO2);
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"Flow");
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"AE");
		SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"AviUtl");
		if (!g_cfg.lang) SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"Custom");
		else SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"カスタム");
		SendMessage(hCombo, CB_SETCURSEL, g_cfg.theme, 0);

		SendMessage(hCombo2, CB_ADDSTRING, 0, (LPARAM)"English");
		SendMessage(hCombo2, CB_ADDSTRING, 0, (LPARAM)"日本語");
		SendMessage(hCombo2, CB_SETCURSEL, g_cfg.lang, 0);

		if (g_cfg.theme == 3) EnableWindow(GetDlgItem(hDlg, IDC_CUSTOM), TRUE);
		return 0;

	case WM_PAINT:
		hdc = GetDC(hDlg);
		hdc_mem = CreateCompatibleDC(hdc);
		SelectObject(hdc_mem, bitmap);
		BitBlt(hdc, 0, 0, 437, 75, hdc_mem, 0, 0, SRCCOPY);
		DeleteDC(hdc_mem);
		ReleaseDC(hDlg, hdc);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			g_cfg.trace = SendMessage(GetDlgItem(hDlg, IDC_CHECK1), BM_GETCHECK, 0, 0);
			g_cfg.bAlerts = SendMessage(GetDlgItem(hDlg, IDC_CHECK4), BM_GETCHECK, 0, 0);
			g_cfg.auto_copy = SendMessage(GetDlgItem(hDlg, IDC_CHECK5), BM_GETCHECK, 0, 0);
			g_cfg.theme = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
			g_cfg.lang = SendMessage(hCombo2, CB_GETCURSEL, 0, 0);
			g_fp->exfunc->ini_save_int(g_fp, "theme", g_cfg.theme);
			g_fp->exfunc->ini_save_int(g_fp, "lang", g_cfg.lang);
			g_fp->exfunc->ini_save_int(g_fp, "show_hst", g_cfg.trace);
			g_fp->exfunc->ini_save_int(g_fp, "show_alerts", g_cfg.bAlerts);
			g_fp->exfunc->ini_save_int(g_fp, "auto_copy", g_cfg.auto_copy);
			EndDialog(hDlg, 1);
			return 0;

		case IDCANCEL:
			EndDialog(hDlg, 1);
			return 0;

		case IDC_CUSTOM:
			if (!g_cfg.lang) DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_CUSTOM), hDlg, dlgProc_custom);
			else DialogBox(g_fp->dll_hinst, MAKEINTRESOURCE(IDD_CUSTOM_JA), hDlg, dlgProc_custom);
			return 0;

		case IDC_COMBO1:
			switch (HIWORD(wparam)) {
			case CBN_SELCHANGE:
				if (SendMessage(hCombo, CB_GETCURSEL, 0, 0) == 3)
					EnableWindow(GetDlgItem(hDlg, IDC_CUSTOM), TRUE);
				else EnableWindow(GetDlgItem(hDlg, IDC_CUSTOM), FALSE);
				return 0;
			}
		case IDC_COMBO2:
			switch (HIWORD(wparam)) {
			case CBN_SELCHANGE:
				cursel = SendMessage(hCombo2, CB_GETCURSEL, 0, 0);
				if (cursel != prevsel)
					MessageBox(hDlg, cursel ? FLSTR_JA_LANG : FLSTR_LANG, "Flow", MB_OK | MB_ICONINFORMATION);
				prevsel = cursel;
				return 0;
			}
		}
	}
	return 0;
}


//---------------------------------------------------------------------
//		Dialog Procedure (Value)
//---------------------------------------------------------------------
BOOL CALLBACK dlgProc_value(HWND hDlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
	TCHAR chBuffer[30];
	std::regex re(R"(^((\d+ *, *)|(\d*\.\d* *, *))((-?\d+ *, *)|(-?\d*\.\d* *, *))((\d+ *, *)|(\d*\.\d* *, *))((-?\d+ *)|(-?\d*\.\d* *))$)");
	switch (msg) {
	case WM_CLOSE:
		EndDialog(hDlg, 1);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			GetDlgItemText(hDlg, IDC_EDIT1, chBuffer, 30);
			if (std::regex_match(chBuffer, re)) {
				std::string str = chBuffer;
				std::vector<std::string> vec = split(chBuffer, ',');
				g_value_curve.ctpt[0].x = (int)(std::stod(vec[0]) * 1000);
				g_value_curve.ctpt[0].x = (int)(std::stod(vec[0]) * 1000);
				g_value_curve.ctpt[0].y = (int)(std::stod(vec[1]) * 1000);
				g_value_curve.ctpt[1].x = (int)(std::stod(vec[2]) * 1000);
				g_value_curve.ctpt[1].y = (int)(std::stod(vec[3]) * 1000);

				for (int i = 0; i < 2; i++) {
					if (g_value_curve.ctpt[i].y > 3730) g_value_curve.ctpt[i].y = 3730;
					else if (g_value_curve.ctpt[i].y < -2730) g_value_curve.ctpt[i].y = -2730;
				}
				EndDialog(hDlg, 1);
			}
			else if (g_cfg.bAlerts)MessageBox(hDlg, g_cfg.lang ? FLSTR_JA_WRONGINPUTVALUES : FLSTR_WRONGINPUTVALUES, TEXT("Flow"), MB_OK | MB_ICONINFORMATION);
			return 0;
		case IDCANCEL:
			EndDialog(hDlg, 1);
			return 0;
		}
	}
	return 0;
}


//---------------------------------------------------------------------
//		Dialog Procedure (Read)
//---------------------------------------------------------------------
BOOL CALLBACK dlgProc_read(HWND hDlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
	HWND hEdit;
	TCHAR chBuffer[12];
	int intValue;
	std::regex re(R"(^-?\d+$)");
	std::string str;
	switch (msg) {
	case WM_INITDIALOG:
		hEdit = GetDlgItem(hDlg, IDC_EDIT1);
		SendMessage(hEdit, EM_SETLIMITTEXT, 11, 0);
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
			GetDlgItemText(hDlg, IDC_EDIT1, chBuffer, 11);
			if (std::regex_match(chBuffer, re)) {
				str = chBuffer;
				try {
					intValue = std::stoi(str);
				}
				catch (std::out_of_range& e) {
					if (g_cfg.bAlerts) MessageBox(hDlg, g_cfg.lang ? FLSTR_JA_OUTOFRANGE : FLSTR_OUTOFRANGE, TEXT("Flow"), MB_OK | MB_ICONINFORMATION);
					return 0;
				}
				if ((intValue < -2147483647 || 2122746761 < intValue) && g_cfg.bAlerts) {
					MessageBox(hDlg, g_cfg.lang ? FLSTR_JA_OUTOFRANGE : FLSTR_OUTOFRANGE, TEXT("Flow"), MB_OK | MB_ICONINFORMATION);
					return 0;
				}
				readValue(intValue);
				EndDialog(hDlg, 1);
			}
			else if (g_cfg.bAlerts)MessageBox(hDlg, g_cfg.lang ? FLSTR_JA_WRONGINPUTVALUES : FLSTR_WRONGINPUTVALUES, TEXT("Flow"), MB_OK | MB_ICONINFORMATION);
			return 0;
		case IDCANCEL:
			EndDialog(hDlg, 1);
			return 0;
		}
	}
	return 0;
}


//---------------------------------------------------------------------
//		Dialog Procedure (Save)
//---------------------------------------------------------------------
BOOL CALLBACK dlgProc_save(HWND hDlg, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//Creare 4D-Curve IDs
	HDC hdc;
	static HFONT hfValues;
	HWND hEdit;
	LPCTSTR lpsResult;
	std::string strBuffer;
	TCHAR chBuffer[64];
	strBuffer = CtptToStr4DValues();
	lpsResult = strBuffer.c_str();

	switch (msg) {
	case WM_CLOSE:
		EndDialog(hDlg, 1);
		return 0;
	case WM_INITDIALOG:
		hEdit = GetDlgItem(hDlg, IDC_EDIT1);
		SendMessage(hEdit, EM_SETLIMITTEXT, 64, 0);
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
			Flow_Value_Preset additem;
			GetDlgItemText(hDlg, IDC_EDIT1, chBuffer, 64);
			if (strlen(chBuffer) < 64 && strlen(chBuffer) != 0) {
				additem = { chBuffer, g_value_curve.ctpt[0].x, g_value_curve.ctpt[0].y, g_value_curve.ctpt[1].x, g_value_curve.ctpt[1].y };
				g_preset_items.emplace_back(additem);
				EndDialog(hDlg, 1);
			}
			else if (strlen(chBuffer) == 0 && g_cfg.bAlerts) MessageBox(hDlg, g_cfg.lang ? FLSTR_JA_GIVEITANAME : FLSTR_GIVEITANAME, TEXT("Flow"), MB_OK | MB_ICONINFORMATION);
			return 0;
		case IDCANCEL:
			EndDialog(hDlg, 1);
			return 0;
		}
		return 0;
	}
	return 0;
}