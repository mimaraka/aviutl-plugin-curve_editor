//----------------------------------------------------------------------------------
//		Curve Editor
//		ヘッダファイル (構造体)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include "cve_framework.hpp"



namespace cve {
	//---------------------------------------------------------------------
	//		エディットモード
	//---------------------------------------------------------------------
	enum Edit_Mode {
		Mode_Bezier,
		Mode_Bezier_Multi,
		Mode_Bezier_Value,
		Mode_Elastic,
		Mode_Bounce
	};



	//---------------------------------------------------------------------
	//		テーマ
	//---------------------------------------------------------------------
	struct Theme {
		aului::Color	bg,
						bg_graph,
						bg_editbox,
						sepr,
						curve_trace,
						curve_preset,
						handle,
						handle_preset,
						bt_selected,
						bt_unselected,
						bt_tx,
						bt_tx_selected,
						preset_tx,
						editbox_tx;
	};



	//---------------------------------------------------------------------
	//		ポイント(ベジェ(複数)モード)
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
		static constexpr COLORREF CURVE_COLOR_DEFAULT = RGB(148, 158, 197);
		enum Layout_Mode {
			Vertical,
			Horizontal
		};

		enum Apply_Mode {
			Normal,
			Ignore_Mid_Point
		};

		struct Curve_ID {
			int multi = 1,
				value = 1;
		};

		bool	show_popup,
				trace,
				auto_copy,
				auto_apply,
				show_handle,
				show_bezier_deriv,
				align_handle,
				linearize,
				hooked_popup,
				hooked_dialog,
				notify_update;

		int		theme,
				separator,
				preset_size;

		Curve_ID		current_id;
		Edit_Mode		edit_mode;
		Layout_Mode		layout_mode;
		Apply_Mode		apply_mode;
		aului::Color	curve_color;

		void reset_configs()
		{
			show_popup = true;
			trace = true;
			auto_copy = false;
			auto_apply = true;
			linearize = false;
			notify_update = false;
			theme = 0;
			curve_color = CURVE_COLOR_DEFAULT;
		}
	};



	//---------------------------------------------------------------------
	//		グラフ表示
	//---------------------------------------------------------------------
	struct Graph_View_Info {
		static constexpr int GRAPH_PADDING = 18;
		static constexpr double SCALE_MIN = 0.001;
		static constexpr double SCALE_MAX = 512.;

		aului::Point<double> origin;
		aului::Point<double> scale;

		void fit(const RECT& rect)
		{
			origin.x = (double)GRAPH_PADDING;
			scale.x = ((double)rect.right - (int)(2 * GRAPH_PADDING)) / (double)CVE_GRAPH_RESOLUTION;

			if (rect.right > rect.bottom && rect.bottom > GRAPH_PADDING * 2 + CVE_GRAPH_RESOLUTION * SCALE_MIN) {
				origin.y = (double)(rect.bottom - GRAPH_PADDING);
				scale.y = ((double)rect.bottom - (int)(2 * GRAPH_PADDING)) / (double)CVE_GRAPH_RESOLUTION;
			}
			else {
				origin.y = (rect.bottom + rect.right) * 0.5 - GRAPH_PADDING;
				scale.y = scale.x;
			}
		}
	};
}
