//----------------------------------------------------------------------------------
//		Curve Editor
//		ヘッダファイル (クラス)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include "cve_struct.hpp"



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
	//		ビットマップバッファ
	//---------------------------------------------------------------------
	class Bitmap_Buffer {
	private:
		HBITMAP bitmap;
		HWND hwnd;

		void draw_grid();

	public:
		HDC hdc_memory;
		ID2D1SolidColorBrush* brush;
		RECT rect;

		Bitmap_Buffer();

		void init(HWND hw);
		void exit();
		void transfer() const;
		bool d2d_setup(const aului::Color& col);
		void resize();

		void draw_panel();

		void draw_rounded_edge(int flag, float radius);

		template <class Interface>
		void release(Interface** pp_resource)
		{
			if (*pp_resource != nullptr) {
				(*pp_resource)->Release();
				(*pp_resource) = nullptr;
			}
		}
	};



	//---------------------------------------------------------------------
	//		Direct2D描画オブジェクト(継承)
	//---------------------------------------------------------------------
	class Cve_Paint_Object : public aului::Direct2d_Paint_Object {
	private:
		static constexpr float SEPARATOR_LINE_WIDTH = 3.6f;
		static constexpr int SEPARATOR_LINE_LENGTH = 32;
		static constexpr float HANDLE_THICKNESS = 2.f;
		static constexpr float HANDLE_THICKNESS_PRESET = 0.8f;
		static constexpr float HANDLE_SIZE = 5.f;
		static constexpr float HANDLE_SIZE_PRESET = 0.7f;
		static constexpr float HANDLE_BORDER_THICKNESS = 2.2f;
		static constexpr float HANDLE_BORDER_THICKNESS_PRESET = 0.5f;
		static constexpr float POINT_SIZE = 4.4f;
		static constexpr float POINT_SIZE_PRESET = 2.f;

		void draw_grid();

	public:
		void draw_panel();
		void draw_panel_main(const RECT& rect_sepr);
		void draw_panel_editor();

		void draw_rounded_edge(int flag, float radius);

		void draw_bezier(
			const aului::Point<float>& stpt,
			const aului::Point<float>& ctpt1,
			const aului::Point<float>& ctpt2,
			const aului::Point<float>& edpt,
			float thickness
		);

		void draw_handle(
			const aului::Point<float>& st,
			const aului::Point<float>& ed,
			int drawing_mode,
			int draw_option
		);
	};



	//---------------------------------------------------------------------
	//		カーブ
	//---------------------------------------------------------------------
	class Curve {
	protected:
		static constexpr int POINT_BOX_WIDTH					= 10;
		static constexpr float CURVE_THICKNESS					= 1.2f;
		static constexpr double DRAW_GRAPH_STEP					= 1.;

		double				get_bezier_value(double ratio, Static_Array<Curve_Points, CVE_POINT_MAX>& points);
		void				set_handle_position(const Point_Address& pt_address, const POINT& pt_graph, double length, bool use_angle, double angle);
		void				correct_handle(const Point_Address& pt_address);

		// ハンドルの角度を取得&設定
		double				get_handle_angle(const Point_Address& pt_address);
		void				set_handle_angle(const Point_Address& pt_address, double angle, bool use_length, double lgth);

		// 描画系
		void				draw_dash_line(aului::Direct2d_Paint_Object* paint_object, const RECT& rect_wnd, int pt_idx);

	public:
		static constexpr float HANDLE_DEFAULT_1 = 0.3f;
		static constexpr float HANDLE_DEFAULT_2 = 0.7f;
		static constexpr int DRAW_CURVE_NORMAL	= 0;
		static constexpr int DRAW_CURVE_TRACE	= 1;
		static constexpr int DRAW_CURVE_PRESET	= 2;
		static constexpr int DRAW_POINT_ONLY	= 1;
		static constexpr int DRAW_HANDLE_ONLY	= 2;


		// サイズ固定
		Static_Array<Curve_Points, CVE_POINT_MAX> ctpts;

		Curve() { init(); }

		// 初期化
		virtual void		init(Static_Array<Curve_Points, CVE_POINT_MAX>& points);
		virtual void		init() { init(ctpts); }

		// ポイントを削除(&初期化)
		void				clear(Static_Array<Curve_Points, CVE_POINT_MAX>& points);
		void				clear() { clear(ctpts); }

		virtual void		pt_on_ctpt(const POINT& pt_client, Point_Address* pt_address, bool prioritize_point=false);
		virtual void		reverse_curve();
		virtual void		draw_curve(Cve_Paint_Object* paint_object, const RECT& rect_wnd, int drawing_mode);
		bool				is_data_valid();

		// 数値を読み取る
		virtual bool		read_number(int number, Static_Array<Curve_Points, CVE_POINT_MAX>& points);
		virtual bool		read_number(int number) { return read_number(number, ctpts); }

		virtual void		move_handle(const Point_Address& pt_address, const POINT& pt_graph, bool init);
	};



	//---------------------------------------------------------------------
	//		カーブ(数値タイプ)
	//---------------------------------------------------------------------
	class Curve_Type_Numeric : public Curve {};



	//---------------------------------------------------------------------
	//		カーブ(IDタイプ)
	//---------------------------------------------------------------------
	class Curve_Type_ID : public Curve {
	protected:
		static constexpr int HANDLE_LENGTH_DEFAULT = 50;

	public:
		void				add_point(const POINT& pt_client);
		void				delete_point(const POINT& pt_client);
		void				move_point(int index, const POINT& pt_graph, bool init);
	};



	//---------------------------------------------------------------------
	//		カーブ(ベジェ)
	//---------------------------------------------------------------------
	class Curve_Bezier : public Curve_Type_Numeric {
	public:
		static constexpr float MIN_Y = -2.73f;
		static constexpr float MAX_Y = 3.73f;

		int					create_number();
		double				create_result(int number, double ratio, double st, double ed);
		std::string			create_parameters();
		bool				read_parameters(LPCTSTR param, Static_Array<Curve_Points, CVE_POINT_MAX>& points);
		bool				read_parameters(LPCTSTR param) { return read_parameters(param, ctpts); }
	};



	//---------------------------------------------------------------------
	//		カーブ(ベジェ(複数))
	//---------------------------------------------------------------------
	class Curve_Bezier_Multi : public Curve_Type_ID {
	public:
		double				create_result(double ratio, double st, double ed);
	};



	//---------------------------------------------------------------------
	//		カーブ(ベジェ(値指定)))
	//---------------------------------------------------------------------
	class Curve_Bezier_Value : public Curve_Type_ID {
	public:
		double				create_result(double ratio, double st, double ed) { return 0; }
	};



	//---------------------------------------------------------------------
	//		カーブ(振動)
	//---------------------------------------------------------------------
	class Curve_Elastic : public Curve_Type_Numeric {
	private:
		static constexpr double DEF_FREQ		= 8.;
		static constexpr double DEF_DECAY		= 6.;
		static constexpr double DEF_AMP			= 1.;

		double				func_elastic(double ratio, double f, double k, double a, double st, double ed);
		double				pt_to_param(int pt_graph_val, int idx_param);
		void				param_to_pt(POINT* pt_graph, int idx_pt);

	public:
		double				ampl;
		double				freq;
		double				dec;
		bool				reverse;

		Curve_Elastic() { init(); }

		void				init();
		void				pt_on_ctpt(const POINT& pt_client, Point_Address* pt_address);
		void				move_handle(const Point_Address pt_address, const POINT& pt_graph);
		void				draw_curve(Cve_Paint_Object* paint_object, const RECT& rect_wnd, int drawing_mode);
		void				reverse_curve();
		int					create_number();
		bool				read_number(int number, double* f, double* k, double* a, bool* rev);
		bool				read_number(int number) { return read_number(number, &freq, &dec, &ampl, &reverse); }
		double				create_result(int number, double ratio, double st, double ed);
	};



	//---------------------------------------------------------------------
	//		カーブ(バウンス)
	//---------------------------------------------------------------------
	class Curve_Bounce : public Curve_Type_Numeric {
	private:
		static constexpr double DEF_COEF_BOUNCE	= 0.6;
		static constexpr double DEF_COEF_TIME	= 0.5;

		double				func_bounce(double ratio, double e, double t, double st, double ed);
		double				pt_to_param(const POINT& pt_graph, int idx_param);
		void				param_to_pt(POINT* pt_graph);

	public:
		double				coef_bounce;
		double				coef_time;
		bool				reverse;

		Curve_Bounce() { init(); }

		void				init();
		void				pt_on_ctpt(const POINT& pt_client, Point_Address* pt_address);
		void				move_handle(const POINT& pt_graph);
		void				draw_curve(Cve_Paint_Object* paint_object, const RECT& rect_wnd, int drawing_mode);
		void				reverse_curve();
		int					create_number();
		bool				read_number(int number, double* e, double* t, bool* rev);
		bool				read_number(int number) { return read_number(number, &coef_bounce, &coef_time, &reverse); }
		double				create_result(int number, double ratio, double st, double ed);
	};



	//---------------------------------------------------------------------
	//		ボタン
	//---------------------------------------------------------------------
	class Button : public aului::Window {
	public:
		static constexpr int FLAG_DISABLED = 1 << 0;
		static constexpr int FLAG_USE_ICON = 1 << 1;
		static constexpr int FLAG_EDGE_LT = 1 << 0;
		static constexpr int FLAG_EDGE_LB = 1 << 1;
		static constexpr int FLAG_EDGE_RT = 1 << 2;
		static constexpr int FLAG_EDGE_RB = 1 << 3;
		static constexpr int FLAG_EDGE_ALL = FLAG_EDGE_LT | FLAG_EDGE_LB | FLAG_EDGE_RT | FLAG_EDGE_RB;
		static constexpr int LABEL_MAX_CHAR = 64;
		enum Content_Type {
			Null,
			Icon,
			String
		};

		int					id;
		int					status;

		virtual BOOL		init(
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
		void				set_status(int flags);
		
	protected:
		LPTSTR				icon_res_dark;
		LPTSTR				icon_res_light;
		Content_Type		content_type;
		char				label[LABEL_MAX_CHAR];
		Bitmap_Buffer		bitmap_buffer;
		HWND				hwnd_parent;
		HWND				hwnd_tooltip;
		TOOLINFO			tool_info;
		bool				hovered, clicked, disabled;
		TRACKMOUSEEVENT		tme;
		HICON				icon_dark;
		HICON				icon_light;
		HFONT				font;
		LPTSTR				text_tooltip;
		int					flag_edge;

		void				draw_content(const aului::Color col_bg, RECT* rect_content, LPCTSTR content, bool change_color);
		void				draw_edge();
		void				set_font(int font_height, LPTSTR font_name);
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
	class Button_Param : public Button {
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
	//		ボタン(カテゴリー)
	//---------------------------------------------------------------------
	class Button_Category : public Button {
	private:
		bool is_expanded;

		LRESULT				wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};



	//---------------------------------------------------------------------
	//		ボタン(色選択)
	//---------------------------------------------------------------------
	class Button_Color : public Button {
	private:
		COLORREF bg_color;

		LRESULT				wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	};



	//---------------------------------------------------------------------
	//		エディットボックス
	//---------------------------------------------------------------------
	class Edit_Box : public Button {
	private:
		int					line_height;

		LRESULT				wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	public:
		HWND				editbox;

		BOOL				init(
			HWND			hwnd_p,
			LPCTSTR			name,
			int				l_height,
			int				ct_id,
			const RECT&		rect,
			int				flag
		);
	};



	//---------------------------------------------------------------------
	//		オブジェクト設定ダイアログのボタン
	//---------------------------------------------------------------------
	class Obj_Dialog_Buttons {
	private:
		HWND hwnd_obj;
		HWND hwnd_button;

		BOOL id_to_rect(int id_, aului::Window_Rectangle* rect);

	public:
		int id = -1;
		// オブジェクト設定ダイアログのクライアント座標
		aului::Window_Rectangle rect_button;

		void	init(HWND hwnd) { hwnd_obj = hwnd; }
		BOOL	is_hovered() const { return id >= 0; }

		int		update(LPPOINT pt_sc, LPRECT old_rect);
		void	click();
		void	highlight() const;
		void	invalidate(const LPRECT rect) const;
	};
}
