#include <lua.hpp>

#include "config.hpp"
#include "constants.hpp"
#include "curve_bezier.hpp"
#include "curve_editor.hpp"
#include "curve_editor_graph.hpp"
#include "global.hpp"


namespace curve_editor {
	// カーブの値を取得
	int get_curve(::lua_State* L) {
		// mode:		編集モード(数値または文字列)
		// parameter:	カーブのコードまたはID
		// progress:	進捗(0~1)
		// start:		トラックバーでの開始時の値
		// end:			トラックバーでの終了時の値
		// 
		// Numeric Typeのカーブのオブジェクトを用意しておく
		static BezierCurve curve_bezier;
		static ElasticCurve curve_elastic;
		static BounceCurve curve_bounce;

		int mode_int = -1;
		const char* mode_str = nullptr;
		EditMode mode = EditMode::Normal;
		int parameter;
		double progress, start, end;
		double ret = 0.;

		if (lua_istable(L, 1)) {
			::lua_getfield(L, 1, "mode");
			::lua_getfield(L, 1, "param");
			mode_str = ::lua_tostring(L, -2);
			parameter = ::lua_tointeger(L, -1);
			::lua_pop(L, 2);
			progress = ::lua_tonumber(L, 2);
			start = ::lua_tonumber(L, 3);
			end = ::lua_tonumber(L, 4);
		}
		else {
			if (::lua_isnumber(L, 1)) {
				mode_int = ::lua_tointeger(L, 1);
			}
			else if (::lua_isstring(L, 1)) {
				mode_str = ::lua_tolstring(L, 1, NULL);
			}
			else {
				::lua_pushnil(L);
				return 1;
			}

			parameter = ::lua_tointeger(L, 2);
			progress = ::lua_tonumber(L, 3);
			start = ::lua_tonumber(L, 4);
			end = ::lua_tonumber(L, 5);
		}

		if (mode_str) {
			mode = global::editor.get_mode(mode_str);
			if (mode == EditMode::NumEditMode) {
				::lua_pushnil(L);
				return 1;
			}
		}
		else if (mode_int != -1) {
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
		else {
			::lua_pushnil(L);
			return 1;
		}

		switch (mode) {
		case EditMode::Normal:
		{
			int idx = parameter - 1;
			if (idx < 0) {
				::lua_pushnil(L);
				return 1;
			}
			auto p_curve_normal = global::editor.editor_graph().p_curve_normal(static_cast<size_t>(idx));
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
			int idx = parameter - 1;
			if (idx < 0) {
				::lua_pushnil(L);
				return 1;
			}
			auto p_curve_script = global::editor.editor_script().p_curve_script(static_cast<size_t>(idx));
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
		lua_pushstring(L, global::config.get_edit_mode_dispname());
		return 1;
	}


	int get_version_str(::lua_State* L) {
		lua_pushstring(L, global::PLUGIN_VERSION.str().c_str());
		return 1;
	}

	int put_select_button(::lua_State* L) {
		constexpr int BUTTON_START_ID = 400;
		constexpr int BUTTON_WIDTH = 40;

		auto p_param_dialog_filter_idx = global::exedit_internal.get<ExEdit::ObjectFilterIndex>(0xd44e4);
		auto p_script_processing_filter = global::exedit_internal.get<ExEdit::Filter*>(0x1b2b10);
		if (!p_param_dialog_filter_idx or !p_script_processing_filter or *p_param_dialog_filter_idx != (*p_script_processing_filter)->processing) {
			return 1;
		}

		int idx = ::lua_tointeger(L, 1);
		if (!mkaul::in_range(idx, 1, 16)) {
			return 1;
		}
		const HWND hwnd_script_param = *global::exedit_internal.get<HWND>(0xd4524);
		if (!hwnd_script_param) {
			return 1;
		}
		HWND hwnd_button = ::GetDlgItem(hwnd_script_param, BUTTON_START_ID + idx);
		if (!hwnd_button) {
			HWND hwnd_edit = ::GetDlgItem(hwnd_script_param, 170 + idx);
			if (!hwnd_edit) {
				return 1;
			}
			RECT rect_edit;
			::GetWindowRect(hwnd_edit, &rect_edit);

			int edit_width = rect_edit.right - rect_edit.left;
			int edit_height = rect_edit.bottom - rect_edit.top;

			POINT tmp = { rect_edit.left, rect_edit.top };
			::ScreenToClient(hwnd_script_param, &tmp);

			::MoveWindow(hwnd_edit, tmp.x, tmp.y, edit_width - BUTTON_WIDTH, edit_height, TRUE);

			hwnd_button = ::CreateWindowExA(
				NULL,
				"BUTTON",
				"選択",
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				tmp.x + edit_width - BUTTON_WIDTH,
				tmp.y,
				BUTTON_WIDTH,
				edit_height,
				hwnd_script_param,
				(HMENU)(BUTTON_START_ID + idx),
				(HINSTANCE)::GetWindowLongA(hwnd_script_param, GWLP_HINSTANCE),
				nullptr
			);
			::PostMessageA(hwnd_button, WM_SETFONT, (WPARAM)::SendMessageA(hwnd_edit, WM_GETFONT, 0, 0), 0);
		}
		return 1;
	}
} // namespace curve_editor


extern "C" __declspec(dllexport) int luaopen_curve_editor(::lua_State* L) {
	static ::luaL_Reg functions[] = {
		{ "getcurve", curve_editor::get_curve },
		//{ "geteditmode", curve_editor::get_edit_mode },
		{ "getversionstr", curve_editor::get_version_str },
		{ "putselectbutton", curve_editor::put_select_button},
		{ nullptr, nullptr }
	};

	::luaL_register(L, curve_editor::global::PLUGIN_NAME, functions);
	return 1;
}