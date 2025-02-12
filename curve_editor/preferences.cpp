#include "global.hpp"
#include "util_json.hpp"
#include "preferences.hpp"
#include <strconv2.h>



namespace curve_editor {
	void Preferences::reset() noexcept {
		language = Language::Automatic;
		theme = ThemeId::Dark;
		curve_color = mkaul::ColorF{ 136, 176, 196 };
		curve_thickness = 1.5f;
		curve_resolution = 400u;
		bg_image_path = "";
		bg_image_opacity = 0.3f;
		show_popup = true;
		show_trace = true;
		auto_copy = false;
		auto_apply = true;
		invert_wheel = false;
		notify_update = true;
		show_bg_image = false;
		enable_hotkeys = true;
		enable_animation = true;
		word_wrap = false;
	}

	void Preferences::from_json(const nlohmann::json& json) noexcept {
#define GET_VALUE(v) util::set_from_json(json, #v, v)
		GET_VALUE(language);
		GET_VALUE(theme);
		COLORREF tmp;
		if (util::set_from_json(json, "curve_color", tmp)) curve_color = tmp;
		GET_VALUE(curve_thickness);
		GET_VALUE(curve_resolution);
		GET_VALUE(show_popup);
		GET_VALUE(show_trace);
		GET_VALUE(auto_copy);
		GET_VALUE(auto_apply);
		GET_VALUE(invert_wheel);
		GET_VALUE(notify_update);
		GET_VALUE(show_bg_image);
		std::string str_tmp;
		if (util::set_from_json(json, "bg_image_path", str_tmp)) bg_image_path = utf8_to_sjis(str_tmp);
		GET_VALUE(bg_image_opacity);
		GET_VALUE(enable_hotkeys);
		GET_VALUE(enable_animation);
		GET_VALUE(word_wrap);
#undef GET_VALUE
	}

	void Preferences::to_json(nlohmann::json& json) const noexcept {
#define MAKE_PAIR(v) {#v, v}
		json = {
			MAKE_PAIR(language),
			MAKE_PAIR(theme),
			{"curve_color", curve_color.colorref()},
			MAKE_PAIR(curve_thickness),
			MAKE_PAIR(curve_resolution),
			MAKE_PAIR(show_popup),
			MAKE_PAIR(show_trace),
			MAKE_PAIR(auto_copy),
			MAKE_PAIR(auto_apply),
			MAKE_PAIR(invert_wheel),
			MAKE_PAIR(notify_update),
			MAKE_PAIR(show_bg_image),
			{"bg_image_path", sjis_to_utf8(bg_image_path.string())},
			MAKE_PAIR(bg_image_opacity),
			MAKE_PAIR(enable_hotkeys),
			MAKE_PAIR(enable_animation),
			MAKE_PAIR(word_wrap)
		};
#undef MAKE_PAIR
	}
} // namespace curve_editor