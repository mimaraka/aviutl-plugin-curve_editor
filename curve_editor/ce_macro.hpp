//----------------------------------------------------------------------------------
//		Curve Editor
//		ヘッダファイル（マクロ）
//		VC++ 2022
//----------------------------------------------------------------------------------


#define CE_MRG					8
#define CE_GR_PADDING			20
#define CE_GR_RATIO				0.9
#define CE_GR_WHEEL_RATIO		0.1
#define CE_GR_GRID_N			2
#define CE_GR_RES				10000
#define CE_GR_GRID_MIN			36
#define CE_GR_GRID_TH_L			0.5
#define CE_GR_GRID_TH_B			1
#define CE_GR_POINT_TH			0.2
#define CE_GR_POINT_DASH		30
#define CE_GR_POINT_CONTRAST	3
#define CE_MAX_W				1440
#define CE_MAX_H				1280
#define CE_DEF_W				500
#define CE_DEF_H				400
#define CE_SEPR_DEF				150
#define CE_POINT_MAX			64
#define CE_CURVE_MAX			256
#define CE_HANDLE_DEF_L			50
#define CE_HANDLE_TH			2
#define CE_HANDLE_SIZE			5.8
#define CE_HANDLE_SIRCLE_LINE	2.6
#define CE_POINT_RANGE			10
#define CE_POINT_SIZE			3.2
#define CE_OUTOFRANGE			2122746762
#define CE_GR_SCALE_INC			1.01
#define CE_GR_SCALE_MAX			512
#define CE_GR_SCALE_MIN			0.001
#define CE_SEPR_W				8
#define CE_SEPR_LINE_W			4
#define CE_SEPR_LINE_L			32
#define CE_CURVE_TH				1.2

//コントロール
#define CE_CT_APPLY				1000
#define CE_CT_MODE_VALUE		1001
#define CE_CT_MODE_ID			1002
#define CE_CT_SAVE				1003
#define CE_CT_READ				1004
#define CE_CT_ALIGN				1005
#define CE_CT_PREF				1006
#define CE_CT_PREV				1007
#define CE_CT_PREV_DUR			1008
#define CE_CT_FIT				1009
#define CE_CT_ID_BACK			1010
#define CE_CT_ID_NEXT			1011
#define CE_CT_ID				1012
#define CE_CT_DELETE			1013
#define CE_CT_APPLY_H			56
#define CE_CT_SIDE_H			44
#define CE_CT_EDT_H				30
#define CE_CT_SEARCH_H			20
#define CE_CT_ICON_SIZE			22
#define CE_CT_APPLY_SIZE		34
#define CE_CT_APPLY_TEXT		"Copy"

#define CE_WD_SIDE_MINW			120
#define CE_WD_EDT_MINW			200
#define CE_WD_PREV_W			30
#define CE_SUBTRACT_LENGTH		10

//明るさ
#define CE_BR_GRID				25
#define CE_BR_TRACE				-10
#define CE_BR_SEPR				256
#define CE_BR_GR_INVALID		-8
#define CE_CT_BR_HOVERED		15
#define CE_CT_BR_CLICKED		-15
#define CE_CT_BR_SWITCH			-20					//unselected

//メッセージ
#define CE_WM_REVERSE			2000
#define CE_WM_CHANGEID			2001
#define CE_WM_REDRAW			2002
#define CE_WM_SHOWHANDLE		2003

#define MATH_PI					3.14159265

//正規表現
#define CE_REGEX_VALUE			R"(^((\d+ *, *)|(\d*\.\d* *, *))((-?\d+ *, *)|(-?\d*\.\d* *, *))((\d+ *, *)|(\d*\.\d* *, *))((-?\d+ *)|(-?\d*\.\d* *))$)"
#define CE_REGEX_FLOW_1			R"()"
#define CE_REGEX_FLOW_2			R"(^\s*\[\s*(\{\s*"name"\s*:\s*".*"\s*,\s*"curve"\s*:\s*\[\s*(\s*-?\d\.?\d+\s*,){3}\s*-?\d\.?\d+\s*\]\s*\},)+\s*\{\s*"name"\s*:\s*".*"\s*,\s*"curve"\s*:\s*\[\s*(\s*-?\d\.?\d+\s*,){3}\s*-?\d\.?\d+\s*\]\s*\}\s*\]\s*$)"
#define CE_REGEX_CEP			R"(^(\s*\{\s*".*"(\s*\[\s*-?\d?\.?\d+\s*,\s*-?\d?\.?\d+\s*\]\s*)+\s*\}\s*)+$)"

//フィルタの情報
#define CE_FILTER_NAME				"Curve Editor"
#define CE_FILTER_VERSION			"v0.1-alpha"
#define CE_FILTER_AUTHOR			"mimaraka"
#define CE_FILTER_INFO				CE_FILTER_NAME " " CE_FILTER_VERSION " by " CE_FILTER_AUTHOR
#define CE_FILTER_LINK				"https://sites.google.com/view/aviutlscripts/home"

//関数マクロ
#define TO_BGR(ref)							(RGB(GetBValue(ref), GetGValue(ref), GetRValue(ref)))
#define DISTANCE2(pt1,pt2x, pt2y)			(std::sqrt(std::pow((pt2x) - pt1.x, 2) + std::pow((pt2y) - pt1.y, 2)))
#define DISTANCE(pt1,pt2)					(std::sqrt(std::pow(pt2.x - pt1.x, 2) + std::pow(pt2.y - pt1.y, 2)))
#define DISTANCE1(pt)						(std::sqrt(std::pow(pt.x, 2) + std::pow(pt.y, 2)))
#define MAXLIM(value,maxv)					(((value) > (maxv))? (maxv): (value))
#define MINLIM(value,minv)					(((value) < (minv))? (minv): (value))
#define MINMAXLIM(value,minv,maxv)			(((value) < (minv))? (minv): (((value) > (maxv))? (maxv): (value)))
#define ISINRANGE(value,minv,maxv)			(((value) >= (minv))? (((value) <= (maxv))? 1 : 0) : 0)
#define ISMORETHAN(value,vl)				(((value) >= (vl))? 1 : 0)
#define ISLESSTHAN(value,vl)				(((value) <= (vl))? 1 : 0)
#define BRIGHTEN(ref,num)					(RGB(MINMAXLIM(GetRValue(ref) + num, 0, 255), MINMAXLIM(GetGValue(ref) + num, 0, 255), MINMAXLIM(GetBValue(ref) + num, 0, 255)))
#define INVERT(ref)							(RGB(255 - GetRValue(ref), 255 - GetGValue(ref), 255 - GetBValue(ref)))
#define CONTRAST(ref,val)					(RGB(MINMAXLIM(127 + (GetRValue(ref) - 127) * val, 0, 255), MINMAXLIM(127 + (GetGValue(ref) - 127) * val, 0, 255), MINMAXLIM(127 + (GetBValue(ref) - 127) * val, 0, 255)))