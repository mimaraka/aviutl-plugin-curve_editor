#include "constants.hpp"
#include "curve_editor.hpp"
#include "curve_editor_graph.hpp"
#include "curve_bezier.hpp"
#include <lua5_1_4_Win32_dll8_lib/include/lua.hpp>



namespace cved {
	// カーブの値を取得
	int get_curve(::lua_State* L) {
		// mode:		編集モード
		// parameter:	カーブのコードまたはID
		// progress:	進捗(0~1)
		// start:		トラックバーでの開始時の値
		// end:			トラックバーでの終了時の値
		const int mode = ::lua_tointeger(L, 1);
		const int parameter = ::lua_tointeger(L, 2);
		const double progress = ::lua_tonumber(L, 3);
		const double start = ::lua_tonumber(L, 4);
		const double end = ::lua_tonumber(L, 5);
		double ret = 0.;

		// Numeric Typeのカーブのオブジェクトを用意しておく
		static BezierCurve curve_bezier;
		static ElasticCurve curve_elastic;
		static BounceCurve curve_bounce;

		switch (mode) {
			// ベジェ
		case 0:
			if (!curve_bezier.decode(parameter)) {
				::lua_pushnil(L);
				return 1;
			}
			ret = curve_bezier.get_value(progress, start, end);
			break;

			// 標準
		case 1:
		{
			auto p_curve_normal = global::editor.editor_graph().curve_normal(parameter - 1);
			if (p_curve_normal) {
				ret = p_curve_normal->get_value(progress, start, end);
			}
			else {
				::lua_pushnil(L);
				return 1;
			}
			break;
		}

			// 値指定
		case 2:
			::lua_pushnil(L);
			return 1;
			break;

			// 振動
		case 3:
			if (!curve_elastic.decode(parameter)) {
				::lua_pushnil(L);
				return 1;
			}
			ret = curve_elastic.get_value(progress, start, end);
			break;

			// バウンス
		case 4:
			if (!curve_bounce.decode(parameter)) {
				::lua_pushnil(L);
				return 1;
			}
			ret = curve_bounce.get_value(progress, start, end);
			break;

			// スクリプト
		case 5:
			::lua_pushnil(L);
			return 1;
			break;

		default:
			::lua_pushnil(L);
			return 1;
		}

		::lua_pushnumber(L, ret);
		return 1;
	}


	int get_version_str(::lua_State* L) {
		::lua_pushstring(L, cved::global::PLUGIN_VERSION.str().c_str());
		return 1;
	}
}


extern "C" __declspec(dllexport) int luaopen_curve_editor(::lua_State * L)
{
	static ::luaL_Reg functions[] = {
		{ "getcurve", cved::get_curve },
		{ "getversionstr", cved::get_version_str },
		{ nullptr, nullptr }
	};

	::luaL_register(L, "curve_editor", functions);
	return 1;
}