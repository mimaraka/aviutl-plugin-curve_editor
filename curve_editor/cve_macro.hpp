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
#define CVE_MARGIN						6
#define CVE_MARGIN_PRESET				5
#define CVE_MAX_W						1920
#define CVE_MAX_H						1080
#define CVE_DEF_W						360
#define CVE_DEF_H						540
#define CVE_DEF_PRESET_SIZE				50
#define CVE_ROUND_RADIUS				4.0f
#define CVE_NON_CLIENT_H				36

// セパレータ
#define CVE_SEPARATOR_WIDTH				8
#define CVE_SEPARATOR_LINE_WIDTH		3.6f
#define CVE_SEPARATOR_LINE_LENGTH		32

// グラフ
#define CVE_GRAPH_PADDING				20
#define CVE_GRAPH_WHEEL_COEF_SCALE		0.1
#define CVE_GRAPH_WHEEL_COEF_POS		0.2
#define CVE_GRAPH_GRID_NUM				2
#define CVE_GRAPH_RESOLUTION			10000				// 変更不可
#define CVE_GRAPH_SCALE_BASE			1.01
#define CVE_GRAPH_SCALE_MAX				512
#define CVE_GRAPH_SCALE_MIN				0.001

// ハンドル・カーブ・制御点
#define CVE_HANDLE_LENGTH_DEFAULT		50
#define CVE_HANDLE_THICKNESS			2.0f
#define CVE_HANDLE_THICKNESS_PRESET		0.8f
#define CVE_HANDLE_SIZE					5.0f
#define CVE_HANDLE_SIZE_PRESET			0.7f
#define CVE_HANDLE_CIRCLE_LINE			2.2f
#define CVE_HANDLE_CIRCLE_LINE_PRESET	0.5f
#define CVE_POINT_RANGE					10
#define CVE_POINT_SIZE					4.4f
#define CVE_PONINT_SIZE_PRESET			2.0f
#define CVE_CURVE_THICKNESS				1.2f
#define CVE_SUBTRACT_LENGTH				10.0f
#define CVE_SUBTRACT_LENGTH_2			8.0f
#define CVE_CURVE_VALUE_MIN_Y			-2.73f
#define CVE_CURVE_VALUE_MAX_Y			3.73f
#define CVE_POINT_DEFAULT_1				0.4f
#define CVE_POINT_DEFAULT_2				0.6f

//フラグ
#define BIT(num)						((UINT)1 << num)
#define CVE_EDGE_LT						BIT(0)
#define CVE_EDGE_LB						BIT(1)
#define CVE_EDGE_RT						BIT(2)
#define CVE_EDGE_RB						BIT(3)
#define CVE_EDGE_ALL					CVE_EDGE_LT | CVE_EDGE_LB | CVE_EDGE_RT | CVE_EDGE_RB



//---------------------------------------------------------------------
//		コントロール
//---------------------------------------------------------------------
#define CVE_CT_MODE_VALUE				0x0400
#define CVE_CT_MODE_ID					0x0401
#define CVE_CT_ALIGN					0x0402
#define CVE_CT_PREV						0x0403
#define CVE_CT_PREV_DUR					0x0404
#define CVE_CT_ID_BACK					0x0405
#define CVE_CT_ID_NEXT					0x0406
#define CVE_CT_ID						0x0407
#define CVE_CT_SETSTATUS				0x0408

#define CVE_CT_UPPER_H					30
#define CVE_CT_LOWER_H					40
#define CVE_CT_FONT_H					20
#define CVE_HEADER_H						(CVE_CT_UPPER_H + CVE_CT_LOWER_H + CVE_MARGIN * 3)

#define CVE_COEF_MOVE_DEFAULT			9
#define CVE_COEF_MOVE_FAST				1



//---------------------------------------------------------------------
//		コマンド
//---------------------------------------------------------------------
#define CVE_CM_REDRAW					0x0800
#define CVE_CM_COPY						0x0801
#define CVE_CM_READ						0x0802
#define CVE_CM_SAVE						0x0803
#define CVE_CM_CLEAR					0x0804
#define CVE_CM_FIT						0x0805
#define CVE_CM_REVERSE					0x0806
#define CVE_CM_SHOWHANDLE				0x0807
#define CVE_CM_CHANGE_ID				0x0808
#define CVE_CM_SELECTED					0x0809
#define CVE_CM_ID_NEXT					0x080a
#define CVE_CM_ID_BACK					0x080b
#define CVE_CM_REDRAW_AVIUTL			0x080c
#define CVE_CM_VALUE					0x080d
#define CVE_CM_VALUE_REDRAW				0x080e



//---------------------------------------------------------------------
//		テーマ関連
//---------------------------------------------------------------------
// 明るさ
#define CVE_BR_GRID						25
#define CVE_BR_GR_INVALID				-10
#define CVE_CT_BR_HOVERED				13
#define CVE_CT_BR_CLICKED				-10
#define CVE_CT_BR_SWITCH				-20					//選択していない状態
// アイコン
#define CVE_ICON_SIZE					32
// フォント
#define CVE_FONT_YU_GOTHIC				"Yu Gothic UI Semibold"

#define CVE_DRAW_CURVE_REGULAR			0
#define CVE_DRAW_CURVE_TRACE			1
#define CVE_DRAW_CURVE_PRESET			2

#define CVE_CURVE_COLOR_DEFAULT			RGB(103, 103, 241)



//---------------------------------------------------------------------
//		その他
//---------------------------------------------------------------------
#define CVE_POINT_MAX					64					// 変更不可
#define CVE_CURVE_MAX					1024				// 変更不可
#define CVE_PRESET_MAX					2048
#define CVE_PRESET_NAME_MAX				128



//---------------------------------------------------------------------
//		プラグイン(フィルタ)の情報
//---------------------------------------------------------------------
#define CVE_PLUGIN_NAME						"Curve Editor"
#define CVE_PLUGIN_VERSION					"v0.5"
#define CVE_PLUGIN_DEVELOPER				"mimaraka"
#define CVE_PLUGIN_YEAR						"2022"
#define CVE_PLUGIN_INFO						CVE_PLUGIN_NAME " " CVE_PLUGIN_VERSION " by " CVE_PLUGIN_DEVELOPER
#define CVE_PLUGIN_LINK						"https://github.com/mimaraka/aviutl-plugin-curve_editor"
#define CVE_PLUGIN_LINK_HELP				CVE_PLUGIN_LINK "/wiki"



//---------------------------------------------------------------------
//		文字列
//---------------------------------------------------------------------
#define CVE_STR_WARNING_DELETE				"編集中のカーブを初期化します。よろしいですか？"
#define CVE_STR_WARNING_DELETE_ALL			"すべてのカーブを初期化します。よろしいですか？"
#define CVE_STR_WARNING_DATA_INVALID		"互換性のないバージョンでカーブが読み込まれたか、データが破損しています。\nすべてのカーブを初期化しますか？"
#define CVE_STR_ABOUT						CVE_PLUGIN_NAME " " CVE_PLUGIN_VERSION "\n" "Copyright : (C) " CVE_PLUGIN_YEAR " " CVE_PLUGIN_DEVELOPER
#define CVE_STR_ERROR_OUTOFRANGE			"値が範囲外です。"
#define CVE_STR_ERROR_INPUT_INVALID			"無効な入力値です。"
#define CVE_STR_ERROR_EXEDIT_NOT_FOUND		"拡張編集プラグイン(exedit.auf)が見つかりません。"



//---------------------------------------------------------------------
//		マクロ関数
//---------------------------------------------------------------------
#define TO_BGR(ref)							(RGB(GetBValue(ref), GetGValue(ref), GetRValue(ref)))
#define DISTANCE(pt1,pt2)					(std::sqrt(std::pow(pt2.x - pt1.x, 2) + std::pow(pt2.y - pt1.y, 2)))
#define MAX_LIMIT(value,maxv)				(((value) > (maxv))? (maxv): (value))
#define MIN_LIMIT(value,minv)				(((value) < (minv))? (minv): (value))
#define MINMAX_LIMIT(value,minv,maxv)		(((value) < (minv))? (minv): (((value) > (maxv))? (maxv): (value)))
#define ISINRANGE(value,minv,maxv)			(((value) > (minv))? (((value) < (maxv))? 1 : 0) : 0)
#define ISINRANGEEQ(value,minv,maxv)		(((value) >= (minv))? (((value) <= (maxv))? 1 : 0) : 0)
#define CHANGE_BRIGHTNESS(ref,num)			(RGB(MINMAX_LIMIT(GetRValue(ref) + num, 0, 255), MINMAX_LIMIT(GetGValue(ref) + num, 0, 255), MINMAX_LIMIT(GetBValue(ref) + num, 0, 255)))
#define INVERT(ref)							(RGB(255 - GetRValue(ref), 255 - GetGValue(ref), 255 - GetBValue(ref)))
#define CONTRAST(ref,val)					(RGB(MINMAX_LIMIT(127 + (GetRValue(ref) - 127) * val, 0, 255), MINMAX_LIMIT(127 + (GetGValue(ref) - 127) * val, 0, 255), MINMAX_LIMIT(127 + (GetBValue(ref) - 127) * val, 0, 255)))
#define LARGER(val1, val2)					(((val1) > (val2))? (val1) : (val2))
#define SMALLER(val1, val2)					(((val1) < (val2))? (val1) : (val2))