//----------------------------------------------------------------------------------
//		Curve Editor
//		ヘッダファイル (構造体)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include "cve_framework.hpp"



namespace cve {
	//---------------------------------------------------------------------
	//		Float Point (floatの点)
	//---------------------------------------------------------------------
	typedef struct tagFloat_Point {
		float x, y;
	} Float_Point;



	//---------------------------------------------------------------------
	//		Double Point (doubleの点)
	//---------------------------------------------------------------------
	typedef struct tagDouble_Point {
		double x, y;
	} Double_Point;



	//---------------------------------------------------------------------
	//		エディットモード
	//---------------------------------------------------------------------
	enum Edit_Mode {
		Mode_Normal,
		Mode_Multibezier,
		Mode_Elastic,
		Mode_Bounce,
		Mode_Value
	};



	//---------------------------------------------------------------------
	//		テーマ
	//---------------------------------------------------------------------
	typedef struct tagTheme {
		COLORREF	bg,
					bg_graph,
					sepr,
					curve_trace,
					curve_prset,
					handle,
					handle_preset,
					bt_selected,
					bt_unselected,
					bt_tx,
					bt_tx_selected,
					preset_tx;
	} Theme;



	//---------------------------------------------------------------------
	//		ポイント(マルチベジェモード)
	//---------------------------------------------------------------------
	typedef struct tagCurve_Points {
		enum Type {
			Default_Left,		// 初期制御点の左
			Default_Right,		// 初期制御点の右
			Extended			// 拡張制御点
		};

		POINT	pt_center,
				pt_left,
				pt_right;

		Type type;
	} Curve_Points;



	//---------------------------------------------------------------------
	//		ポイントの場所
	//---------------------------------------------------------------------
	typedef struct tagPoint_Address {
		// ポイントの位置
		enum Point_Position {
			Null,
			Center,
			Left,
			Right
		};

		int index;
		Point_Position position;
	} Point_Address;



	//---------------------------------------------------------------------
	//		設定
	//---------------------------------------------------------------------
	typedef struct tagConfig {
		enum Layout_Mode {
			Vertical,
			Horizontal
		};

		bool	alert,
				trace,
				auto_copy,
				show_handle,
				align_handle,
				is_hooked_popup,
				is_hooked_dialog;

		int		theme,
				separator,
				current_id,
				preset_size;

		Edit_Mode	edit_mode;
		Layout_Mode	layout_mode;
		COLORREF	curve_color;
	} Config;
}
