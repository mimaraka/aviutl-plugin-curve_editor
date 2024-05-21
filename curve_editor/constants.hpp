#pragma once

#include "theme.hpp"
#include "version.hpp"



namespace cved {
	namespace global {
		inline constexpr int MARGIN = 6;
		inline constexpr int MARGIN_PRESET = 8;
		inline constexpr int MIN_WIDTH = 100;
		inline constexpr int MIN_HEIGHT = 200;
		inline constexpr int SEPARATOR_WIDTH = 8;
		inline constexpr int MENU_HEIGHT = 75;
		inline constexpr size_t IDCURVE_MAX_N = 4096u;
		inline constexpr float ROUND_RADIUS = 4.f;
		inline constexpr float SEPARATOR_LINE_WIDTH = 3.6f;
		inline constexpr float SEPARATOR_LINE_LENGTH = 32.f;
		inline constexpr float HANDLE_THICKNESS_PRESET = 0.8f;
		inline constexpr float HANDLE_SIZE_PRESET = 0.7f;
		inline constexpr float HANDLE_BORDER_THICKNESS_PRESET = 0.5f;
		inline constexpr float POINT_SIZE_PREEST = 2.f;

		inline constexpr char DATA_PREFIX[] = "CEV2";

		inline constexpr char PLUGIN_NAME[] = "Curve Editor";
		inline constexpr auto PLUGIN_VERSION = mkaul::Version{
			mkaul::VersionNumber{2},
			mkaul::PreviewType{mkaul::PreviewType::Type::Alpha},
			mkaul::VersionNumber{1, 2}
		};
		inline constexpr char PLUGIN_DEVELOPER[] = "mimaraka";
		inline constexpr char PLUGIN_TRANSLATOR[] = "Deepdive";
		inline constexpr char PLUGIN_GITHUB_URL[] = "https://github.com/mimaraka/aviutl-plugin-curve_editor";
	}
}