//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (メイン)
//		(Visual C++ 2022)
//----------------------------------------------------------------------------------


#include "ce_header.hpp"


//---------------------------------------------------------------------
//		グローバル変数
//---------------------------------------------------------------------
ce::Curve_Value					g_curve_value;
ce::Curve_ID					g_curve_id[CE_CURVE_MAX];
ce::Config						g_config;
ce::Window						g_window;
ce::Gr_Disp_Info				g_disp_info;
FILTER*							g_fp;
std::vector<ce::Preset_Value>	g_presets_value;
std::vector<ce::Preset_ID>		g_presets_id;

//ダークテーマ
const ce::Theme g_theme_dark = {
	RGB(27,27,27),
	RGB(27,27,27),
	RGB(27,27,27),
	RGB(103,103,241),
	RGB(243,243,243),
	RGB(243,243,243),
	RGB(27,27,27),
	RGB(60,60,60),
	NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL,
};
//ライトテーマ
const ce::Theme g_theme_light = {
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
};

ce::Theme g_theme[2] = {
	g_theme_dark,
	g_theme_light,
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
	CE_FILTER_NAME,
	NULL,NULL,NULL,
	NULL,NULL,
	NULL,NULL,NULL,
	NULL,
	initialize,
	exit,
	NULL,
	wndproc_base,
	NULL,NULL,
	NULL,
	NULL,
	CE_FILTER_INFO,
	NULL,NULL,
	NULL,NULL,NULL,NULL,
	NULL,
};


//---------------------------------------------------------------------
//		Lua関数
//---------------------------------------------------------------------

//処理関数
int get_result(lua_State* L)
{
	// index:	カーブのID
	// ratio:	進捗(0~1)
	// st:		トラックバーでの開始時の値
	// ed:		トラックバーでの終了時の値
	int		index	= lua_tointeger(L, 1);
	double	ratio	= lua_tonumber(L, 2);
	double	st		= lua_tonumber(L, 3);
	double	ed		= lua_tonumber(L, 4);

	if (index < 0 || index > CE_CURVE_MAX) lua_pushnumber(L, st + (ed - st) * ratio);
	else lua_pushnumber(L, g_curve_id[index].get_value(ratio, st, ed));
	return 1;
}

//関数テーブル
static luaL_Reg functions[] = {
	{ "get_result", get_result },
	{ nullptr, nullptr }
};

//登録
extern "C" __declspec(dllexport) int luaopen_curve_editor(lua_State * L)
{
	luaL_register(L, "curve_editor", functions);
	return 1;
}