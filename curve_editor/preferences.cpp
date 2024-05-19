#include "preferences.hpp"
#include <strconv2/strconv2.h>
#include "global.hpp"
#include "json_loader.hpp"



namespace cved {
	void Preferences::reset() noexcept {
		language = Language::Automatic;
		theme_id = ThemeId::Dark;
		curve_color = mkaul::ColorF{ 148, 158, 197 };
		curve_thickness = 1.2f;
		curve_drawing_interval = 1.f;
		graphic_method = mkaul::graphics::Factory::GraphicEngine::Directx;
		bg_image_path = "";
		bg_image_opacity = 0.3f;
		show_popup = true;
		show_trace = true;
		auto_copy = false;
		auto_apply = true;
		reverse_wheel = false;
		notify_update = false;
		set_bg_image = false;
		enable_hotkeys = true;
	}

	void Preferences::from_json(const nlohmann::json& data) noexcept {
		JsonLoader::get_value(data, "language", language);
		JsonLoader::get_value(data, "theme", theme_id);
		COLORREF tmp;
		if (JsonLoader::get_value(data, "curve_color", tmp)) curve_color = tmp;
		JsonLoader::get_value(data, "curve_thickness", curve_thickness);
		JsonLoader::get_value(data, "curve_drawing_interval", curve_drawing_interval);
		JsonLoader::get_value(data, "graphic_method", graphic_method);
		JsonLoader::get_value(data, "show_popup", show_popup);
		JsonLoader::get_value(data, "show_trace", show_trace);
		JsonLoader::get_value(data, "auto_copy", auto_copy);
		JsonLoader::get_value(data, "auto_apply", auto_apply);
		JsonLoader::get_value(data, "reverse_wheel", reverse_wheel);
		JsonLoader::get_value(data, "notify_update", notify_update);
		JsonLoader::get_value(data, "set_bg_image", set_bg_image);
		std::string str_tmp;
		JsonLoader::get_value(data, "bg_image_path", str_tmp);
		bg_image_path = utf8_to_sjis(str_tmp);
		JsonLoader::get_value(data, "bg_image_opacity", bg_image_opacity);
		JsonLoader::get_value(data, "enable_hotkeys", enable_hotkeys);
	}

	void Preferences::to_json(nlohmann::json* p_json) const noexcept {
		using json = nlohmann::json;
		*p_json = {
			{"language", language},
			{"theme", theme_id},
			{"curve_color", curve_color.colorref()},
			{"curve_thickness", curve_thickness},
			{"curve_drawing_interval", curve_drawing_interval},
			{"graphic_method", graphic_method},
			{"show_popup", show_popup},
			{"show_trace", show_trace},
			{"auto_copy", auto_copy},
			{"auto_apply", auto_apply},
			{"reverse_wheel", reverse_wheel},
			{"notify_update", notify_update},
			{"set_bg_image", set_bg_image},
			{"bg_image_path", sjis_to_utf8(bg_image_path.string())},
			{"bg_image_opacity", bg_image_opacity}
			{"enable_hotkeys", enable_hotkeys}
		};
	}


}