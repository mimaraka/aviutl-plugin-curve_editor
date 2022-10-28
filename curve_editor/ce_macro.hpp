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
#define CE_MARGIN						6
#define CE_MARGIN_PRESET				5
#define CE_MAX_W						1920
#define CE_MAX_H						1080
#define CE_DEF_W						360
#define CE_DEF_H						460
#define CE_DEF_PRESET_SIZE				50
#define CE_ROUND_RADIUS					4.0f
#define CE_NON_CLIENT_H					36

// セパレータ
#define CE_SEPR_W						8
#define CE_SEPR_LINE_W					3.6f
#define CE_SEPR_LINE_L					32

// グラフ
#define CE_GR_PADDING					20
#define CE_GR_RATIO						0.9
#define CE_GR_WHEEL_COEF_SCALE			0.1
#define CE_GR_WHEEL_COEF_POS			0.2
#define CE_GR_GRID_N					2
#define CE_GR_RESOLUTION				10000				// 変更不可
#define CE_GR_SCALE_INC					1.01
#define CE_GR_SCALE_MAX					512
#define CE_GR_SCALE_MIN					0.001

// ハンドル・カーブ・制御点
#define CE_HANDLE_DEF_L					50
#define CE_HANDLE_TH					2.0f
#define CE_HANDLE_TH_PRESET				1.0f
#define CE_HANDLE_SIZE					5.8f
#define CE_HANDLE_SIZE_PRESET			2.0f
#define CE_HANDLE_CIRCLE_LINE			2.6f
#define CE_HANDLE_CIRCLE_LINE_PRESET	1.0f
#define CE_POINT_RANGE					10
#define CE_POINT_SIZE					5.0f
#define CE_PONINT_SIZE_PRESET			2.0f
#define CE_CURVE_TH						1.2f
#define CE_SUBTRACT_LENGTH				10.0f
#define CE_SUBTRACT_LENGTH_2			8.0f
#define CE_CURVE_VALUE_MIN_Y			-2.73f
#define CE_CURVE_VALUE_MAX_Y			3.73f
#define CE_CURVE_DEF_1					0.4f
#define CE_CURVE_DEF_2					0.6f

//フラグ
#define BIT(num)						((UINT)1 << num)
#define CE_EDGE_LT						BIT(0)
#define CE_EDGE_LB						BIT(1)
#define CE_EDGE_RT						BIT(2)
#define CE_EDGE_RB						BIT(3)
#define CE_EDGE_ALL						CE_EDGE_LT | CE_EDGE_LB | CE_EDGE_RT | CE_EDGE_RB



//---------------------------------------------------------------------
//		コントロール
//---------------------------------------------------------------------
#define CE_CT_MODE_VALUE				0x0400
#define CE_CT_MODE_ID					0x0401
#define CE_CT_ALIGN						0x0402
#define CE_CT_PREV						0x0403
#define CE_CT_PREV_DUR					0x0404
#define CE_CT_ID_BACK					0x0405
#define CE_CT_ID_NEXT					0x0406
#define CE_CT_ID						0x0407
#define CE_CT_SETSTATUS					0x0408

#define CE_CT_UPPER_H					30
#define CE_CT_LOWER_H					40
#define CE_CT_FONT_H					20
#define CE_HEADER_H						(CE_CT_UPPER_H + CE_CT_LOWER_H + CE_MARGIN * 3)

#define CE_COEF_MOVE_DEFAULT			9
#define CE_COEF_MOVE_FAST				1



//---------------------------------------------------------------------
//		コマンド
//---------------------------------------------------------------------
#define CE_CM_REDRAW					0x0800
#define CE_CM_COPY						0x0801
#define CE_CM_READ						0x0802
#define CE_CM_SAVE						0x0803
#define CE_CM_CLEAR						0x0804
#define CE_CM_FIT						0x0805
#define CE_CM_REVERSE					0x0806
#define CE_CM_SHOWHANDLE				0x0807
#define CE_CM_CHANGE_ID					0x0808
#define CE_CM_SELECTED					0x0809
#define CE_CM_ID_NEXT					0x080a
#define CE_CM_ID_BACK					0x080b
#define CE_CM_REDRAW_AVIUTL				0x080c
#define CE_CM_VALUE						0x080d
#define CE_CM_VALUE_REDRAW				0x080e



//---------------------------------------------------------------------
//		テーマ関連
//---------------------------------------------------------------------
// 明るさ
#define CE_BR_GRID						25
#define CE_BR_GR_INVALID				-10
#define CE_CT_BR_HOVERED				13
#define CE_CT_BR_CLICKED				-10
#define CE_CT_BR_SWITCH					-20					//選択していない状態
// アイコン
#define CE_ICON_SIZE					32
// フォント
#define CE_FONT_YU_GOTHIC				"Yu Gothic UI Semibold"

#define CE_DRAW_CURVE_REGULAR			0
#define CE_DRAW_CURVE_TRACE				1
#define CE_DRAW_CURVE_PRESET			2



//---------------------------------------------------------------------
//		その他
//---------------------------------------------------------------------
#define CE_POINT_MAX					64					// 変更不可
#define CE_CURVE_MAX					1024				// 変更不可
#define MATH_PI							3.14159265



//---------------------------------------------------------------------
//		プラグイン(フィルタ)の情報
//---------------------------------------------------------------------
#define CE_PLUGIN_NAME						"Curve Editor"
#define CE_PLUGIN_VERSION					"v0.5"
#define CE_PLUGIN_DEVELOPER					"mimaraka"
#define CE_PLUGIN_YEAR						"2022"
#define CE_PLUGIN_INFO						CE_PLUGIN_NAME " " CE_PLUGIN_VERSION " by " CE_PLUGIN_DEVELOPER
#define CE_PLUGIN_LINK						"https://github.com/mimaraka/aviutl-plugin-curve_editor"
#define CE_PLUGIN_LINK_HELP					CE_PLUGIN_LINK "#readme"



//---------------------------------------------------------------------
//		文字列
//---------------------------------------------------------------------
#define CE_STR_WARNING_DELETE				"編集中のカーブを初期化します。よろしいですか？"
#define CE_STR_WARNING_DELETE_ALL			"すべてのカーブを初期化します。よろしいですか？"
#define CE_STR_ABOUT						CE_PLUGIN_NAME " " CE_PLUGIN_VERSION "\n" "Copyright : (C) " CE_PLUGIN_YEAR " " CE_PLUGIN_DEVELOPER
#define CE_STR_ERROR_INPUTANAME				"プリセット名を入力してください。"
#define CE_STR_ERROR_OUTOFRANGE				"値が範囲外です。"
#define CE_STR_ERROR_INPUT_INVALID			"無効な入力値です。"
#define CE_STR_ERROR_DATA_INVALID			"互換性のないバージョンでカーブが読み込まれたか、データが破損しています。\nすべてのカーブを初期化しますか？"
#define CE_STR_ERROR_EXEDIT_NOT_FOUND		"拡張編集(exedit.auf)が見つかりません。"



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
#define BRIGHTEN(ref,num)					(RGB(MINMAX_LIMIT(GetRValue(ref) + num, 0, 255), MINMAX_LIMIT(GetGValue(ref) + num, 0, 255), MINMAX_LIMIT(GetBValue(ref) + num, 0, 255)))
#define INVERT(ref)							(RGB(255 - GetRValue(ref), 255 - GetGValue(ref), 255 - GetBValue(ref)))
#define CONTRAST(ref,val)					(RGB(MINMAX_LIMIT(127 + (GetRValue(ref) - 127) * val, 0, 255), MINMAX_LIMIT(127 + (GetGValue(ref) - 127) * val, 0, 255), MINMAX_LIMIT(127 + (GetBValue(ref) - 127) * val, 0, 255)))
#define LARGER(val1, val2)					(((val1) > (val2))? (val1) : (val2))
#define SMALLER(val1, val2)					(((val1) < (val2))? (val1) : (val2))