#pragma once

#include <mkaul/version.hpp>



namespace cved {
	namespace global {
		inline constexpr int SEPARATOR_WIDTH = 8;
		inline constexpr size_t IDCURVE_MAX_N = 524288u;

		inline constexpr auto CURVE_NAME_NORMAL = "normal";
		inline constexpr auto CURVE_NAME_VALUE = "value";
		inline constexpr auto CURVE_NAME_BEZIER = "bezier";
		inline constexpr auto CURVE_NAME_ELASTIC = "elastic";
		inline constexpr auto CURVE_NAME_BOUNCE = "bounce";
		inline constexpr auto CURVE_NAME_LINEAR = "linear";
		inline constexpr auto CURVE_NAME_SCRIPT = "script";

		inline constexpr auto PLUGIN_NAME = "Curve Editor";
		inline constexpr auto PLUGIN_VERSION = mkaul::Version{
			mkaul::VersionNumber{2},
			mkaul::PreviewType{mkaul::PreviewType::Type::Alpha},
			mkaul::VersionNumber{2}
		};
		inline constexpr auto PLUGIN_DEVELOPER = "mimaraka";
		inline constexpr auto PLUGIN_TRANSLATOR = "Deepdive";
		inline constexpr auto PLUGIN_GITHUB_URL = "https://github.com/mimaraka/aviutl-plugin-curve_editor";
	}
}