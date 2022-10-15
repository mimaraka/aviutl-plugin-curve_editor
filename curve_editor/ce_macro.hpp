//----------------------------------------------------------------------------------
//		Curve Editor
//		ヘッダファイル（マクロ）
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once



//---------------------------------------------------------------------
//		レイアウト関連
//---------------------------------------------------------------------
// ウィンドウ
#define CE_MARGIN					6
#define CE_MARGIN_PRESET			5
#define CE_MAX_W					1440
#define CE_MAX_H					1080
#define CE_DEF_W					360
#define CE_DEF_H					460
#define CE_DEF_PRESET_SIZE			50
#define CE_ROUND_RADIUS				4.0f
#define CE_NON_CLIENT_H				20

// セパレータ
#define CE_SEPR_W					8
#define CE_SEPR_LINE_W				3.6f
#define CE_SEPR_LINE_L				32

// グラフ
#define CE_GR_PADDING				20
#define CE_GR_RATIO					0.9
#define CE_GR_WHEEL_COEF_SCALE		0.1
#define CE_GR_WHEEL_COEF_POS		0.2
#define CE_GR_GRID_N				2
#define CE_GR_RESOLUTION			10000				// 変更不可
#define CE_GR_GRID_MIN				36
#define CE_GR_GRID_TH_L				0.5f
#define CE_GR_GRID_TH_B				1.0f
#define CE_GR_POINT_TH				0.2f
#define CE_GR_POINT_DASH			30
#define CE_GR_POINT_CONTRAST		3
#define CE_GR_SCALE_INC				1.01
#define CE_GR_SCALE_MAX				512
#define CE_GR_SCALE_MIN				0.001

// ハンドル・カーブ・制御点
#define CE_HANDLE_DEF_L				50
#define CE_HANDLE_TH				2
#define CE_HANDLE_SIZE				5.8
#define CE_HANDLE_SIRCLE_LINE		2.6
#define CE_POINT_RANGE				10
#define CE_POINT_SIZE				5
#define CE_CURVE_TH					1.2f
#define CE_SUBTRACT_LENGTH			10.0f
#define CE_SUBTRACT_LENGTH_2		8.0f

//フラグ
#define BIT(num)					((UINT)1 << num)
#define CE_EDGE_LT					BIT(0)
#define CE_EDGE_LB					BIT(1)
#define CE_EDGE_RT					BIT(2)
#define CE_EDGE_RB					BIT(3)
#define CE_EDGE_ALL					CE_EDGE_LT | CE_EDGE_LB | CE_EDGE_RT | CE_EDGE_RB



//---------------------------------------------------------------------
//		コントロール
//---------------------------------------------------------------------
#define CE_CT_MODE_VALUE			0x0400
#define CE_CT_MODE_ID				0x0401
#define CE_CT_ALIGN					0x0402
#define CE_CT_PREV					0x0403
#define CE_CT_PREV_DUR				0x0404
#define CE_CT_ID_BACK				0x0405
#define CE_CT_ID_NEXT				0x0406
#define CE_CT_ID					0x0407
#define CE_CT_SETSTATUS				0x0408

#define CE_CT_UPPER_H				30
#define CE_CT_LOWER_H				40
#define CE_CT_FONT_H				20
#define CE_HEADER_H					CE_CT_UPPER_H + CE_CT_LOWER_H + CE_MARGIN * 3



//---------------------------------------------------------------------
//		コマンド
//---------------------------------------------------------------------
#define CE_CM_REDRAW				0x0800
#define CE_CM_COPY					0x0801
#define CE_CM_READ					0x0802
#define CE_CM_SAVE					0x0803
#define CE_CM_CLEAR					0x0804
#define CE_CM_FIT					0x0805
#define CE_CM_REVERSE				0x0806
#define CE_CM_SHOWHANDLE			0x0807
#define CE_CM_CHANGEID				0x0808
#define CE_CM_SELECTED				0x0809
#define CE_CM_ID_NEXT				0x080a
#define CE_CM_ID_BACK				0x080b



//---------------------------------------------------------------------
//		テーマ関連
//---------------------------------------------------------------------
// 明るさ
#define CE_BR_GRID					25
#define CE_BR_GR_INVALID			-10
#define CE_CT_BR_HOVERED			13
#define CE_CT_BR_CLICKED			-10
#define CE_CT_BR_SWITCH				-20					//選択していない状態
// アイコン
#define CE_ICON_SIZE				32
// フォント
#define CE_FONT_YU					"Yu Gothic UI Semibold"



//---------------------------------------------------------------------
//		その他
//---------------------------------------------------------------------
#define CE_POINT_MAX				64					// 変更不可
#define CE_CURVE_MAX				1024				// 変更不可
#define MATH_PI						3.14159265



//---------------------------------------------------------------------
//		正規表現
//---------------------------------------------------------------------
#define CE_REGEX_VALUE				R"(^((\d+ *, *)|(\d*\.\d* *, *))((-?\d+ *, *)|(-?\d*\.\d* *, *))((\d+ *, *)|(\d*\.\d* *, *))((-?\d+ *)|(-?\d*\.\d* *))$)"
#define CE_REGEX_FLOW_1				R"()"
#define CE_REGEX_FLOW_2				R"(^\s*\[\s*(\{\s*"name"\s*:\s*".*"\s*,\s*"curve"\s*:\s*\[\s*(\s*-?\d\.?\d+\s*,){3}\s*-?\d\.?\d+\s*\]\s*\},)+\s*\{\s*"name"\s*:\s*".*"\s*,\s*"curve"\s*:\s*\[\s*(\s*-?\d\.?\d+\s*,){3}\s*-?\d\.?\d+\s*\]\s*\}\s*\]\s*$)"
#define CE_REGEX_CEP				R"(^(\s*\{\s*".*"(\s*\[\s*-?\d?\.?\d+\s*,\s*-?\d?\.?\d+\s*\]\s*)+\s*\}\s*)+$)"



//---------------------------------------------------------------------
//		プラグイン(フィルタ)の情報
//---------------------------------------------------------------------
#define CE_PLUGIN_NAME				"Curve Editor"
#define CE_PLUGIN_VERSION			"v0.3"
#define CE_PLUGIN_AUTHOR			"mimaraka"
#define CE_PLUGIN_YEAR				"2022"
#define CE_PLUGIN_INFO				CE_PLUGIN_NAME " " CE_PLUGIN_VERSION " by " CE_PLUGIN_AUTHOR
#define CE_PLUGIN_LINK				"https://github.com/mimaraka/aviutl-plugin-curve_editor"
#define CE_PLUGIN_LINK_HELP			CE_PLUGIN_LINK "#readme"



//---------------------------------------------------------------------
//		文字列
//---------------------------------------------------------------------
#define CE_STR_DELETE				"編集中のカーブを初期化します。よろしいですか？"
#define CE_STR_ABOUT				CE_PLUGIN_NAME " " CE_PLUGIN_VERSION "\n" "Copyright : (C) " CE_PLUGIN_YEAR " " CE_PLUGIN_AUTHOR
#define CE_STR_INVALIDINPUT			"無効な入力値です。"
#define CE_STR_INPUTANAME			"プリセット名を入力してください。"
#define CE_STR_OUTOFRANGE			"値が範囲外です。"



//---------------------------------------------------------------------
//		マクロ関数
//---------------------------------------------------------------------
#define TO_BGR(ref)							(RGB(GetBValue(ref), GetGValue(ref), GetRValue(ref)))
#define DISTANCE2(pt1,pt2x, pt2y)			(std::sqrt(std::pow((pt2x) - pt1.x, 2) + std::pow((pt2y) - pt1.y, 2)))
#define DISTANCE(pt1,pt2)					(std::sqrt(std::pow(pt2.x - pt1.x, 2) + std::pow(pt2.y - pt1.y, 2)))
#define DISTANCE1(pt)						(std::sqrt(std::pow(pt.x, 2) + std::pow(pt.y, 2)))
#define MAXLIM(value,maxv)					(((value) > (maxv))? (maxv): (value))
#define MINLIM(value,minv)					(((value) < (minv))? (minv): (value))
#define MINMAXLIM(value,minv,maxv)			(((value) < (minv))? (minv): (((value) > (maxv))? (maxv): (value)))
#define ISINRANGE(value,minv,maxv)			(((value) > (minv))? (((value) < (maxv))? 1 : 0) : 0)
#define ISINRANGEEQ(value,minv,maxv)		(((value) >= (minv))? (((value) <= (maxv))? 1 : 0) : 0)
#define ISMORETHAN(value,vl)				(((value) >= (vl))? 1 : 0)
#define ISLESSTHAN(value,vl)				(((value) <= (vl))? 1 : 0)
#define BRIGHTEN(ref,num)					(RGB(MINMAXLIM(GetRValue(ref) + num, 0, 255), MINMAXLIM(GetGValue(ref) + num, 0, 255), MINMAXLIM(GetBValue(ref) + num, 0, 255)))
#define INVERT(ref)							(RGB(255 - GetRValue(ref), 255 - GetGValue(ref), 255 - GetBValue(ref)))
#define CONTRAST(ref,val)					(RGB(MINMAXLIM(127 + (GetRValue(ref) - 127) * val, 0, 255), MINMAXLIM(127 + (GetGValue(ref) - 127) * val, 0, 255), MINMAXLIM(127 + (GetBValue(ref) - 127) * val, 0, 255)))