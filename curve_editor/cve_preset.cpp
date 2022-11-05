//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル(プリセット)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		コンストラクタ
//---------------------------------------------------------------------
cve::Preset::Preset()
{
	::time(&unix_time);

	bool is_id_available;

	int count = 0;

	// IDをカウントアップしていき利用可能なIDを探す
	while (true) {
		is_id_available = true;

		// 既存のプリセットのIDを検索
		for (int i = 0; i < (int)g_presets_custom.size; i++) {
			if (count == g_presets_custom[i].id) {
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
BOOL cve::Preset::initialize(HWND hwnd_p, const Curve* cv, LPTSTR nm)
{
	strcpy_s(name, CVE_PRESET_NAME_MAX, nm);
	std::string class_name = "cve_preset_" + std::to_string(id);
	curve = *cv;
	hwnd_parent = hwnd_p;
	content_type = Button::String;

	RECT rect = {
		0, 0,
		g_config.preset_size,
		g_config.preset_size
	};

	edge_flag = CVE_EDGE_ALL;


	return create(
		hwnd_p,
		(LPTSTR)class_name.c_str(),
		wndproc_static,
		NULL,
		NULL,
		rect,
		this
	);
}



//---------------------------------------------------------------------
//		プリセットを移動
//---------------------------------------------------------------------
void cve::Preset::move(int panel_width, int index)
{
	const POINT width = {
		CVE_MARGIN + g_config.preset_size,
		CVE_MARGIN + g_config.preset_size + CVE_PRESET_TITLE_HEIGHT
	};

	int x_count = index % (panel_width / width.x);
	int y_count = index / (panel_width / width.x);

	::MoveWindow(
		hwnd,
		width.x * x_count + CVE_MARGIN / 2,
		width.y * y_count + CVE_MARGIN / 2,
		g_config.preset_size,
		g_config.preset_size + CVE_PRESET_TITLE_HEIGHT,
		TRUE
	);
}



//---------------------------------------------------------------------
//		ウィンドウプロシージャ(static変数使用不可)
//---------------------------------------------------------------------
LRESULT cve::Preset::wndproc(HWND hw, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect_wnd;

	::GetClientRect(hwnd, &rect_wnd);

	RECT rect_title = {
		0,
		g_config.preset_size,
		rect_wnd.right,
		rect_wnd.bottom
	};


	switch (msg) {
	case WM_CREATE:
		bitmap_buffer.init(hw);
		bitmap_buffer.set_size(rect_wnd);

		font = ::CreateFont(
			14, 0,
			0, 0,
			FW_REGULAR,
			FALSE, FALSE, FALSE,
			SHIFTJIS_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			NULL,
			CVE_FONT_REGULAR
		);

		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hw;
		return 0;

	case WM_PAINT:
	{
		COLORREF bg = g_theme[g_config.theme].bg;

		::SetTextColor(bitmap_buffer.hdc_memory, g_theme[g_config.theme].preset_tx);

		draw_content(bg, &rect_title, strcmp(name, "") == 0 ? "(noname)" : name, true);

		if (g_render_target != nullptr && g_d2d1_factory != nullptr) {
			g_render_target->BeginDraw();

			curve.draw_curve(&bitmap_buffer, rect_wnd, CVE_DRAW_CURVE_PRESET);

			g_render_target->EndDraw();
		}

		draw_edge();
		bitmap_buffer.transfer();

		return 0;
	}

	default:
		return Button::wndproc(hw, msg, wparam, lparam);
	}
}