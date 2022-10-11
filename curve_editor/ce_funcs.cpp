//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (その他の関数)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		aviutl.iniから設定を読み込み
//---------------------------------------------------------------------
void ini_load_configs(FILTER* fp)
{
	g_config.theme = fp->exfunc->ini_load_int(fp, "theme", 0);
	g_config.trace = fp->exfunc->ini_load_int(fp, "show_previous_curve", 1);
	g_config.alert = fp->exfunc->ini_load_int(fp, "show_alerts", 1);
	g_config.auto_copy = fp->exfunc->ini_load_int(fp, "auto_copy", 0);
	g_config.current_id = fp->exfunc->ini_load_int(fp, "id", 0);
	g_curve_value.ctpt[0].x = fp->exfunc->ini_load_int(fp, "x1", (int)(CE_GR_RESOLUTION * 0.4));
	g_curve_value.ctpt[0].y = fp->exfunc->ini_load_int(fp, "y1", (int)(CE_GR_RESOLUTION * 0.4));
	g_curve_value.ctpt[1].x = fp->exfunc->ini_load_int(fp, "x2", (int)(CE_GR_RESOLUTION * 0.6));
	g_curve_value.ctpt[1].y = fp->exfunc->ini_load_int(fp, "y2", (int)(CE_GR_RESOLUTION * 0.6));
	g_config.separator = fp->exfunc->ini_load_int(fp, "separator", CE_SEPR_DEF);
	g_config.mode = fp->exfunc->ini_load_int(fp, "mode", 0);
	g_config.align_handle = fp->exfunc->ini_load_int(fp, "align_handle", 1);
	g_config.show_handle = fp->exfunc->ini_load_int(fp, "show_handle", 1);
	g_config.preset_size = fp->exfunc->ini_load_int(fp, "preset_size", CE_DEF_PRESET_SIZE);
}



//---------------------------------------------------------------------
//		aviutl.iniから設定を書き込み
//---------------------------------------------------------------------
void ini_write_configs(FILTER* fp)
{
	fp->exfunc->ini_save_int(fp, "x1", g_curve_value.ctpt[0].x);
	fp->exfunc->ini_save_int(fp, "y1", g_curve_value.ctpt[0].y);
	fp->exfunc->ini_save_int(fp, "x2", g_curve_value.ctpt[1].x);
	fp->exfunc->ini_save_int(fp, "y2", g_curve_value.ctpt[1].y);
	fp->exfunc->ini_save_int(fp, "separator", g_config.separator);
	fp->exfunc->ini_save_int(fp, "mode", g_config.mode);
	fp->exfunc->ini_save_int(fp, "align_handle", g_config.align_handle);
	fp->exfunc->ini_save_int(fp, "show_handle", g_config.show_handle);
}



//---------------------------------------------------------------------
//		子ウィンドウを生成
//---------------------------------------------------------------------
HWND create_child(
	HWND hwnd,
	WNDPROC wndproc,
	LPSTR name,
	LONG style,
	int x, int y, int width, int height)
	//int upperleft_x,
	//int upperleft_y,
	//int bottomright_x,
	//int bottomright_y,
	//int padding_top,
	//int padding_left,
	//int padding_bottom,
	//int padding_right)
{
	HWND hCld;
	WNDCLASSEX tmp;
	tmp.cbSize			= sizeof(tmp);
	tmp.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	tmp.lpfnWndProc		= wndproc;
	tmp.cbClsExtra		= 0;
	tmp.cbWndExtra		= 0;
	tmp.hInstance		= g_fp->dll_hinst;
	tmp.hIcon			= NULL;
	tmp.hCursor			= LoadCursor(NULL, IDC_ARROW);
	tmp.hbrBackground	= NULL;
	tmp.lpszMenuName	= NULL;
	tmp.lpszClassName	= name;
	tmp.hIconSm			= NULL;

	if (RegisterClassEx(&tmp)) {
		hCld = CreateWindowEx(
			NULL,
			name,
			NULL,
			WS_CHILD | WS_VISIBLE | style,
			x, y,
			width, height,
			/*upperleft_x + padding_left,
			upperleft_y + padding_top,
			bottomright_x - upperleft_x - padding_left - padding_right,
			bottomright_y - upperleft_y - padding_top - padding_bottom,*/
			hwnd,
			NULL,
			g_fp->dll_hinst,
			NULL
		);
		return hCld;
	}
	return 0;
}



//---------------------------------------------------------------------
//		split関数
//---------------------------------------------------------------------
std::vector<std::string> split(const std::string& s, TCHAR c)
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
BOOL copy_to_clipboard(HWND hwnd, LPCTSTR text)
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
ce::Float_Point subtract_length(ce::Float_Point st, ce::Float_Point ed, float length)
{
	ce::Float_Point result;
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
void apply_config_to_menu(HMENU menu, MENUITEMINFO minfo) {
	minfo.fMask = MIIM_STATE;

	// Value/ID
	minfo.fState = g_config.mode ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(menu, ID_MENU_MODE_ID, FALSE, &minfo);
	minfo.fState = g_config.mode ? MFS_UNCHECKED : MFS_CHECKED;
	SetMenuItemInfo(menu, ID_MENU_MODE_VALUE, FALSE, &minfo);

	//その他
	minfo.fState = g_config.show_handle ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(menu, ID_MENU_SHOWHANDLE, FALSE, &minfo);
	minfo.fState = g_config.align_handle ? MFS_CHECKED : MFS_UNCHECKED;
	SetMenuItemInfo(menu, ID_MENU_ALIGNHANDLE, FALSE, &minfo);

	// ボタンを無効化/有効化
	// IDモードで有効化
	// チェックボックスが存在する場合
	minfo.fState |= g_config.mode ? MFS_ENABLED : MFS_DISABLED;
	SetMenuItemInfo(menu, ID_MENU_ALIGNHANDLE, FALSE, &minfo);
	// チェックボックスが存在しない場合
	minfo.fState = g_config.mode ? MFS_ENABLED : MFS_DISABLED;
	SetMenuItemInfo(menu, ID_MENU_PROPERTY, FALSE, &minfo);

	// Valueモードで有効化
	minfo.fState = g_config.mode ? MFS_DISABLED : MFS_ENABLED;
	SetMenuItemInfo(menu, ID_MENU_COPY, FALSE, &minfo);
	SetMenuItemInfo(menu, ID_MENU_COPY4D, FALSE, &minfo);
	SetMenuItemInfo(menu, ID_MENU_READ, FALSE, &minfo);

	//プラグイン名の反映
	minfo.fMask = MIIM_TYPE;
	minfo.dwTypeData = CE_PLUGIN_NAME "について";
	SetMenuItemInfo(menu, ID_MENU_ABOUT, FALSE, &minfo);
}



//---------------------------------------------------------------------
//		RECTを横n個に分割する
//---------------------------------------------------------------------
void divide_rect(LPRECT rect_parent, LPRECT* rects_child, int n)
{
	int width_parent = rect_parent->right - rect_parent->left;
	int width_child = width_parent / n;

	for (int i = 0; i < n; i++) {
		rects_child[i]->left = rect_parent->left + i * width_child;
		rects_child[i]->right = rect_parent->left + (i + 1) * width_child;
		rects_child[i]->top = rect_parent->top;
		rects_child[i]->bottom = rect_parent->bottom;
	}
}