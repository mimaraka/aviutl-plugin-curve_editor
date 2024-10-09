#pragma once

#include <nlohmann/json.hpp>
#include <mkaul/color.hpp>
#include "enum.hpp"



namespace cved {
	struct Preferences {
		Language language;
		ThemeId theme_id;
		mkaul::ColorF curve_color;
		float curve_thickness;
		uint32_t curve_resolution;
		std::filesystem::path bg_image_path;
		float bg_image_opacity;
		bool show_popup;
		bool show_trace;
		bool auto_copy;
		bool auto_apply;
		bool invert_wheel;
		bool notify_update;
		bool set_bg_image;
		bool enable_hotkeys;
		bool enable_animation;

		void reset() noexcept;
		void from_json(const nlohmann::json& data) noexcept;
		void to_json(nlohmann::json* p_data) const noexcept;
	};
}