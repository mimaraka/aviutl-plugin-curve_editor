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
#define CVE_MARGIN_CONTROL				3
#define CVE_MARGIN_PRESET				8
#define CVE_MAX_W						1920
#define CVE_MAX_H						1080
#define CVE_MIN_W						100
#define CVE_MIN_H						200
#define CVE_DEF_W						320
#define CVE_DEF_H						540
#define CVE_ROUND_RADIUS				4.f
// セパレータ
#define CVE_SEPARATOR_WIDTH				8

// グラフ
#define CVE_GRAPH_GRID_NUM				2
#define CVE_GRAPH_RESOLUTION			10000				// 変更不可

// ハンドル・カーブ・制御点
#define CVE_SUBTRACT_LENGTH				10.f
#define CVE_SUBTRACT_LENGTH_2			8.f

#define CVE_NUM_BEZIER_LINER			145674282
#define CVE_NUM_ELASTIC_DEFAULT			1575600
#define CVE_NUM_BOUNCE_DEFAULT			10612242



//---------------------------------------------------------------------
//		コントロール
//---------------------------------------------------------------------
#define CVE_CT_EDIT_MODE_BEZIER			0x0400
#define CVE_CT_EDIT_MODE_MULTIBEZIER	0x0401
#define CVE_CT_EDIT_MODE_VALUE			0x0402
#define CVE_CT_EDIT_MODE_ELASTIC		0x0403
#define CVE_CT_EDIT_MODE_BOUNCE			0x0404

#define CVE_CT_ALIGN					0x0410
#define CVE_CT_PREV						0x0411
#define CVE_CT_PREV_DUR					0x0412
#define CVE_CT_ID_BACK					0x0413
#define CVE_CT_ID_NEXT					0x0414
#define CVE_CT_ID						0x0415
#define CVE_CT_SETSTATUS				0x0416
#define CVE_CT_COLOR_CURVE				0x0417
#define CVE_CT_EDIT_MODE				0x0418
#define CVE_CT_SEARCHBAR				0x0419

#define CVE_CT_UPPER_H					30
#define CVE_CT_LOWER_H					30
#define CVE_CT_FONT_H					20
#define CVE_MENU_H						(CVE_CT_UPPER_H + CVE_CT_LOWER_H + CVE_MARGIN * 2 + CVE_MARGIN_CONTROL)



//---------------------------------------------------------------------
//		コマンド
//---------------------------------------------------------------------
#define CVE_CM_COPY						0x0801
#define CVE_CM_READ						0x0802
#define CVE_CM_SAVE						0x0803
#define CVE_CM_CLEAR					0x0804
#define CVE_CM_FIT						0x0805
#define CVE_CM_REVERSE					0x0806
#define CVE_CM_SHOW_HANDLE				0x0807
#define CVE_CM_CHANGE_ID				0x0808
#define CVE_CM_SELECTED					0x0809
#define CVE_CM_ID_NEXT					0x080a
#define CVE_CM_ID_BACK					0x080b
#define CVE_CM_REDRAW_AVIUTL			0x080c
#define CVE_CM_PARAM					0x080d
#define CVE_CM_PARAM_REDRAW				0x080e
#define CVE_CM_PRESET_MOVE				0x080f
#define CVE_CM_CHANGE_COLOR				0x0810
#define CVE_CM_CHANGE_LABEL				0x0811
#define CVE_CM_SET_STATUS				0x0812
#define CVE_CM_GET_EXEDIT				0x0813



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
#define CVE_FONT_REGULAR				"Yu Gothic UI Regular"
#define CVE_FONT_SEMIBOLD				"Yu Gothic UI Semibold"



//---------------------------------------------------------------------
//		ライブラリ・プリセット
//---------------------------------------------------------------------
#define CVE_DEF_PRESET_SIZE				50
#define CVE_PRESET_NUM_DEFAULT			40
#define CVE_PRESET_NUM_CUSTOM			1024
#define CVE_PRESET_NAME_MAX				128
#define CVE_LIBRARY_SEARCHBAR_HEIGHT	30
#define CVE_PRESET_TITLE_HEIGHT			16



//---------------------------------------------------------------------
//		その他
//---------------------------------------------------------------------
#define CVE_POINT_MAX					64					// 変更不可
#define CVE_CURVE_MAX					1024				// 変更不可
#define CVE_VALUE_RESOLUTION			1000



//---------------------------------------------------------------------
//		プラグイン(フィルタ)の情報
//---------------------------------------------------------------------
#define CVE_FILTER_NAME						"Curve Editor"
#define CVE_FILTER_VERSION					"v1.0.6"
#define CVE_VERSION_MAJOR					1
#define CVE_VERSION_MINOR					0
#define CVE_VERSION_REVISION				6
#define CVE_FILTER_DEVELOPER				"mimaraka"
#define CVE_FILTER_YEAR						"2022"
#define CVE_FILTER_INFO						CVE_FILTER_NAME " " CVE_FILTER_VERSION " by " CVE_FILTER_DEVELOPER
#define CVE_FILTER_LINK						"https://github.com/mimaraka/aviutl-plugin-curve_editor"
#define CVE_FILTER_LINK_HELP				CVE_FILTER_LINK "/wiki"
#define CVE_FILTER_LINK_FORM				"https://forms.gle/mhv96DSYVhhKPkYQ8"
#define CVE_FILTER_LINK_LICENSE				"https://github.com/mimaraka/aviutl-plugin-curve_editor/blob/master/LICENSE"



//---------------------------------------------------------------------
//		文字列
//---------------------------------------------------------------------
#define CVE_STR_MODE_BEZIER					"ベジェ"
#define CVE_STR_MODE_BEZIER_MULTI			"ベジェ(複数)"
#define CVE_STR_MODE_BEZIER_VALUE			"ベジェ(値指定)"
#define CVE_STR_MODE_ELASTIC				"振動"
#define CVE_STR_MODE_BOUNCE					"バウンス"

#define CVE_STR_WARNING_DELETE				"編集中のカーブを初期化します。よろしいですか？"
#define CVE_STR_WARNING_DELETE_ALL			"すべてのカーブを初期化します。よろしいですか？"
#define CVE_STR_WARNING_RESET_CONFIGS		"すべての設定を初期化します。よろしいですか？"
#define CVE_STR_WARNING_DATA_INVALID		"互換性のないバージョンでカーブが読み込まれたか、データが破損しています。\nすべてのカーブを初期化しますか？"
#define CVE_STR_ABOUT						CVE_FILTER_NAME " " CVE_FILTER_VERSION "\n(" CVE_FILTER_YEAR " " CVE_FILTER_DEVELOPER ")"
#define CVE_STR_ERROR_OUTOFRANGE			"値が範囲外です。"
#define CVE_STR_ERROR_INPUT_INVALID			"無効な入力値です。"
#define CVE_STR_ERROR_EXEDIT_NOT_FOUND		"拡張編集プラグイン(exedit.auf)が見つかりません。"
#define CVE_STR_INFO_LATEST_VERSION			"お使いのバージョン (" CVE_FILTER_VERSION ") は最新です。"
#define CVE_STR_ERROR_CONNECTION_FAILED		"接続に失敗しました。"
#define CVE_STR_ERROR_D2D_INIT_FAILED		"Direct2Dの初期化に失敗しました。"



//---------------------------------------------------------------------
//		aviutl.iniに保存するキー
//---------------------------------------------------------------------
#define CVE_INI_KEY_MODE_EDIT				"edit_mode"
#define CVE_INI_KEY_MODE_APPLY				"apply_mode"
#define CVE_INI_KEY_MODE_LAYOUT				"layout_mode"
#define CVE_INI_KEY_THEME					"theme"
#define CVE_INI_KEY_SHOW_TRACE				"show_trace"
#define CVE_INI_KEY_SHOW_BEZIER_DERIV		"show_bezier_deriv"
#define CVE_INI_KEY_SHOW_POPUP				"show_popup"
#define CVE_INI_KEY_AUTO_COPY				"auto_copy"
#define CVE_INI_KEY_AUTO_APPLY				"auto_apply"
#define CVE_INI_KEY_LINEARIZE				"linearize"
#define CVE_INI_KEY_SEPARATOR				"separator"
#define CVE_INI_KEY_ALIGN_HANDLE			"align_handle"
#define CVE_INI_KEY_SHOW_HANDLE				"show_handle"
#define CVE_INI_KEY_PRESET_SIZE				"preset_size"
#define CVE_INI_KEY_CURVE_COLOR				"curve_color"
#define CVE_INI_KEY_NOTIFY_UPDATE			"notify_update"
#define CVE_INI_KEY_NUM_BEZIER				"num_bezier"
#define CVE_INI_KEY_NUM_ELASTIC				"num_elastic"
#define CVE_INI_KEY_NUM_BOUNCE				"num_bounce"