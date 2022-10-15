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
	g_config.separator = fp->exfunc->ini_load_int(fp, "separator", CE_SEPR_W);
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
void rect_divide(LPRECT rect_parent, LPRECT* rects_child, float* weights, int n)
{
	// 親要素の横幅
	int width_parent = rect_parent->right - rect_parent->left;
	// 等分する場合の子要素の横幅
	int width_child = width_parent / n;
	float weights_sum = 0;
	// 0番目からk-1番目までの重みの総和がn個格納された配列
	std::vector<float> list_weights_sum;
	int left, right;

	// 重みがある場合
	if (weights) {
		// 重みの総和を計算するとともに、0番目からk-1番目までの重みの総和をlist_weights_sum[k - 1]に代入する。
		for (int i = 0; i < n; i++) {
			weights_sum += weights[i];
			list_weights_sum.emplace_back(weights_sum);
		}
	}

	// それぞれのRECTの位置を設定
	for (int i = 0; i < n; i++) {
		// 重みがある場合
		if (weights) {
			left = (int)(width_parent * ((i == 0) ? 0 : list_weights_sum[i - 1]) / weights_sum);
			right = (int)(width_parent * list_weights_sum[i] / weights_sum);
		}
		// 重みがない(等分する)場合
		else {
			left = i * width_child;
			right = (i + 1) * width_child;
		}
		rects_child[i]->left = rect_parent->left + left;
		rects_child[i]->right = rect_parent->left + right;
		rects_child[i]->top = rect_parent->top;
		rects_child[i]->bottom = rect_parent->bottom;
	}
}



//---------------------------------------------------------------------
//		RECTに余白を設定
//---------------------------------------------------------------------
void rect_set_margin(LPRECT rect, int left, int top, int right, int bottom)
{
	rect->left += left;
	rect->top += top;
	rect->right -= right;
	rect->bottom -= bottom;

	if (rect->left > rect->right)
		rect->left = rect->right = (rect->left + rect->right) / 2;
	if (rect->top > rect->bottom)
		rect->top = rect->bottom = (rect->top + rect->bottom) / 2;
}



//---------------------------------------------------------------------
//		キー押下時の処理
//---------------------------------------------------------------------
LRESULT on_keydown(WPARAM wparam)
{
	switch (wparam) {
	case 82: //[R]
		if (g_window_editor.hwnd)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_REVERSE, 0);
		return 0;

	case 67: //[C]
		if (::GetAsyncKeyState(VK_CONTROL) < 0)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_COPY, 0);
		return 0;

	case 83: //[S]
		if (::GetAsyncKeyState(VK_CONTROL) < 0)
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_SAVE, 0);
		else
			::SendMessage(g_window_editor.hwnd, WM_COMMAND, CE_CM_SHOWHANDLE, 0);
		return 0;

	case VK_LEFT: //[<]	
		if (g_config.mode && g_window_header.hwnd)
			::SendMessage(g_window_header.hwnd, WM_COMMAND, CE_CM_ID_BACK, 0);
		return 0;

	case VK_RIGHT: //[>]
		if (g_config.mode && g_window_header.hwnd)
			::SendMessage(g_window_header.hwnd, WM_COMMAND, CE_CM_ID_NEXT, 0);
		return 0;

	case 70: //[F]
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