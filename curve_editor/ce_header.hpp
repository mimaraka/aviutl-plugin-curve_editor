//----------------------------------------------------------------------------------
//		Curve Editor
//		ヘッダファイル
//		(Visual C++ 2022)
//----------------------------------------------------------------------------------

#pragma once

#include <windows.h>
#include <windowsx.h>
#include <wininet.h>
#include <d2d1.h>
#include <lua.hpp>
#include <fstream>
#include <regex>
#include <filter.h>
#include "ce_macro.hpp"
#include "resource.h"


//----------------------------------------------------------------------------------
//		typedef
//----------------------------------------------------------------------------------
typedef struct tagDoublePoint {
	double x, y;
} DoublePoint;

namespace ce {
	typedef struct tagTheme {
		COLORREF
			bg,
			bg_window,
			bg_graph,
			curve,
			handle,
			bt_apply,
			bt_tx,
			bg_others;
		COLORREF reserved[10];
	} Theme;

	typedef struct tagWindow {
		HWND
			base,
			main,
			side,
			editor,
			graph,
			prev;
	} Window;

	typedef struct tagWindow_Position {
		RECT
			rect_wnd,
			padding;
	} Window_Position;

	typedef enum tagPoint_Position {
		CTPT_NULL,
		CTPT_CENTER,
		CTPT_LEFT,
		CTPT_RIGHT
	} Point_Position;

	typedef struct tagPoint_Address {
		int index;
		Point_Position position;
	} Point_Address;

	typedef struct tagPoints_ID {
		POINT
			pt_center,
			pt_left,
			pt_right;
		int type;							//[タイプ]0:初期制御点の左; 1:初期制御点の右; 2:拡張制御点;
	} Points_ID;

	typedef struct tagConfig {
		BOOL
			alert,
			trace,
			auto_copy,
			mode,
			show_handle,
			align_handle;
		int
			theme,
			separator,
			current_id;
	} Config;

	typedef struct tagGr_Disp_Info {
		DoublePoint o;
		DoublePoint scale;
	} Gr_Disp_Info;
}


//----------------------------------------------------------------------------------
//		クラス
//----------------------------------------------------------------------------------
namespace ce {
	template <typename T, size_t N>
	class StaticArray {
	public:
		size_t size;
		static const size_t max_size = N;

		T elements[N];

		StaticArray() : size(0) {}

		template <typename U>
		T& operator[] (U i) { return elements[MINMAXLIM(i, 0, N - 1)]; }

		void clear(void)
		{
			size = 0;
		}

		void insert(size_t index, const T& v)
		{
			int max = size >= max_size ? max_size - 1 : size;
			for (int i = max; i > (int)index;) {
				i--;
				elements[i + 1] = elements[i];
			}
			elements[index] = v;
			if (size < max_size) size++;
		}

		void push_back(const T& v)
		{
			if (size < max_size) {
				size++;
				elements[size - 1] = v;
			}
		}

		void erase(int index)
		{
			size--;
			for (int i = index; i < (int)size; i++) {
				elements[i] = elements[i + 1];
			}
		}
	};


	class Curve_Value {
	public:
		POINT ctpt[2];

		Curve_Value() { init(); }

		void init();
		int point_in_ctpts(POINT cl_pt);
		void move_point(int index, POINT gr_pt);
		int create_value_1d();
		std::string create_value_4d();
		void read_value_1d(int value);
	};


	class Curve_ID {
	public:
		StaticArray <Points_ID, CE_POINT_MAX> ctpts;

		Curve_ID() { init(); }

		void				init();
		void				add_point(POINT cl_pt);
		void				delete_point(POINT cl_pt);
		void				clear();
		POINT				get_point(Point_Address address);
		void				move_point(Point_Address address, POINT gr_pt, BOOL bReset);
		Point_Address		pt_in_ctpt(POINT cl_pt);
		double				get_handle_angle(Point_Address address);
		void				correct_handle(Point_Address address, double angle);
		void				set_handle_angle(Point_Address address, double angle, BOOL bLength, double lgth);
		void				reverse_curve();
		double				get_value(double ratio, double st, double ed);
	};


	class Control {
		int flag;
		LPCTSTR icon;
		COLORREF& color;
		static LRESULT CALLBACK message_router(HWND, UINT, WPARAM, LPARAM);
	public:
		HWND hwnd;
		int id;

		Control(COLORREF& col) : color{ col } {}

		BOOL				create(HWND hwnd_p, WNDPROC exWndProc, LPSTR name, LPCTSTR icon, int ct_id, int x, int y, int w, int h, int fl);
		void				move(int x, int y, int w, int h);
		LRESULT CALLBACK	wndproc(HWND, UINT, WPARAM, LPARAM);
	};

	class Control_Test
	{
		TCHAR WINDOW_CLASS_NAME[1024];

		static LRESULT CALLBACK wndproc_static(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	private:
		HWND hwnd;
		HINSTANCE hinstance;
	public:
		Control_Test();
		virtual ~Control_Test();

		// ウィンドウ定義用メンバ関数
		bool regist_window(
			const TCHAR* window_class_name,
			const HINSTANCE hinst
		);

		// ウィンドウプロシージャ
		virtual LRESULT wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		HWND get_hwnd()const { return hwnd; }
		HINSTANCE get_hinstance()const { return hinstance; }

		// ウィンドウ作成用メンバ関数
		bool create_window(
			LPCTSTR window_name,	//ウィンドウ名
			DWORD style,			//ウィンドウスタイル
			int x,					//ウィンドウの横方向の位置
			int y,					//ウィンドウの縦方向の位置
			int width,				//ウィンドウの幅
			int height,				//ウィンドウの高さ
			HWND hwnd_parent,		//親ウィンドウまたはオーナーウィンドウのハンドル
			HMENU menu				//メニューハンドルまたは子ウィンドウ ID
		);
	};


	//プリセット
	typedef struct tagPreset_Value {
		LPCTSTR name;
		POINT ct1, ct2;
	} Preset_Value;

	typedef struct Preset_ID {
		LPCTSTR name;
		Curve_ID curve;
	} Preset_ID;
}


//----------------------------------------------------------------------------------
//		extern宣言
//----------------------------------------------------------------------------------
extern ce::Curve_Value						g_curve_value;
extern ce::Curve_Value						g_curve_value_previous;
extern ce::Curve_ID							g_curve_id[CE_CURVE_MAX];
extern ce::Curve_ID							g_curve_id_previous;
extern std::vector<ce::Preset_Value>		g_presets_value;
extern std::vector<ce::Preset_ID>			g_presets_id;
extern const ce::Theme						g_theme_dark, g_theme_light;
extern ce::Theme							g_theme[2];
extern ce::Config							g_config;
extern ce::Window							g_window;
extern ce::Gr_Disp_Info						g_disp_info;
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

//初期化
BOOL				initialize(FILTER*);

//終了
BOOL				exit(FILTER*);

void				ini_load_configs(FILTER* fp);

void				ini_write_configs(FILTER* fp);

BOOL				on_project_load(FILTER* fp, void* editp, void* data, int size);

BOOL				on_project_save(FILTER* fp, void* editp, void* data, int* size);

//文字列の分割
std::vector<std::string> split(const std::string& s, TCHAR c);
//クリップボードにテキストをコピー
BOOL				copy_to_clipboard(HWND, LPCTSTR);

DoublePoint			subtract_length(DoublePoint st, DoublePoint ed, double length);

void apply_config_to_menu(HMENU menu, MENUITEMINFO minfo);

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
void				d2d_initialize();
void				d2d_setup(HDC hdc, LPRECT rect_wnd, COLORREF cr);
void				d2d_draw_bezier(ID2D1SolidColorBrush* pBrush,
					DoublePoint stpt, DoublePoint ctpt1, DoublePoint ctpt2, DoublePoint edpt, float thickness);
void				d2d_draw_handle(ID2D1SolidColorBrush* pBrush, DoublePoint stpt, DoublePoint edpt);
void				draw_main(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd, LPRECT rect_sepr);
void				draw_footer(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd);
void				draw_panel_library(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd);
void				draw_panel_editor(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd);
void				draw_panel_graph(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd);


//ダイアログプロシージャ
BOOL CALLBACK		wndproc_daialog_settings(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		wndproc_daialog_value(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		wndproc_daialog_read(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK		wndproc_daialog_save(HWND, UINT, WPARAM, LPARAM);


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


//バグ回避のテスト用
LRESULT CALLBACK	WndProc_Test(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);


//----------------------------------------------------------------------------------
//		インライン関数
//----------------------------------------------------------------------------------

//グラフ -> クライアント
inline DoublePoint to_client(int gr_x, int gr_y)
{
	return {
		g_disp_info.o.x + gr_x * g_disp_info.scale.x,
		g_disp_info.o.y - gr_y * g_disp_info.scale.y
	};
}

inline DoublePoint to_client(POINT gr_pt)
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
		(int)((cl_x - g_disp_info.o.x) / g_disp_info.scale.x),
		(int)((-cl_y + g_disp_info.o.y) / g_disp_info.scale.y)
	};
}

inline POINT to_graph(POINT cl_pt)
{
	return {
		to_graph(cl_pt.x, cl_pt.y).x,
		to_graph(cl_pt.x, cl_pt.y).y
	};
}
