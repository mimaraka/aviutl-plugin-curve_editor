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
		inline constexpr float ROUND_RADIUS = 4.f;
		inline constexpr float SEPARATOR_LINE_WIDTH = 3.6f;
		inline constexpr float SEPARATOR_LINE_LENGTH = 32.f;
		inline constexpr float HANDLE_THICKNESS_PRESET = 0.8f;
		inline constexpr float HANDLE_SIZE_PRESET = 0.7f;
		inline constexpr float HANDLE_BORDER_THICKNESS_PRESET = 0.5f;
		inline constexpr float POINT_SIZE_PREEST = 2.f;

		inline constexpr char PLUGIN_NAME[] = "Curve Editor";
		inline constexpr auto PLUGIN_VERSION = mkaul::Version{
			mkaul::VersionNumber{2},
			mkaul::PreviewType{mkaul::PreviewType::Type::Alpha},
			mkaul::VersionNumber{1}
		};
		inline constexpr char PLUGIN_DEVELOPER[] = "mimaraka";
		inline constexpr char PLUGIN_TRANSLATOR[] = "Deepdive";
		inline constexpr char PLUGIN_GITHUB_URL[] = "https://github.com/mimaraka/aviutl-plugin-curve_editor";

		// ダークテーマ
		inline constexpr Theme THEME_DARK = {
			mkaul::ColorF{27, 27, 27},
			mkaul::ColorF{27, 27, 27},
			mkaul::ColorF{19, 19, 19},
			mkaul::ColorF{240, 240, 240},
			mkaul::ColorF{57, 57, 57},
			mkaul::ColorF{95, 95, 95},
			mkaul::ColorF{240, 240, 240},
			mkaul::ColorF{243, 243, 243},
			mkaul::ColorF{180, 180, 180},
			mkaul::ColorF{230, 230, 230},
			mkaul::ColorF{68, 68, 68},
			mkaul::ColorF{240, 240, 240},
			mkaul::ColorF{35, 35, 35},
			mkaul::ColorF{180, 180, 180},
			mkaul::ColorF{200, 200, 200}
		};

		// ライトテーマ
		inline constexpr Theme THEME_LIGHT = {
			mkaul::ColorF{244, 244, 244},
			mkaul::ColorF{230, 230, 230},
			mkaul::ColorF{252, 252, 252},
			mkaul::ColorF{70, 70, 70},
			mkaul::ColorF{205, 205, 205},
			mkaul::ColorF{160, 160, 160},
			mkaul::ColorF{100, 100, 100},
			mkaul::ColorF{70, 70, 70},
			mkaul::ColorF{160, 160, 160},
			mkaul::ColorF{230, 230, 230},
			mkaul::ColorF{188, 188, 188},
			mkaul::ColorF{70, 70, 70},
			mkaul::ColorF{70, 70, 70},
			mkaul::ColorF{70, 70, 70},
			mkaul::ColorF{100, 100, 100}
		};
	}
}