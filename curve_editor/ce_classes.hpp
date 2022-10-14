//----------------------------------------------------------------------------------
//		Curve Editor
//		ヘッダファイル (クラス)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include "ce_structs.hpp"



namespace ce {
	//---------------------------------------------------------------------
	//		配列
	//---------------------------------------------------------------------
	template <typename T, size_t N>
	class Static_Array {
	public:
		size_t size;
		static const size_t max_size = N;
		T elements[N];

		Static_Array() : size(0) {}

		template <typename U>
		T& operator[] (U i) { return elements[MINMAXLIM(i, 0, N - 1)]; }

		// クリア
		void clear()
		{
			size = 0;
		}

		// 挿入
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

		// 押し込み
		void push_back(const T& v)
		{
			if (size < max_size) {
				size++;
				elements[size - 1] = v;
			}
		}

		// 削除
		void erase(int index)
		{
			size--;
			for (int i = index; i < (int)size; i++) {
				elements[i] = elements[i + 1];
			}
		}
	};



	//---------------------------------------------------------------------
	//		カーブ(Valueモード)
	//---------------------------------------------------------------------
	class Curve_Value {
	public:
		POINT ctpt[2];

		Curve_Value() { init(); }

		void				init();
		int					point_in_ctpts(POINT cl_pt);
		void				move_point(int index, POINT gr_pt);
		int					create_value_1d();
		std::string			create_value_4d();
		void				read_value_1d(int value);
	};



	//---------------------------------------------------------------------
	//		カーブ(IDモード)
	//---------------------------------------------------------------------
	class Curve_ID {
	public:
		Static_Array <Points_ID, CE_POINT_MAX> ctpts;

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



	//---------------------------------------------------------------------
	//		ビットマップキャンバス
	//---------------------------------------------------------------------
	class Bitmap_Canvas {
	private:
		HBITMAP bitmap;
		HWND hwnd;
	public:
		HDC hdc_memory;

		void init(HWND hw);
		void exit();
		void transfer(LPRECT rect);
	};



	//---------------------------------------------------------------------
	//		ウィンドウ
	//---------------------------------------------------------------------
	class Window {
	protected:
		static LRESULT CALLBACK wndproc_static(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	public:
		HWND hwnd;

		virtual BOOL		create(HWND hwnd_parent, LPTSTR class_name, WNDPROC wndproc, LONG style, LPRECT rect);
		virtual void		move(LPRECT rect);
		void				redraw();
		BOOL				close();
		virtual LRESULT		wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};



	//---------------------------------------------------------------------
	//		プリセット
	//---------------------------------------------------------------------
	class Preset : public Window {
	public:
		HWND				hwnd;
		LPTSTR				name;
		Curve_Value			curve_value;
		Curve_ID			curve_id;
		time_t				unix_time;
		const int			val_or_id, def_or_user;
		int					index;

		Preset(int v_i, Curve_Value c_value, Curve_ID c_id, LPTSTR n, int d_u) : name(n), val_or_id(v_i), def_or_user(d_u)
		{
			if (v_i == 1)
				curve_id = c_id;
			else
				curve_value = c_value;
			time(&unix_time);
		}

		BOOL				create(HWND hwnd_parent);
		void				move(int width);
		LRESULT				wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};



	//---------------------------------------------------------------------
	//		ボタン
	//---------------------------------------------------------------------
	class Button : public Window {
	protected:
		LPTSTR				icon_res_dark;
		LPTSTR				icon_res_light;
		int					icon_or_str;			//0: アイコン, 1: 文字列
		LPTSTR				label;
		Bitmap_Canvas		canvas;
		HWND				hwnd_parent;
		HWND				hwnd_tooltip;
		TOOLINFO			tool_info;
		BOOL				hovered, clicked;
		TRACKMOUSEEVENT		tme;
		HICON				icon_dark;
		HICON				icon_light;
		HFONT				font;
		LPTSTR				description;
		int					edge_flag;
		ID2D1SolidColorBrush* brush;

		void				draw(COLORREF bg, LPRECT rect_wnd);

	public:
		int					id;

		BOOL				create(
								HWND hwnd_p,
								LPTSTR name,
								LPTSTR desc,
								int ic_or_str,
								LPTSTR ico_res_dark,
								LPTSTR ico_res_light,
								LPTSTR lb,
								int ct_id,
								LPRECT rect,
								int flag
							);
		virtual LRESULT		wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};



	//---------------------------------------------------------------------
	//		ボタン(スイッチ)
	//---------------------------------------------------------------------
	class Button_Switch : public Button {
	private:
		BOOL				is_selected;

	public:
		void				set_status(BOOL bl);
		LRESULT				wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};



	//---------------------------------------------------------------------
	//		グラフ表示
	//---------------------------------------------------------------------
	class Graph_View_Info {
	public:
		Float_Point origin;
		Double_Point scale;

		void fit(LPRECT rect)
		{
			origin.x = CE_GR_PADDING;
			scale.x = ((double)rect->right - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RESOLUTION;
			if (rect->right <= rect->bottom) {
				origin.y = (rect->bottom + rect->right) * 0.5f - CE_GR_PADDING;
				scale.y = scale.x;
			}
			else {
				origin.y = (float)(rect->bottom - CE_GR_PADDING);
				scale.y = ((double)rect->bottom - (int)(2 * CE_GR_PADDING)) / (double)CE_GR_RESOLUTION;
			}
		}
	};
}
