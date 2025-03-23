#pragma once

#include <mkaul/version.hpp>



namespace curve_editor::global {
	inline constexpr size_t CURVE_ID_MAX = 524288u;

	inline constexpr std::string_view CURVE_NAME_NORMAL = "normal";
	inline constexpr std::string_view CURVE_NAME_VALUE = "value";
	inline constexpr std::string_view CURVE_NAME_BEZIER = "bezier";
	inline constexpr std::string_view CURVE_NAME_ELASTIC = "elastic";
	inline constexpr std::string_view CURVE_NAME_BOUNCE = "bounce";
	inline constexpr std::string_view CURVE_NAME_LINEAR = "linear";
	inline constexpr std::string_view CURVE_NAME_SCRIPT = "script";

	inline constexpr std::string_view MODIFIER_NAME_DISCRETIZATION = "discretization";
	inline constexpr std::string_view MODIFIER_NAME_NOISE = "noise";
	inline constexpr std::string_view MODIFIER_NAME_SINE_WAVE = "sine_wave";
	inline constexpr std::string_view MODIFIER_NAME_SQUARE_WAVE = "square_wave";

	inline constexpr std::string_view PLUGIN_NAME = "curve_editor";
	inline constexpr std::wstring_view PLUGIN_DISPLAY_NAME = L"Curve Editor";
	inline constexpr std::string_view PLUGIN_EXT = "auf";
	inline constexpr auto PLUGIN_VERSION = mkaul::Version{
		mkaul::VersionNumber{2},
		mkaul::PreviewType{mkaul::PreviewType::Type::Beta},
		mkaul::VersionNumber{1, 3}
	};
	inline constexpr auto PLUGIN_DEVELOPER = L"mimaraka";
	inline constexpr auto PLUGIN_TRANSLATOR = L"Deepdive";
	inline constexpr auto PLUGIN_GITHUB_URL = L"https://github.com/mimaraka/aviutl-plugin-curve_editor";
} // namespace curve_editor