//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル(プリセット)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		コンストラクタ
//---------------------------------------------------------------------
cve::Preset::Preset(const Curve* cv) : curve(*cv)
{
	::time(&unix_time);

	bool is_id_available;

	int count = 0;

	// IDをカウントアップしていき利用可能なIDを探す
	while (true) {
		is_id_available = true;

		// 既存のプリセットのIDを検索
		for (int i = 0; i < (int)g_presets.size(); i++) {
			if (count == g_presets[i].id) {
				is_id_available = false;
				break;
			}
		}

		if (is_id_available)
			break;

		count++;
	}

	id = count;
}



//---------------------------------------------------------------------
//		プリセット(ウィンドウ)を作成
//---------------------------------------------------------------------
BOOL cve::Preset::create(HWND hwnd_parent, LPTSTR nm)
{
	strcpy_s(name, CVE_PRESET_NAME_MAX, nm);
	std::string class_name = "cve_preset_" + std::to_string(id);

	WNDCLASSEX tmp;
	tmp.cbSize = sizeof(tmp);
	tmp.style = CS_HREDRAW | CS_VREDRAW;
	tmp.lpfnWndProc = wndproc_static;
	tmp.cbClsExtra = 0;
	tmp.cbWndExtra = 0;
	tmp.hInstance = g_fp->dll_hinst;
	tmp.hIcon = NULL;
	tmp.hIconSm = NULL;
	tmp.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	tmp.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
	tmp.lpszMenuName = NULL;
	tmp.lpszClassName = class_name.c_str();

	if (RegisterClassEx(&tmp)) {
		hwnd = ::CreateWindowEx(
			NULL,
			class_name.c_str(),
			NULL,
			WS_CHILD | WS_VISIBLE,
			g_config.preset_size * id, 0,
			g_config.preset_size,
			g_config.preset_size,
			hwnd_parent,
			NULL,
			g_fp->dll_hinst,
			this
		);
		if (hwnd != nullptr)
			return TRUE;
	}
	return FALSE;
}



//---------------------------------------------------------------------
//		プリセットを移動
//---------------------------------------------------------------------
void cve::Preset::move(int panel_width, int index)
{
	const int w = 2 * CVE_MARGIN_PRESET + g_config.preset_size;
	int x_count = index % (panel_width / w);
	int y_count = index / (panel_width / w);

	::MoveWindow(
		hwnd,
		w * x_count,
		w * y_count,
		g_config.preset_size,
		g_config.preset_size + CVE_MARGIN_PRESET,
		TRUE
	);
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ(static変数使用不可)
//---------------------------------------------------------------------
LRESULT cve::Preset::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{

	switch (msg) {

	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
}