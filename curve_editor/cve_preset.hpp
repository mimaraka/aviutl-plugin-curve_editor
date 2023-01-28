//----------------------------------------------------------------------------------
//		Curve Editor
//		ヘッダファイル
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include "cve_extern.hpp"



//---------------------------------------------------------------------
//		プリセット
//---------------------------------------------------------------------
namespace cve {
	template <typename Curve_Class>
	class Preset : public Button {
	protected:
		virtual LRESULT		wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	public:
		char				name[CVE_PRESET_NAME_MAX];
		Curve_Class			curve;
		time_t				unix_time;

		Preset();

		BOOL				init(HWND hwnd_p, const Curve_Class& cv, LPTSTR nm);

		void				move(int panel_width, int index);
	};
}



//----------------------------------------------------------------------------------
//		extern宣言
//----------------------------------------------------------------------------------
extern cve::Static_Array<cve::Preset<cve::Curve_Bezier>, CVE_PRESET_NUM_DEFAULT> g_presets_bezier_default;
extern cve::Static_Array<cve::Preset<cve::Curve_Bezier>, CVE_PRESET_NUM_CUSTOM> g_presets_bezier_custom;
extern cve::Static_Array<cve::Preset<cve::Curve_Bezier_Multi>, CVE_PRESET_NUM_DEFAULT> g_presets_bezier_multi_default;
extern cve::Static_Array<cve::Preset<cve::Curve_Bezier_Multi>, CVE_PRESET_NUM_CUSTOM> g_presets_bezier_multi_custom;
extern cve::Static_Array<cve::Preset<cve::Curve_Elastic>, CVE_PRESET_NUM_CUSTOM> g_presets_elastic_custom;
extern cve::Static_Array<cve::Preset<cve::Curve_Bounce>, CVE_PRESET_NUM_CUSTOM> g_presets_bounce_custom;



//---------------------------------------------------------------------
//		コンストラクタ
//---------------------------------------------------------------------
template <typename Curve_Class>
cve::Preset<Curve_Class>::Preset()
{
	::time(&unix_time);

	bool is_id_available;

	int count = 0;

	// IDをカウントアップしていき利用可能なIDを探す
	while (true) {
		is_id_available = true;

		// 既存のプリセットのIDを検索
		for (int i = 0; i < (int)g_presets_bezier_custom.size; i++) {
			if (count == g_presets_bezier_custom[i].id) {
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
template <typename Curve_Class>
BOOL cve::Preset<Curve_Class>::init(HWND hwnd_p, const Curve_Class& cv, LPTSTR nm)
{
	strcpy_s(name, CVE_PRESET_NAME_MAX, nm);
	std::string class_name = "cve_preset_" + std::to_string(id);
	hwnd_parent = hwnd_p;
	content_type = Button::String;

	RECT rect = {
		0, 0,
		g_config.preset_size,
		g_config.preset_size + CVE_PRESET_TITLE_HEIGHT
	};

	flag_edge = FLAG_EDGE_ALL;

	curve = cv;

	return create(
		g_fp->dll_hinst,
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
template <typename Curve_Class>
void cve::Preset<Curve_Class>::move(int panel_width, int index)
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
template <typename Curve_Class>
LRESULT cve::Preset<Curve_Class>::wndproc(HWND hw, UINT msg, WPARAM wparam, LPARAM lparam)
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
		bitmap_buffer.init(hwnd);
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
		aului::Color col_bg = g_theme[g_config.theme].bg;

		::SetTextColor(bitmap_buffer.hdc_memory, g_theme[g_config.theme].preset_tx.colorref());

		draw_content(col_bg, &rect_title, strcmp(name, "") == 0 ? "(無題)" : name, true);

		if (g_p_render_target != nullptr && g_p_factory != nullptr) {
			g_p_render_target->BeginDraw();

			// curve.draw_curve(&bitmap_buffer, rect_wnd, DRAW_CURVE_PRESET);

			g_p_render_target->EndDraw();
		}

		draw_edge();
		bitmap_buffer.transfer();

		return 0;
	}

	default:
		return Button::wndproc(hw, msg, wparam, lparam);
	}
}