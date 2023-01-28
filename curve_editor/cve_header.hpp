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
	// DirectX初期化・終了
	bool						dx_init();
	void						dx_exit();

	//文字列の分割
	std::vector<std::string>	split(const std::string& s, TCHAR c);

	//クリップボードにテキストをコピー
	BOOL						copy_to_clipboard(HWND, LPCTSTR);

	// 長さを減算
	void						subtract_length(aului::Point<float>* pt, const aului::Point<float>& st, const aului::Point<float>& ed, float length);

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
BOOL CALLBACK					dialogproc_about(HWND, UINT, WPARAM, LPARAM);
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

// グラフ -> クライアント
template <typename T>
inline aului::Point<T> to_client(T graph_x, T graph_y)
{
	return {
		(T)(g_view_info.origin.x + graph_x * g_view_info.scale.x),
		(T)(g_view_info.origin.y - graph_y * g_view_info.scale.y),
	};
}

template <typename T>
inline aului::Point<T> to_client(const aului::Point<T>& pt_graph)
{
	return to_client(pt_graph.x, pt_graph.y);
}

// クライアント -> グラフ
template <typename T>
inline aului::Point<T> to_graph(T client_x, T client_y)
{
	return {
		(T)((client_x - g_view_info.origin.x) / g_view_info.scale.x),
		(T)((-client_y + g_view_info.origin.y) / g_view_info.scale.y)
	};
}

template <typename T>
inline aului::Point<T> to_graph(const aului::Point<T>& pt_client)
{
	return to_graph(pt_client.x, pt_client.y);
}

// グラフ -> プリセット
template <typename T>
inline aului::Point<T> to_preset(T graph_x, T graph_y)
{
	return {
		(T)(CVE_MARGIN_PRESET + (graph_x / (double)CVE_GRAPH_RESOLUTION) * (g_config.preset_size - CVE_MARGIN_PRESET * 2)),
		(T)(g_config.preset_size - CVE_MARGIN_PRESET - (graph_y / (double)CVE_GRAPH_RESOLUTION) * (g_config.preset_size - CVE_MARGIN_PRESET * 2))
	};
}

template <typename T>
inline aului::Point<T> to_preset(const aului::Point<T>& pt_graph)
{
	return to_preset(pt_graph.x, pt_graph.y);
}