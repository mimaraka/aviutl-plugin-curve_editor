#include "config.hpp"
#include "constants.hpp"
#include "curve_bezier.hpp"
#include "curve_editor.hpp"
#include "curve_editor_graph.hpp"
#include <lua.hpp>



namespace cved {
	// カーブの値を取得
	int get_curve(::lua_State* L) {
		// mode:		編集モード(数値または文字列)
		// parameter:	カーブのコードまたはID
		// progress:	進捗(0~1)
		// start:		トラックバーでの開始時の値
		// end:			トラックバーでの終了時の値
		const int parameter = ::lua_tointeger(L, 2);
		const double progress = ::lua_tonumber(L, 3);
		const double start = ::lua_tonumber(L, 4);
		const double end = ::lua_tonumber(L, 5);
		double ret = 0.;
		EditMode mode;

		if (::lua_isnumber(L, 1)) {
			int mode_int = ::lua_tointeger(L, 1);
			switch (mode_int) {
			case 0:
				mode = EditMode::Bezier;
				break;
			case 1:
				mode = EditMode::Normal;
				break;
			case 2:
				mode = EditMode::Value;
				break;
			case 3:
				mode = EditMode::Elastic;
				break;
			case 4:
				mode = EditMode::Bounce;
				break;
			case 5:
				mode = EditMode::Script;
				break;
			default:
				::lua_pushnil(L);
				return 1;
			}
		}
		else if (::lua_isstring(L, 1)) {
			auto mode_str = ::lua_tolstring(L, 1, NULL);
			if (::strcmp(mode_str, global::CURVE_NAME_NORMAL) == 0) {
				mode = EditMode::Normal;
			}
			else if (::strcmp(mode_str, global::CURVE_NAME_VALUE) == 0) {
				mode = EditMode::Value;
			}
			else if (::strcmp(mode_str, global::CURVE_NAME_BEZIER) == 0) {
				mode = EditMode::Bezier;
			}
			else if (::strcmp(mode_str, global::CURVE_NAME_ELASTIC) == 0) {
				mode = EditMode::Elastic;
			}
			else if (::strcmp(mode_str, global::CURVE_NAME_BOUNCE) == 0) {
				mode = EditMode::Bounce;
			}
			else if (::strcmp(mode_str, global::CURVE_NAME_SCRIPT) == 0) {
				mode = EditMode::Script;
			}
			else {
				::lua_pushnil(L);
				return 1;
			}
		}
		else {
			::lua_pushnil(L);
			return 1;
		}

		// Numeric Typeのカーブのオブジェクトを用意しておく
		static BezierCurve curve_bezier;
		static ElasticCurve curve_elastic;
		static BounceCurve curve_bounce;

		switch (mode) {
		case EditMode::Normal:
		{
			int idx = parameter - 1;
			if (idx < 0) {
				::lua_pushnil(L);
				return 1;
			}
			auto p_curve_normal = global::editor.editor_graph().curve_normal(static_cast<size_t>(idx));
			if (p_curve_normal) {
				ret = p_curve_normal->get_value(progress, start, end);
			}
			else {
				::lua_pushnil(L);
				return 1;
			}
			break;
		}

		case EditMode::Value:
			::lua_pushnil(L);
			return 1;
			break;

		case EditMode::Bezier:
			if (!curve_bezier.decode(parameter)) {
				::lua_pushnil(L);
				return 1;
			}
			ret = curve_bezier.get_value(progress, start, end);
			break;

		case EditMode::Elastic:
			if (!curve_elastic.decode(parameter)) {
				::lua_pushnil(L);
				return 1;
			}
			ret = curve_elastic.get_value(progress, start, end);
			break;

		case EditMode::Bounce:
			if (!curve_bounce.decode(parameter)) {
				::lua_pushnil(L);
				return 1;
			}
			ret = curve_bounce.get_value(progress, start, end);
			break;

		case EditMode::Script:
		{
			int idx = parameter - global::IDCURVE_MAX_N - 1;
			if (idx < 0) {
				::lua_pushnil(L);
				return 1;
			}
			auto p_curve_script = global::editor.editor_script().curve_script(static_cast<size_t>(idx));
			if (p_curve_script) {
				ret = p_curve_script->get_value(progress, start, end);
			}
			else {
				::lua_pushnil(L);
				return 1;
			}
			break;
		}

		default:
			::lua_pushnil(L);
			return 1;
		}

		::lua_pushnumber(L, ret);
		return 1;
	}


	int get_edit_mode(::lua_State* L) {
		::lua_pushstring(L, global::config.get_edit_mode_str());
		return 1;
	}


	int get_version_str(::lua_State* L) {
		::lua_pushstring(L, cved::global::PLUGIN_VERSION.str().c_str());
		return 1;
	}
}


extern "C" __declspec(dllexport) int luaopen_curve_editor(::lua_State* L)
{
	static ::luaL_Reg functions[] = {
		{ "getcurve", cved::get_curve },
		{ "geteditmode", cved::get_edit_mode },
		{ "getversionstr", cved::get_version_str },
		{ nullptr, nullptr }
	};

	::luaL_register(L, "curve_editor", functions);
	return 1;
}