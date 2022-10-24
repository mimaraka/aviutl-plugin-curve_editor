//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (その他の関数)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		split関数
//---------------------------------------------------------------------
std::vector<std::string> ce::split(const std::string& s, TCHAR c)
{
	std::vector<std::string> elems;
	std::string item;
	for (TCHAR ch : s) {
		if (ch == c) {
			if (!item.empty()) {
				elems.emplace_back(item);
				item.clear();
			}
		}
		else item += ch;
	}
	if (!item.empty())
		elems.emplace_back(item);

	return elems;
}



//---------------------------------------------------------------------
//		クリップボードにテキストをコピー
//---------------------------------------------------------------------
BOOL ce::copy_to_clipboard(HWND hwnd, LPCTSTR text)
{
	HGLOBAL memory;
	LPTSTR buffer;

	if (!OpenClipboard(hwnd))
		return FALSE;

	::EmptyClipboard();
	memory = GlobalAlloc(GHND | GMEM_SHARE, strlen(text) + 1);
	buffer = (PTSTR)::GlobalLock(memory);
	::lstrcpy(buffer, text);

	::GlobalUnlock(memory);
	::SetClipboardData(CF_TEXT, memory);
	::CloseClipboard();

	return TRUE;
}



//---------------------------------------------------------------------
//		for UI
//---------------------------------------------------------------------
ce::Float_Point ce::subtract_length(Float_Point st, Float_Point ed, float length)
{
	Float_Point result;
	float old_length = (float)DISTANCE(st, ed);
	if (old_length == 0)
		return ed;
	if (length > old_length)
		return st;
	double length_ratio = (old_length - length) / old_length;
	float after_x = (float)(st.x + (ed.x - st.x) * length_ratio);
	float after_y = (float)(st.y + (ed.y - st.y) * length_ratio);
	result = {after_x, after_y};

	return result;
}



//---------------------------------------------------------------------
//		g_configの内容をポップアップメニューに反映させる
//---------------------------------------------------------------------
void ce::apply_config_to_menu(HMENU menu, MENUITEMINFO* mi) {
	mi->fMask = MIIM_STATE;

	// Value/ID
	mi->fState = g_config.mode == ce::Config::ID ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(menu, ID_MENU_MODE_ID, FALSE, mi);
	mi->fState = g_config.mode == ce::Config::ID ? MFS_UNCHECKED : MFS_CHECKED;
	SetMenuItemInfo(menu, ID_MENU_MODE_VALUE, FALSE, mi);

	//その他
	mi->fState = g_config.show_handle ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(menu, ID_MENU_SHOWHANDLE, FALSE, mi);
	mi->fState = g_config.align_handle ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(menu, ID_MENU_ALIGNHANDLE, FALSE, mi);

	// ボタンを無効化/有効化
	// IDモードで有効化
	// チェックボックスが存在する場合
	mi->fState |= g_config.mode == ce::Config::ID ? MFS_ENABLED : MFS_DISABLED;
	SetMenuItemInfo(menu, ID_MENU_ALIGNHANDLE, FALSE, mi);
	// チェックボックスが存在しない場合
	mi->fState = g_config.mode == ce::Config::ID ? MFS_ENABLED : MFS_DISABLED;
	SetMenuItemInfo(menu, ID_MENU_PROPERTY, FALSE, mi);

	// Valueモードで有効化
	mi->fState = g_config.mode == ce::Config::ID ? MFS_DISABLED : MFS_ENABLED;
	SetMenuItemInfo(menu, ID_MENU_COPY, FALSE, mi);
	SetMenuItemInfo(menu, ID_MENU_COPY4D, FALSE, mi);
	SetMenuItemInfo(menu, ID_MENU_READ, FALSE, mi);

	//プラグイン名の反映
	mi->fMask = MIIM_TYPE;
	mi->dwTypeData = CE_PLUGIN_NAME "について";
	SetMenuItemInfo(menu, ID_MENU_ABOUT, FALSE, mi);
}



//---------------------------------------------------------------------
//		キー押下時の処理
//---------------------------------------------------------------------
LRESULT ce::on_keydown(WPARAM wparam)
{
	switch (wparam) {
	case 82: //[R]
		if (g_window_editor.hwnd)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_REVERSE, 0);
		return 0;

	case 67: //[C]
		if (::GetAsyncKeyState(VK_CONTROL) < 0 && g_config.mode == ce::Config::Value)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_COPY, 0);
		return 0;

	case 83: //[S]
		if (::GetAsyncKeyState(VK_CONTROL) < 0)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_SAVE, 0);
		else
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_SHOWHANDLE, 0);
		return 0;

	case VK_LEFT: //[<]	
		if (g_config.mode == ce::Config::ID && g_window_header.hwnd)
			::SendMessage(g_window_header.hwnd, WM_COMMAND, CE_CM_ID_BACK, 0);
		return 0;

	case VK_RIGHT: //[>]
		if (g_config.mode == ce::Config::ID && g_window_header.hwnd)
			::SendMessage(g_window_header.hwnd, WM_COMMAND, CE_CM_ID_NEXT, 0);
		return 0;

	case VK_HOME: //[Home]
		if (g_window_editor.hwnd)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_FIT, 0);
		return 0;

	case 65: //[A]
		if (g_window_editor.hwnd)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CT_ALIGN, 0);
		return 0;

	case VK_DELETE:
		if (g_window_editor.hwnd)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_CLEAR, 0);
		return 0;
	}
	return 0;
}