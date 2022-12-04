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
	struct Float_Point {
		float x, y;
	};



	//---------------------------------------------------------------------
	//		Double Point (doubleの点)
	//---------------------------------------------------------------------
	struct Double_Point {
		double x, y;
	};



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
	struct Theme {
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
	};



	//---------------------------------------------------------------------
	//		ポイント(マルチベジェモード)
	//---------------------------------------------------------------------
	struct Curve_Points {
		enum Type {
			Default_Left,		// 初期制御点の左
			Default_Right,		// 初期制御点の右
			Extended			// 拡張制御点
		};

		POINT	pt_center,
				pt_left,
				pt_right;

		Type type;
	};



	//---------------------------------------------------------------------
	//		ポイントの場所
	//---------------------------------------------------------------------
	struct Point_Address {
		// ポイントの位置
		enum Point_Position {
			Null,
			Center,
			Left,
			Right
		};

		int index;
		Point_Position position;
	};



	//---------------------------------------------------------------------
	//		設定
	//---------------------------------------------------------------------
	struct Config {
		enum Layout_Mode {
			Vertical,
			Horizontal
		};

		struct Curve_ID {
			int multibezier = 0,
				value = 0;
		};

		bool	alert,
				trace,
				auto_copy,
				show_handle,
				align_handle,
				is_hooked_popup,
				is_hooked_dialog,
				notify_latest_version;

		int		theme,
				separator,
				preset_size;

		Curve_ID	current_id;
		Edit_Mode	edit_mode;
		Layout_Mode	layout_mode;
		COLORREF	curve_color;
	};
}
