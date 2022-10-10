//----------------------------------------------------------------------------------
//		Curve Editor
//		ヘッダファイル
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include "ce_classes.hpp"



//----------------------------------------------------------------------------------
//		extern宣言
//----------------------------------------------------------------------------------
extern ce::Curve_Value						g_curve_value;
extern ce::Curve_Value						g_curve_value_previous;
extern ce::Curve_ID							g_curve_id[CE_CURVE_MAX];
extern ce::Curve_ID							g_curve_id_previous;
extern std::vector<ce::Preset>				g_presets;
extern const ce::Theme						g_theme_dark, g_theme_light;
extern const ce::Theme						g_theme[2];
extern ce::Config							g_config;
extern ce::Window							g_window;
extern ce::Graph_View_Info					g_view_info;
extern FILTER*								g_fp;
extern FILTER_DLL							g_filter;
//Direct2D
extern ID2D1Factory*						g_d2d1_factory;
extern ID2D1DCRenderTarget*					g_render_target;


//----------------------------------------------------------------------------------
//		関数
//----------------------------------------------------------------------------------

//フィルタ構造体のポインタを渡す
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void);

//初期化・終了
BOOL				initialize(FILTER*);
BOOL				exit(FILTER*);

// aviutl.iniを用いて設定を読み書き
void				ini_load_configs(FILTER* fp);
void				ini_write_configs(FILTER* fp);

// プロジェクト読み込み・書き出し時に呼び出される関数
BOOL				on_project_load(FILTER* fp, void* editp, void* data, int size);
BOOL				on_project_save(FILTER* fp, void* editp, void* data, int* size);

//文字列の分割
std::vector<std::string> split(const std::string& s, TCHAR c);

//クリップボードにテキストをコピー
BOOL				copy_to_clipboard(HWND, LPCTSTR);

ce::Float_Point		subtract_length(ce::Float_Point st, ce::Float_Point ed, float length);

// 設定をメニューに反映
void				apply_config_to_menu(HMENU menu, MENUITEMINFO minfo);

// RECTを分割
void				divide_rect(LPRECT rect_parent, LPRECT* rects_child, int n);

//子ウィンドウを作成
HWND				create_child(
						HWND hwnd,
						WNDPROC wndproc,
						LPSTR name,
						LONG style,
						int x,
						int y,
						int width,
						int height);

//HWND				create_child(
//	HWND hwnd,
//	WNDPROC wndproc,
//	LPSTR name,
//	LONG style,
//	int upperleft_x,
//	int upperleft_y,
//	int bottomright_x,
//	int bottomright_y,
//	int padding_top,
//	int padding_left,
//	int padding_bottom,
//	int padding_right);


//描画
void				d2d_init();
void				d2d_setup(ce::Bitmap_Canvas* canvas, LPRECT rect_wnd, COLORREF cr);
void				draw_main(ce::Bitmap_Canvas* canvas, LPRECT rect_wnd, LPRECT rect_sepr);
void				draw_footer(ce::Bitmap_Canvas* canvas, LPRECT rect_wnd);
void				draw_panel_library(ce::Bitmap_Canvas* canvas, LPRECT rect_wnd);
void				draw_panel_editor(ce::Bitmap_Canvas* canvas, LPRECT rect_wnd);
void				draw_panel_graph(ce::Bitmap_Canvas* canvas, LPRECT rect_wnd);


//ダイアログプロシージャ
BOOL CALLBACK		dialogproc_settings(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		dialogproc_value(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		dialogproc_read(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		dialogproc_save(HWND, UINT, WPARAM, LPARAM);


//ウィンドウプロシージャ
BOOL				wndproc_base(HWND, UINT, WPARAM, LPARAM, void*, FILTER*);
LRESULT CALLBACK	wndproc_main(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_editor(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_footer(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_graph(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_library(HWND, UINT, WPARAM, LPARAM);

//コントロールプロシージャ
LRESULT CALLBACK	wndproc_control_apply(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_control_value(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_control_save(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_control_read(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_control_mode_value(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_control_mode_id(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_control_id(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_control_align(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	wndproc_control_id_delete(HWND, UINT, WPARAM, LPARAM);



//----------------------------------------------------------------------------------
//		インライン関数
//----------------------------------------------------------------------------------

//グラフ -> クライアント
inline ce::Float_Point to_client(int gr_x, int gr_y)
{
	return {
		g_view_info.origin.x + (float)(gr_x * g_view_info.scale.x),
		g_view_info.origin.y - (float)(gr_y * g_view_info.scale.y)
	};
}

inline ce::Float_Point to_client(POINT gr_pt)
{
	return {
		to_client(gr_pt.x, gr_pt.y).x,
		to_client(gr_pt.x, gr_pt.y).y
	};
}

//クライアント -> グラフ
inline POINT to_graph(double cl_x, double cl_y)
{
	return {
		(int)((cl_x - g_view_info.origin.x) / g_view_info.scale.x),
		(int)((-cl_y + g_view_info.origin.y) / g_view_info.scale.y)
	};
}

inline POINT to_graph(POINT cl_pt)
{
	return {
		to_graph(cl_pt.x, cl_pt.y).x,
		to_graph(cl_pt.x, cl_pt.y).y
	};
}
