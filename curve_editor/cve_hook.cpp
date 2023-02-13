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
	if (g_config.hooked_popup) {
		constexpr unsigned NUM_APPLY_MODE = 2u;
		unsigned menu_id = 16u;
		unsigned idx_edit_mode;
		unsigned idx_apply_mode;
		unsigned result;
		LPCSTR script_name_top = "Type1@" CVE_FILTER_NAME;
		TCHAR menu_label[MAX_PATH];

		g_config.hooked_popup = false;

		while (true) {
			if (!::GetMenuString(menu, menu_id, menu_label, MAX_PATH, MF_BYCOMMAND))
				return 0;
			if (strcmp(menu_label, script_name_top) == 0) {
				break;
			}
			menu_id += 65536;
		}

		switch (g_config.edit_mode) {
		case cve::Mode_Bezier:
		case cve::Mode_Bezier_Multi:
		case cve::Mode_Bezier_Value:
			idx_edit_mode = 0;
			break;

		case cve::Mode_Elastic:
		case cve::Mode_Bounce:
			idx_edit_mode = 1;
			break;

		default:
			idx_edit_mode = 0;
		}

		idx_apply_mode = std::clamp((unsigned)g_config.apply_mode, 0u, NUM_APPLY_MODE - 1u);
		result = menu_id + 65536 * (idx_edit_mode * NUM_APPLY_MODE + idx_apply_mode);
		
		return result;
	}
	else return TrackPopupMenu_original(menu, flags, x, y, reserved, hwnd, rect);
}



//---------------------------------------------------------------------
//		DialogBox()のフック
//---------------------------------------------------------------------
INT_PTR(WINAPI* DialogBox_original)(HINSTANCE, LPCSTR, HWND, DLGPROC, LPARAM) = nullptr;

INT_PTR WINAPI DialogBox_hooked(HINSTANCE hinstance, LPCSTR template_name, HWND hwnd_parent, DLGPROC dlgproc, LPARAM init_param)
{
	if (g_config.hooked_dialog) {
		g_config.hooked_dialog = false;
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
			data = g_curve_bezier.create_number();
			break;

		case cve::Mode_Bezier_Multi:
			data = g_config.current_id.multi;
			break;

		case cve::Mode_Elastic:
			data = g_curve_elastic.create_number();
			break;

		case cve::Mode_Bounce:
			data = g_curve_bounce.create_number();
			break;

		case cve::Mode_Bezier_Value:
			data = -g_config.current_id.value;
			break;

		default:
			data = 0;
		}

		::_itoa_s(data, text, MAX_PATH, 10);
		::SendMessage(hwnd_edit, WM_SETTEXT, 0, (LPARAM)text);
		::PostMessage(hwnd, WM_COMMAND, IDOK, 0);
		return result;
	}
	return dialogproc_original(hwnd, msg, wparam, lparam);
}