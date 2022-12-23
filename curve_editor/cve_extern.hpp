//----------------------------------------------------------------------------------
//		Curve Editor
//		ヘッダファイル
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include "cve_class.hpp"



//----------------------------------------------------------------------------------
//		extern宣言
//----------------------------------------------------------------------------------
extern cve::Curve_Bezier					g_curve_bezier,
											g_curve_bezier_previous;
extern cve::Curve_Multibezier				g_curve_mb[CVE_CURVE_MAX],
											g_curve_mb_previous;
extern cve::Curve_Value						g_curve_value[CVE_CURVE_MAX],
											g_curve_value_previous;
extern cve::Curve_Elastic					g_curve_elastic,
											g_curve_elastic_previous;
extern cve::Curve_Bounce					g_curve_bounce,
											g_curve_bounce_previous;

extern const cve::Theme						g_theme[2];

extern cve::Config							g_config;

extern cve::Window							g_window_main,
											g_window_menu,
											g_window_editor,
											g_window_library,
											g_window_preset_list;

extern cve::Graph_View_Info					g_view_info;

extern FILTER*								g_fp;
extern FILTER_DLL							g_filter;
//Direct2D
extern ID2D1Factory*						g_d2d1_factory;
extern ID2D1DCRenderTarget*					g_render_target;

//フック
extern BOOL									(WINAPI* TrackPopupMenu_original)(HMENU, UINT, int, int, int, HWND, const RECT*);
extern INT_PTR								(WINAPI* DialogBox_original)(HINSTANCE, LPCSTR, HWND, DLGPROC, LPARAM);
extern INT_PTR								(WINAPI* dialogproc_original)(HWND, UINT, WPARAM, LPARAM);