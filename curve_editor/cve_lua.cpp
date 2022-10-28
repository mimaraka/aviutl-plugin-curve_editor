//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (Lua用)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		処理関数
//---------------------------------------------------------------------
int get_result(lua_State* L)
{
	// index:	カーブのID
	// ratio:	進捗(0~1)
	// st:		トラックバーでの開始時の値
	// ed:		トラックバーでの終了時の値
	int		index = lua_tointeger(L, 1);
	double	ratio = lua_tonumber(L, 2);
	double	st = lua_tonumber(L, 3);
	double	ed = lua_tonumber(L, 4);

	if (index < 0 || index > CVE_CURVE_MAX) lua_pushnumber(L, st + (ed - st) * ratio);
	else lua_pushnumber(L, g_curve_id[index].id_create_result(ratio, st, ed));
	return 1;
}



//---------------------------------------------------------------------
//		関数テーブル
//---------------------------------------------------------------------
static luaL_Reg functions[] = {
	{ "get_result", get_result },
	{ nullptr, nullptr }
};



//---------------------------------------------------------------------
//		登録
//---------------------------------------------------------------------
extern "C" __declspec(dllexport) int luaopen_curve_editor(lua_State * L)
{
	luaL_register(L, "curve_editor", functions);
	return 1;
}