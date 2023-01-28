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
	// mode:	編集モード
	// index:	カーブの数値またはID
	// ratio:	進捗(0~1)
	// st:		トラックバーでの開始時の値
	// ed:		トラックバーでの終了時の値
	int		mode = lua_tointeger(L, 1);
	int		num = lua_tointeger(L, 2);
	double	ratio = lua_tonumber(L, 3);
	double	st = lua_tonumber(L, 4);
	double	ed = lua_tonumber(L, 5);
	double	result;

	switch ((cve::Edit_Mode)mode) {
	case cve::Mode_Bezier:
		result = g_curve_bezier.create_result(num, ratio, st, ed);
		break;

	case cve::Mode_Bezier_Multi:
		result = g_curve_bezier_multi[std::clamp(num, 1, CVE_CURVE_MAX) - 1].create_result(ratio, st, ed);
		break;

	case cve::Mode_Bezier_Value:
		//result = g_curve_bezier_value[std::clamp(num, 1, CVE_CURVE_MAX) - 1].create_result(ratio, st, ed);
		result = (ed - st) * ratio + st;
		break;

	case cve::Mode_Elastic:
		result = g_curve_elastic.create_result(num, ratio, st, ed);
		break;

	case cve::Mode_Bounce:
		result = g_curve_bounce.create_result(num, ratio, st, ed);
		break;

	default:
		result = 0;
		break;
	}

	lua_pushnumber(L, result);
	return 1;
}



//---------------------------------------------------------------------
//		関数テーブル
//---------------------------------------------------------------------
static luaL_Reg functions[] = {
	{ "getcurve", get_result },
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