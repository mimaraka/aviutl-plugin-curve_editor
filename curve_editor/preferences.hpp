#pragma once

#include "enum.hpp"
#include <mkaul/color.hpp>
#include <nlohmann/json.hpp>



namespace curve_editor {
	struct Preferences {
		Language language = Language::Automatic;
		ThemeId theme = ThemeId::System;
		mkaul::ColorF curve_color = mkaul::ColorF{};
		float curve_thickness = 0.f;
		uint32_t curve_resolution = 0u;
		std::filesystem::path bg_image_path = L"";
		float bg_image_opacity = 0.f;
		uint32_t apply_button_height = 0u;
		bool show_popup = false;
		bool show_trace = false;
		bool auto_copy = false;
		bool auto_apply = false;
		bool invert_wheel = false;
		bool notify_update = false;
		bool show_bg_image = false;
		bool enable_hotkeys = false;
		bool enable_animation = false;
		bool word_wrap = false;

		void reset() noexcept;
		void from_json(const nlohmann::json& json) noexcept;
		void to_json(nlohmann::json& json) const noexcept;
	};
} // namespace curve_editor