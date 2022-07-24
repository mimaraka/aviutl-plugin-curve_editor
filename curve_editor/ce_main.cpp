//----------------------------------------------------------------------------------
//		curve_editor
//		source file (main)
//		(Visual C++ 2019)
//----------------------------------------------------------------------------------


#include "ce_header.hpp"


//---------------------------------------------------------------------
//		グローバル変数
//---------------------------------------------------------------------
ce::Curve_Value					g_cv_vl;
ce::Curve_ID					g_cv_id[CE_CURVE_MAX];
ce::Config						g_cfg;
ce::Window						g_wnd;
ce::Gr_Disp_Info				g_disp_info;
FILTER*							g_fp;
std::vector<ce::Preset_Value>	g_pres_vl;
std::vector<ce::Preset_ID>		g_pres_id;

const ce::Theme g_th_dark = {
	RGB(28,28,28),
	RGB(50,50,50),
	RGB(40,40,40),
	RGB(219,219,219),
	RGB(240,38,68),
	RGB(71,135,228),
	RGB(238,238,238),
	RGB(60,60,60),
	NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL,
	TRUE
};
const ce::Theme g_th_light = {
	RGB(32,32,32),
	RGB(50,50,50),
	RGB(45,45,45),
	RGB(259,249,249),
	RGB(64,96,222),
	RGB(71,135,228),
	RGB(238,238,238),
	RGB(60,60,60),
	NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL,
	TRUE
};
ce::Theme g_th_custom = { 0 };

ce::Theme g_theme[3] = {
	g_th_dark,
	g_th_light,
	g_th_custom
};

//Direct2D
ID2D1Factory*					g_d2d1_factory;
ID2D1DCRenderTarget*			g_render_target;

//---------------------------------------------------------------------
//		フィルタ構造体定義
//---------------------------------------------------------------------
FILTER_DLL g_filter = {
	FILTER_FLAG_ALWAYS_ACTIVE |
	FILTER_FLAG_WINDOW_SIZE |
	FILTER_FLAG_WINDOW_THICKFRAME |
	FILTER_FLAG_DISP_FILTER |
	FILTER_FLAG_EX_INFORMATION,
	CE_DEF_W,
	CE_DEF_H,
	CE_FLT_NAME,
	NULL,NULL,NULL,
	NULL,NULL,
	NULL,NULL,NULL,
	NULL,
	Init,
	Exit,
	NULL,
	wndProc_base,
	NULL,NULL,
	NULL,
	NULL,
	CE_FLT_INFO,
	NULL,NULL,
	NULL,NULL,NULL,NULL,
	NULL,
};


//---------------------------------------------------------------------
//		Lua関数
//---------------------------------------------------------------------

//処理関数
int getResult(lua_State* L)
{
	int		index	= lua_tointeger(L, 1);
	double	ratio	= lua_tonumber(L, 2);
	double	st		= lua_tonumber(L, 3);
	double	ed		= lua_tonumber(L, 4);

	if (index < 0 || index > CE_CURVE_MAX) lua_pushnumber(L, st + (ed - st) * ratio);
	else lua_pushnumber(L, g_cv_id[index].getValue(ratio, st, ed));
	return 1;
}

//関数テーブル
static luaL_Reg functions[] = {
	{ "getResult", getResult },
	{ nullptr, nullptr }
};

//登録
extern "C" __declspec(dllexport) int luaopen_curve_editor(lua_State * L)
{
	luaL_register(L, "curve_editor", functions);
	return 1;
}