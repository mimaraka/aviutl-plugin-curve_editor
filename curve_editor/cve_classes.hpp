//----------------------------------------------------------------------------------
//		Curve Editor
//		ヘッダファイル (クラス)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include "cve_structs.hpp"



namespace cve {
	//---------------------------------------------------------------------
	//		配列
	//---------------------------------------------------------------------
	template <typename T, size_t N>
	class Static_Array : public yulib::CStaticArray<T, N> {
	public:
		void PushBack(const T& v) {
			if (this->size < this->max_size) {
				this->size++;
				this->buf[this->size - 1] = v;
			}
		}
	};



	//---------------------------------------------------------------------
	//		カーブ
	//---------------------------------------------------------------------
	class Curve {
	private:
		void				set_handle_position(const Point_Address& pt_address, const POINT& pt_graph, double length, bool use_angle, double angle);
		void				correct_handle(const Point_Address& pt_address, double angle);
		double				get_handle_angle(const Point_Address& pt_address);
		void				set_handle_angle(const Point_Address& pt_address, double angle, bool use_length, double lgth);
		void				draw_bezier(
								ID2D1SolidColorBrush* brush,
								const Float_Point& stpt,
								const Float_Point& ctpt1,
								const Float_Point& ctpt2,
								const Float_Point& edpt,
								float thickness
							);
		void				draw_handle(ID2D1SolidColorBrush* brush, const Float_Point& st, const Float_Point& ed, int drawing_mode);

	public:
		// サイズ固定
		Static_Array<Curve_Points, CVE_POINT_MAX> ctpts;
		Mode mode = Mode_Value;

		Curve() { initialize(); }

		// 共通
		void				initialize();
		void				set_mode(Mode md);
		void				clear();
		void				pt_in_ctpt(const POINT& pt_client, Point_Address* pt_address);
		void				reverse_curve();
		void				draw_curve(ID2D1SolidColorBrush* brush, const RECT& rect_wnd, int drawing_mode);
		bool				is_data_valid();

		// Valueモード用
		int					create_value_1d();
		std::string			create_value_4d();
		void				read_value_1d(int value);
		void				read_ease(int value);

		// IDモード用
		void				add_point(const POINT& pt_client);
		void				delete_point(const POINT& pt_client);
		void				move_point(int index, const POINT& pt_graph, bool init);
		void				move_handle(const Point_Address& pt_address, const POINT& pt_graph, bool init);
		double				id_create_result(double ratio, double st, double ed);
	};



	//---------------------------------------------------------------------
	//		ビットマップバッファ
	//---------------------------------------------------------------------
	class Bitmap_Buffer {
	private:
		HBITMAP bitmap;
		HWND hwnd;
		RECT rect;
		ID2D1SolidColorBrush* brush = nullptr;

		void draw_grid();

	public:
		HDC hdc_memory;

		void init(HWND hw);
		void exit() const;
		void transfer() const;
		bool d2d_setup(COLORREF cr);
		void set_size(const RECT& rect_wnd);

		void draw_panel_main(const RECT& rect_sepr);
		void draw_panel_header();
		void draw_panel_preset();
		void draw_panel_editor();

		void draw_rounded_edge(int flag, float radius);
	};



	//---------------------------------------------------------------------
	//		ウィンドウ
	//---------------------------------------------------------------------
	class Window {
	protected:
		static LRESULT CALLBACK wndproc_static(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
		virtual LRESULT		wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	public:
		HWND hwnd;

		BOOL				create(
			HWND		hwnd_parent,
			LPCTSTR		class_name,
			WNDPROC		wndproc,
			LONG		window_style,
			LONG		class_style,
			const RECT& rect,
			LPVOID		lp_param
		);

		virtual void		move(const RECT& rect) const;
		void				redraw() const;
		BOOL				close() const;
		BOOL				show() const;
		BOOL				hide() const;
	};



	//---------------------------------------------------------------------
	//		ボタン
	//---------------------------------------------------------------------
	class Button : public Window {
	public:
		enum Content_Type {
			Null,
			Icon,
			String
		};

		int					id;

		virtual BOOL		initialize(
			HWND hwnd_p,
			LPCTSTR name,
			LPCTSTR desc,
			Content_Type cont_type,
			LPCTSTR ico_res_dark,
			LPCTSTR ico_res_light,
			LPCTSTR lb,
			int ct_id,
			const RECT& rect,
			int flag
		);
		
	protected:
		LPTSTR				icon_res_dark;
		LPTSTR				icon_res_light;
		Content_Type		content_type;
		LPTSTR				label;
		Bitmap_Buffer		bitmap_buffer;
		HWND				hwnd_parent;
		HWND				hwnd_tooltip;
		TOOLINFO			tool_info;
		bool				hovered, clicked;
		TRACKMOUSEEVENT		tme;
		HICON				icon_dark;
		HICON				icon_light;
		HFONT				font;
		LPTSTR				description;
		int					edge_flag;

		void				draw_content(COLORREF bg, RECT& rect_wnd, LPCTSTR content);
		//void				set_font(const RECT& rect_wnd, LPTSTR font_name);
		virtual LRESULT		wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};



	//---------------------------------------------------------------------
	//		ボタン(スイッチ)
	//---------------------------------------------------------------------
	class Button_Switch : public Button {
	private:
		bool				is_selected;

		LRESULT				wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	public:
		void				set_status(BOOL bl);
	};



	//---------------------------------------------------------------------
	//		ボタン(Value)
	//---------------------------------------------------------------------
	class Button_Value : public Button {
	private:
		LRESULT				wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};



	//---------------------------------------------------------------------
	//		ボタン(ID)
	//---------------------------------------------------------------------
	class Button_ID : public Button {
	private:
		int					id_buffer;
		POINT				pt_lock;
		bool				is_scrolling = false;
		int					coef_move;

		LRESULT				wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};



	//---------------------------------------------------------------------
	//		プリセット
	//---------------------------------------------------------------------
	class Preset : public Button {
	private:
		LRESULT				wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	public:
		char				name[CVE_PRESET_NAME_MAX];
		Curve				curve;
		time_t				unix_time;

		Preset();

		BOOL				initialize(HWND hwnd_p, const Curve* cv, LPTSTR nm);
		void				move(int panel_width, int index);
	};



	//---------------------------------------------------------------------
	//		矩形
	//---------------------------------------------------------------------
	class Rectangle {
	public:
		RECT rect;

		void set(const RECT& rc);
		void set(int left, int top, int right, int bottom);
		void set_margin(int left, int top, int right, int bottom);
		void divide(LPRECT rects_child[], float weights[], int n) const;
		void client_to_screen(HWND hwnd);
		void screen_to_client(HWND hwnd);
	};



	//---------------------------------------------------------------------
	//		オブジェクト設定ダイアログのボタン
	//---------------------------------------------------------------------
	class Obj_Dialog_Buttons {
	private:
		HWND hwnd_obj;
		HWND hwnd_button;

		BOOL id_to_rect(int id_, Rectangle* rect);

	public:
		int id = -1;
		// オブジェクト設定ダイアログのクライアント座標
		Rectangle rect_button;

		void	init(HWND hwnd) { hwnd_obj = hwnd; }
		BOOL	is_hovered() const { return id >= 0; }

		int		update(LPPOINT pt_sc, LPRECT old_rect);
		void	click();
		void	highlight() const;
		void	invalidate(const LPRECT rect) const;
	};



	//---------------------------------------------------------------------
	//		グラフ表示
	//---------------------------------------------------------------------
	class Graph_View_Info {
	public:
		Float_Point origin;
		Double_Point scale;

		void fit(const RECT& rect)
		{
			origin.x = CVE_GRAPH_PADDING;
			scale.x = ((double)rect.right - (int)(2 * CVE_GRAPH_PADDING)) / (double)CVE_GRAPH_RESOLUTION;

			if (rect.right > rect.bottom && rect.bottom > CVE_GRAPH_PADDING * 2 + CVE_GRAPH_RESOLUTION * CVE_GRAPH_SCALE_MIN) {
				origin.y = (float)(rect.bottom - CVE_GRAPH_PADDING);
				scale.y = ((double)rect.bottom - (int)(2 * CVE_GRAPH_PADDING)) / (double)CVE_GRAPH_RESOLUTION;
			}
			else {
				origin.y = (rect.bottom + rect.right) * 0.5f - CVE_GRAPH_PADDING;
				scale.y = scale.x;
			}
		}
	};
}
