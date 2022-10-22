//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (フック)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



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
		int id_script;
		LPCSTR script_name = "Value@" CE_PLUGIN_NAME;
		TCHAR menu_label[MAX_PATH];

		static MENUITEMINFO minfo;
		minfo.cbSize = sizeof(MENUITEMINFO);
		minfo.fMask = MIIM_TYPE;

		g_config.is_hooked_popup = FALSE;

		// スクリプトのメニューIDを取得
		while (true) {
			BOOL result = ::GetMenuItemInfo(menu, count, MF_BYPOSITION, &minfo);
			::GetMenuString(menu, count, menu_label, MAX_PATH, MF_BYPOSITION);

			if (!result)
				return 0;
			else if (minfo.fType & MFT_SEPARATOR) {
				index_separator = count;
			}
			else if (strcmp(menu_label, script_name) == 0) {
				break;
			}
			count++;
		}

		index_script = count - index_separator - 10 + g_config.mode;

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
		g_config.is_hooked_dialog = FALSE;
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
		if (g_config.mode == ce::Config::ID)
			data = g_config.current_id;
		else
			data = g_curve_value.create_value_1d();
		::_itoa_s(data, text, MAX_PATH, 10);
		::SendMessage(hwnd_edit, WM_SETTEXT, 0, (LPARAM)text);
		::PostMessage(hwnd, WM_COMMAND, IDOK, 0);
		return result;
	}
	return dialogproc_original(hwnd, msg, wparam, lparam);
}