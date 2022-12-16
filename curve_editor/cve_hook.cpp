//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (フック)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		TrackPopupMenu()のフック
//---------------------------------------------------------------------
BOOL(WINAPI* TrackPopupMenu_original)(HMENU, UINT, int, int, int, HWND, const RECT*) = nullptr;

BOOL WINAPI TrackPopupMenu_hooked(HMENU menu, UINT flags, int x, int y, int reserved, HWND hwnd, const RECT* rect)
{
	if (g_config.is_hooked_popup) {
		int count = 0;
		int index_separator = -1;
		int index_script;
		int index_mode;
		int id_script;
		LPCSTR script_name_top = "ベジェ・ID@" CVE_PLUGIN_NAME;
		TCHAR menu_label[MAX_PATH];

		static MENUITEMINFO minfo;
		minfo.cbSize = sizeof(MENUITEMINFO);
		minfo.fMask = MIIM_TYPE;

		g_config.is_hooked_popup = false;

		// スクリプトのメニューIDを取得
		while (true) {
			BOOL result = ::GetMenuItemInfo(menu, count, MF_BYPOSITION, &minfo);
			::GetMenuString(menu, count, menu_label, MAX_PATH, MF_BYPOSITION);

			if (!result)
				return 0;
			else if (minfo.fType & MFT_SEPARATOR) {
				index_separator = count;
			}
			else if (strcmp(menu_label, script_name_top) == 0) {
				break;
			}
			count++;
		}
		switch (g_config.edit_mode) {
		case cve::Mode_Bezier:
		case cve::Mode_Multibezier:
		case cve::Mode_Value:
			index_mode = 0;
			break;

		case cve::Mode_Elastic:
		case cve::Mode_Bounce:
			index_mode = 1;
			break;

		default:
			index_mode = 0;
			break;
		}

		index_script = count - index_separator - 10 + index_mode * 2;

		if (::GetAsyncKeyState(VK_CONTROL) < 0)
			index_script++;

		id_script = 16 + 65536 * index_script;
		
		return id_script;
	}
	else return TrackPopupMenu_original(menu, flags, x, y, reserved, hwnd, rect);
}



//---------------------------------------------------------------------
//		DialogBox()のフック
//---------------------------------------------------------------------
INT_PTR(WINAPI* DialogBox_original)(HINSTANCE, LPCSTR, HWND, DLGPROC, LPARAM) = nullptr;

INT_PTR WINAPI DialogBox_hooked(HINSTANCE hinstance, LPCSTR template_name, HWND hwnd_parent, DLGPROC dlgproc, LPARAM init_param)
{
	if (g_config.is_hooked_dialog) {
		g_config.is_hooked_dialog = false;
		dialogproc_original = dlgproc;
		return DialogBox_original(hinstance, template_name, hwnd_parent, dialogproc_hooked, init_param);
	}
	return DialogBox_original(hinstance, template_name, hwnd_parent, dlgproc, init_param);

}



//---------------------------------------------------------------------
//		ダイアログプロシージャ(フック)
//---------------------------------------------------------------------
INT_PTR(WINAPI* dialogproc_original)(HWND, UINT, WPARAM, LPARAM) = nullptr;

BOOL CALLBACK dialogproc_hooked(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_INITDIALOG) {
		INT_PTR result = dialogproc_original(hwnd, msg, wparam, lparam);
		HWND hwnd_edit = ::GetWindow(hwnd, GW_CHILD);
		TCHAR text[MAX_PATH];
		int data;

		switch (g_config.edit_mode) {
		case cve::Mode_Bezier:
			data = g_curve_normal.create_number();
			break;

		case cve::Mode_Multibezier:
			data = g_config.current_id.multibezier;
			break;

		case cve::Mode_Elastic:
			data = g_curve_elastic.create_number();
			break;

		case cve::Mode_Bounce:
			data = g_curve_bounce.create_number();
			break;

		case cve::Mode_Value:
			data = -g_config.current_id.value;
			break;
		}

		::_itoa_s(data, text, MAX_PATH, 10);
		::SendMessage(hwnd_edit, WM_SETTEXT, 0, (LPARAM)text);
		::PostMessage(hwnd, WM_COMMAND, IDOK, 0);
		return result;
	}
	return dialogproc_original(hwnd, msg, wparam, lparam);
}