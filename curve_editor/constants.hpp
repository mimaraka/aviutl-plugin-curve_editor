#pragma once

#include <mkaul/version.hpp>



namespace curve_editor::global {
	inline constexpr size_t CURVE_ID_MAX = 524288u;

	inline constexpr auto CURVE_NAME_NORMAL = "normal";
	inline constexpr auto CURVE_NAME_VALUE = "value";
	inline constexpr auto CURVE_NAME_BEZIER = "bezier";
	inline constexpr auto CURVE_NAME_ELASTIC = "elastic";
	inline constexpr auto CURVE_NAME_BOUNCE = "bounce";
	inline constexpr auto CURVE_NAME_LINEAR = "linear";
	inline constexpr auto CURVE_NAME_SCRIPT = "script";

	inline constexpr auto MODIFIER_NAME_DISCRETIZATION = "discretization";
	inline constexpr auto MODIFIER_NAME_NOISE = "noise";
	inline constexpr auto MODIFIER_NAME_SINE_WAVE = "sine_wave";
	inline constexpr auto MODIFIER_NAME_SQUARE_WAVE = "square_wave";

	inline constexpr auto PLUGIN_NAME = "curve_editor";
	inline constexpr auto PLUGIN_DISPLAY_NAME = "Curve Editor";
	inline constexpr auto PLUGIN_EXT = "auf";
	inline constexpr auto PLUGIN_VERSION = mkaul::Version{
		mkaul::VersionNumber{2},
		mkaul::PreviewType{mkaul::PreviewType::Type::Beta},
		mkaul::VersionNumber{1, 1, 0}
	};
	inline constexpr auto PLUGIN_DEVELOPER = "mimaraka";
	inline constexpr auto PLUGIN_TRANSLATOR = "Deepdive";
	inline constexpr auto PLUGIN_GITHUB_URL = "https://github.com/mimaraka/aviutl-plugin-curve_editor";
} // namespace curve_editor