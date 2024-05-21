#include "config.hpp"
#include <fstream>
#include "constants.hpp"
#include "curve_editor.hpp"
#include "global.hpp"
#include "json_loader.hpp"
#include "string_table.hpp"



using json = nlohmann::json;

namespace cved {
	namespace global {
		void Config::init(HINSTANCE hinst) noexcept {
			pref_.reset();
			edit_mode_ = EditMode::Normal;
			layout_mode_ = LayoutMode::Vertical;
			apply_mode_.fill(ApplyMode::Normal);
			current_theme_ = THEME_DARK;
			curve_code_bezier_ = 145674282;
			curve_code_elastic_ = 2554290;
			curve_code_bounce_ = 10612242;
			show_handle_ = true;
			show_library_ = true;
			show_velocity_graph_ = false;
			align_handle_ = true;
			ignore_autosaver_warning_ = false;
			separator_ = global::SEPARATOR_WIDTH;
			preset_size_ = 50;

			// AviUtlのディレクトリの取得
			char path_str[MAX_PATH];
			::GetModuleFileNameA(NULL, path_str, MAX_PATH);
			std::filesystem::path aviutl_path{ path_str };
			aviutl_directory_ = aviutl_path.parent_path();
		}

		bool Config::set_language(Language language) noexcept {
			switch (language) {
			case Language::Automatic:
			case Language::Japanese:
			case Language::English:
			case Language::Korean:
				pref_.language = language;
				return true;

			default:
				return false;
			}
		}

		bool Config::set_theme_id(ThemeId theme_id) noexcept {
			switch (theme_id) {
			case ThemeId::Dark:
				pref_.theme_id = ThemeId::Dark;
				current_theme_ = THEME_DARK;
				return true;

			case ThemeId::Light:
				pref_.theme_id = ThemeId::Light;
				current_theme_ = THEME_LIGHT;
				return true;

			default:
				return false;
			}
		}

		bool Config::set_graphic_method(mkaul::graphics::Factory::GraphicEngine graphic_engine) noexcept {
			using GraphicEngine = mkaul::graphics::Factory::GraphicEngine;
			switch (graphic_engine) {
			case GraphicEngine::Gdiplus:
			case GraphicEngine::Directx:
				pref_.graphic_method = graphic_engine;
				return true;

			default:
				return false;
			}
		}

		const char* Config::get_edit_mode_str(EditMode edit_mode) const noexcept {
			using StringId = StringTable::StringId;

			if (!string_table.loaded()) return nullptr;

			switch (edit_mode) {
			case EditMode::Normal:
				return string_table[StringId::LabelEditModeNormal];

			case EditMode::Value:
				return string_table[StringId::LabelEditModeValue];

			case EditMode::Bezier:
				return string_table[StringId::LabelEditModeBezier];

			case EditMode::Elastic:
				return string_table[StringId::LabelEditModeElastic];

			case EditMode::Bounce:
				return string_table[StringId::LabelEditModeBounce];

			case EditMode::Script:
				return string_table[StringId::LabelEditModeScript];

			default:
				return nullptr;
			}
		}

		const char* Config::get_edit_mode_str() const noexcept {
			return get_edit_mode_str(edit_mode_);
		}

		bool Config::set_apply_mode(EditMode edit_mode, ApplyMode apply_mode) noexcept {
			if (mkaul::in_range((size_t)apply_mode, 0u, (size_t)ApplyMode::NumApplyMode - 1u, true)) {
				apply_mode_[(size_t)edit_mode] = apply_mode;
				return true;
			}
			else return false;
		}

		void Config::set_curve_thickness(float curve_thickness) noexcept {
			pref_.curve_thickness = mkaul::clamp(roundf(curve_thickness * 10.f) * 0.1f, 0.1f, 10.f);
		}

		void Config::set_curve_drawing_interval(float curve_drawing_interval) noexcept {
			pref_.curve_drawing_interval = mkaul::clamp(roundf(curve_drawing_interval * 10.f) * 0.1f, 0.1f, 10.f);
		}

		bool Config::set_layout_mode(LayoutMode layout_mode) noexcept {
			switch (layout_mode) {
			case LayoutMode::Vertical:
			case LayoutMode::Horizontal:
				layout_mode_ = layout_mode;
				return true;

			default:
				return false;
			}
		}

		bool Config::set_edit_mode(EditMode edit_mode) noexcept {
			if (mkaul::in_range((uint32_t)edit_mode, 0u, (uint32_t)EditMode::NumEditMode - 1u, true)) {
				edit_mode_ = edit_mode;
				return true;
			}
			else return false;
		}

		void Config::set_separator(int separator) noexcept {
			separator_ = std::max(separator, global::SEPARATOR_WIDTH);
		}

		void Config::set_preset_size(int preset_size) noexcept {

		}

#define GET_KEY(var) std::string{ #var, 0u, strlen(#var) - 1u}.c_str()

		// jsonファイルを読み込み
		bool Config::load_json() {
			json data;
			if (JsonLoader::get_data(global::fp->dll_hinst, "config", &data)) {
				try {
					if (data.contains("preferences")) pref_.from_json(data["preferences"]);
					if (data.contains(GET_KEY(edit_mode_))) set_edit_mode(data[GET_KEY(edit_mode_)]);
					if (data.contains(GET_KEY(layout_mode_))) set_layout_mode(data[GET_KEY(layout_mode_)]);
					if (data[GET_KEY(apply_mode_)].is_array()) {
						size_t count = 0u;
						for (const auto& el : data[GET_KEY(apply_mode_)]) {
							if (count >= apply_mode_.size()) break;
							set_apply_mode((EditMode)count++, (ApplyMode)el);
						}
					}
					JsonLoader::get_value(data, GET_KEY(curve_code_bezier_), curve_code_bezier_);
					JsonLoader::get_value(data, GET_KEY(curve_code_elastic_), curve_code_elastic_);
					JsonLoader::get_value(data, GET_KEY(curve_code_bounce_), curve_code_bounce_);
					if (data.contains(GET_KEY(show_library_))) set_show_library(data[GET_KEY(show_library_)]);
					if (data.contains(GET_KEY(show_velocity_graph_))) set_show_velocity_graph(data[GET_KEY(show_velocity_graph_)]);
					if (data.contains(GET_KEY(align_handle_))) set_align_handle(data[GET_KEY(align_handle_)]);
					if (data.contains(GET_KEY(ignore_autosaver_warning_))) set_ignore_autosaver_warning(data[GET_KEY(ignore_autosaver_warning_)]);
					if (data.contains(GET_KEY(separator_))) set_separator(data[GET_KEY(separator_)]);
					if (data.contains(GET_KEY(preset_size_))) set_preset_size(data[GET_KEY(preset_size_)]);

					return true;
				}
				catch (const json::exception&) {}
			}
			return false;
		}

		// jsonファイルを保存
		bool Config::save_json() {
			json data_pref;
			pref_.to_json(&data_pref);

			json data = {
				{"preferences", data_pref},
				{GET_KEY(edit_mode_), edit_mode_},
				{GET_KEY(layout_mode_), layout_mode_},
				{GET_KEY(apply_mode_), apply_mode_},
				{"curve_code_bezier", editor.editor_graph().curve_bezier()->encode()},
				{"curve_code_elastic", editor.editor_graph().curve_elastic()->encode()},
				{"curve_code_bounce", editor.editor_graph().curve_bounce()->encode()},
				{GET_KEY(show_library_), show_library_},
				{GET_KEY(show_velocity_graph_), show_velocity_graph_},
				{GET_KEY(align_handle_), align_handle_},
				{GET_KEY(ignore_autosaver_warning_), ignore_autosaver_warning_},
				{GET_KEY(separator_), separator_},
				{GET_KEY(preset_size_), preset_size_}
			};

			return JsonLoader::set_data(global::fp->dll_hinst, "config", data);
		}

#undef GET_KEY
	}
}