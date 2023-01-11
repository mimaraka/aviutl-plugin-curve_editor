//----------------------------------------------------------------------------------
//		Curve Editor
//		ヘッダファイル
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include "cve_preset.hpp"



//----------------------------------------------------------------------------------
//		フィルタ関数
//----------------------------------------------------------------------------------

//フィルタ構造体のポインタを渡す
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void);

//初期化・終了
BOOL							filter_init(FILTER*);
BOOL							filter_exit(FILTER*);

// プロジェクト読み込み・書き出し時に呼び出される関数
BOOL							on_project_load(FILTER* fp, void* editp, void* data, int size);
BOOL							on_project_save(FILTER* fp, void* editp, void* data, int* size);

// ウィンドウプロシージャ
BOOL							filter_wndproc(HWND, UINT, WPARAM, LPARAM, void*, FILTER*);



//----------------------------------------------------------------------------------
//		その他の関数
//----------------------------------------------------------------------------------

namespace cve {
	// Direct2D初期化
	bool						d2d_init();

	// オブジェクト開放
	template <class Interface>
	inline void					d2d_release(Interface** pp_interface)
	{
		if (*pp_interface != nullptr) {
			(*pp_interface)->Release();
			(*pp_interface) = nullptr;
		}
	}

	//文字列の分割
	std::vector<std::string>	split(const std::string& s, TCHAR c);

	//クリップボードにテキストをコピー
	BOOL						copy_to_clipboard(HWND, LPCTSTR);

	// 長さを減算
	void						subtract_length(cve::Float_Point* pt, const cve::Float_Point& st, const cve::Float_Point& ed, float length);

	// 設定をメニューに反映
	void						apply_config_to_menu(HMENU menu, MENUITEMINFO* mi);

	// カーブをトレース
	void						trace_curve();

	// キー押下時の処理
	LRESULT						on_keydown(WPARAM wparam);

	// バージョンをチェック
	DWORD WINAPI				check_version(LPVOID param);
}



//----------------------------------------------------------------------------------
//		ウィンドウプロシージャ
//----------------------------------------------------------------------------------
LRESULT CALLBACK				wndproc_main(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK				wndproc_menu(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK				wndproc_editor(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK				wndproc_library(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK				wndproc_preset_list(HWND, UINT, WPARAM, LPARAM);



//----------------------------------------------------------------------------------
//		ダイアログプロシージャ
//----------------------------------------------------------------------------------
BOOL CALLBACK					dialogproc_config(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK					dialogproc_bezier_param(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK					dialogproc_read(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK					dialogproc_save(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK					dialogproc_id(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK					dialogproc_check_update(HWND, UINT, WPARAM, LPARAM);



//----------------------------------------------------------------------------------
//		フック
//----------------------------------------------------------------------------------
BOOL WINAPI						TrackPopupMenu_hooked(HMENU menu, UINT flags, int x, int y, int reserved, HWND hwnd, const RECT* rect);
INT_PTR WINAPI					DialogBox_hooked(HINSTANCE hinstance, LPCSTR template_name, HWND hwnd_parent, DLGPROC dlgproc, LPARAM init_param);
BOOL CALLBACK					dialogproc_hooked(HWND, UINT, WPARAM, LPARAM);



//----------------------------------------------------------------------------------
//		インライン関数
//----------------------------------------------------------------------------------

// Float_Point -> Double_Point
//inline cve::Double_Point to_double_point(const cve::Float_Point& pt)
//{
//	return  {
//		(double)pt.x,
//		(double)pt.y
//	};
//}
//
//// POINT -> Double_Point
//inline cve::Double_Point to_double_point(const POINT& pt)
//{
//	return  {
//		(double)pt.x,
//		(double)pt.y
//	};
//}
//
//// Float_Point -> Double_Point
//inline cve::Float_Point to_float_point(const cve::Double_Point& pt)
//{
//	return  {
//		(double)pt.x,
//		(double)pt.y
//	};
//}
//
//// POINT -> Double_Point
//inline cve::Float_Point to_float_point(const POINT& pt)
//{
//	return  {
//		(float)pt.x,
//		(float)pt.y
//	};
//}

//グラフ -> クライアント
inline cve::Double_Point to_client2(double graph_x, double graph_y)
{
	return {
		g_view_info.origin.x + graph_x * g_view_info.scale.x,
		g_view_info.origin.y + graph_y * g_view_info.scale.y,
	};
}

inline cve::Double_Point to_client2(const cve::Double_Point& pt_graph)
{
	return {
		to_client2(pt_graph.x, pt_graph.y).x,
		to_client2(pt_graph.x, pt_graph.y).y
	};
}

inline cve::Double_Point to_client2(const cve::Float_Point& pt_graph)
{
	return {
		to_client2((double)pt_graph.x, (double)pt_graph.y).x,
		to_client2((double)pt_graph.x, (double)pt_graph.y).y
	};
}


inline cve::Double_Point to_client2(const POINT& pt_graph)
{
	return {
		to_client2((double)pt_graph.x, (double)pt_graph.y).x,
		to_client2((double)pt_graph.x, (double)pt_graph.y).y
	};
}

inline cve::Float_Point to_client(int gr_x, int gr_y)
{
	return {
		(float)g_view_info.origin.x + (float)(gr_x * g_view_info.scale.x),
		(float)g_view_info.origin.y - (float)(gr_y * g_view_info.scale.y)
	};
}

inline cve::Float_Point to_clientf(float gr_x, float gr_y)
{
	return {
		(float)g_view_info.origin.x + (float)(gr_x * g_view_info.scale.x),
		(float)g_view_info.origin.y - (float)(gr_y * g_view_info.scale.y)
	};
}

inline cve::Float_Point to_client(const POINT& pt_graph)
{
	return {
		to_client(pt_graph.x, pt_graph.y).x,
		to_client(pt_graph.x, pt_graph.y).y
	};
}

inline cve::Float_Point to_client(const cve::Float_Point& pt_graph)
{
	return {
		to_clientf(pt_graph.x, pt_graph.y).x,
		to_clientf(pt_graph.x, pt_graph.y).y
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

inline cve::Float_Point to_graphf(double cl_x, double cl_y)
{
	return {
		(float)((cl_x - g_view_info.origin.x) / g_view_info.scale.x),
		(float)((-cl_y + g_view_info.origin.y) / g_view_info.scale.y)
	};
}

inline POINT to_graph(const POINT& pt_client)
{
	return {
		to_graph(pt_client.x, pt_client.y).x,
		to_graph(pt_client.x, pt_client.y).y
	};
}

//グラフ -> プリセット
inline cve::Float_Point to_preset(int gr_x, int gr_y)
{
	return {
		(float)CVE_MARGIN_PRESET + (gr_x / (float)CVE_GRAPH_RESOLUTION) * (g_config.preset_size - CVE_MARGIN_PRESET * 2),
		g_config.preset_size - (float)CVE_MARGIN_PRESET - (gr_y / (float)CVE_GRAPH_RESOLUTION) * (g_config.preset_size - CVE_MARGIN_PRESET * 2)
	};
}

inline cve::Float_Point to_preset(const POINT& pt_graph)
{
	return {
		to_preset(pt_graph.x, pt_graph.y).x,
		to_preset(pt_graph.x, pt_graph.y).y
	};
}