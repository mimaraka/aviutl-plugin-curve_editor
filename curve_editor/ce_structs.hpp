//----------------------------------------------------------------------------------
//		Curve Editor
//		ヘッダファイル (構造体)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include "ce_framework.hpp"



namespace ce {
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
	//		テーマ
	//---------------------------------------------------------------------
	typedef struct tagTheme {
		COLORREF
			bg,
			bg_graph,
			sepr,
			curve,
			curve_trace,
			handle,
			bt_selected,
			bt_unselected,
			bt_tx,
			bt_tx_selected;
	} Theme;



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
	//		ポイント(IDモード)
	//---------------------------------------------------------------------
	typedef struct tagPoints_ID {
		POINT
			pt_center,
			pt_left,
			pt_right;
		int type;							//[タイプ]0:初期制御点の左; 1:初期制御点の右; 2:拡張制御点;
	} Points_ID;



	//---------------------------------------------------------------------
	//		設定
	//---------------------------------------------------------------------
	typedef struct tagConfig {
		enum Mode {
			Value,
			ID
		};

		BOOL
			alert,
			trace,
			auto_copy,
			show_handle,
			align_handle,
			is_hooked;
		int
			theme,
			separator,
			current_id,
			preset_size;
		Mode mode;
	} Config;
}
