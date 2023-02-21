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
											g_curve_bezier_trace;
extern cve::Curve_Bezier_Multi				g_curve_bezier_multi[CVE_CURVE_MAX],
											g_curve_bezier_multi_trace;
extern cve::Curve_Bezier_Value				g_curve_bezier_value[CVE_CURVE_MAX],
											g_curve_bezier_value_trace;
extern cve::Curve_Elastic					g_curve_elastic,
											g_curve_elastic_trace;
extern cve::Curve_Bounce					g_curve_bounce,
											g_curve_bounce_trace;

extern const cve::Theme						g_theme[2];

extern cve::Config							g_config;

extern aului::Window						g_window_main,
											g_window_menu,
											g_window_editor,
											g_window_library,
											g_window_preset_list;

extern cve::Graph_View_Info					g_view_info;

extern FILTER*								g_fp;
extern void*								g_editp;
extern FILTER_DLL							g_filter;
//Direct2D
extern ID2D1Factory*						g_p_factory;
extern IDWriteFactory*						g_p_write_factory;
extern IWICImagingFactory*					g_p_imaging_factory;
extern ID2D1DCRenderTarget*					g_p_render_target;

//フック
extern BOOL									(WINAPI* TrackPopupMenu_original)(HMENU, UINT, int, int, int, HWND, const RECT*);
extern INT_PTR								(WINAPI* DialogBox_original)(HINSTANCE, LPCSTR, HWND, DLGPROC, LPARAM);
extern INT_PTR								(WINAPI* dialogproc_original)(HWND, UINT, WPARAM, LPARAM);