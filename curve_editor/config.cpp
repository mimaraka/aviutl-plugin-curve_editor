#include "config.hpp"
#include "constants.hpp"
#include "curve_editor.hpp"
#include "global.hpp"
#include "json_loader.hpp"
#include "string_table.hpp"
#include <fstream>



using json = nlohmann::json;

namespace cved {
	namespace global {
		void Config::init(HINSTANCE hinst) noexcept {
			pref_.reset();
			edit_mode_ = EditMode::Normal;
			layout_mode_ = LayoutMode::Vertical;
			apply_mode_.fill(ApplyMode::Normal);
			set_apply_mode(EditMode::Value, ApplyMode::IgnoreMidPoint);
			curve_code_bezier_ = 145674282;
			curve_code_elastic_ = 2554290;
			curve_code_bounce_ = 10612242;
			show_x_label_ = false;
			show_y_label_ = true;
			show_handle_ = true;
			show_library_ = true;
			show_velocity_graph_ = false;
			align_handle_ = true;
			ignore_autosaver_warning_ = false;
			separator_pos_ = 1.;
			preset_size_ = 50;

			// AviUtlのディレクトリの取得
			char path_str[MAX_PATH];
			::GetModuleFileNameA(NULL, path_str, MAX_PATH);
			std::filesystem::path path_aviutl{ path_str };
			dir_aviutl_ = path_aviutl.parent_path();
			::GetModuleFileNameA(hinst, path_str, MAX_PATH);
			std::filesystem::path path_plugin{ path_str };
			dir_plugin_ = path_plugin.parent_path() / "curve_editor";
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

		bool Config::set_theme(ThemeId theme) noexcept {
			switch (theme) {
			case ThemeId::System:
			case ThemeId::Dark:
			case ThemeId::Light:
				pref_.theme = theme;
				return true;

			default:
				return false;
			}
		}

		const char* Config::get_edit_mode_dispname(EditMode edit_mode) const noexcept {
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

		const char* Config::get_edit_mode_dispname() const noexcept {
			return get_edit_mode_dispname(edit_mode_);
		}

		bool Config::set_apply_mode(EditMode edit_mode, ApplyMode apply_mode) noexcept {
			if (mkaul::in_range((size_t)apply_mode, 0u, (size_t)ApplyMode::NumApplyMode - 1u)) {
				apply_mode_[(size_t)edit_mode] = apply_mode;
				return true;
			}
			else return false;
		}

		void Config::set_curve_thickness(float curve_thickness) noexcept {
			pref_.curve_thickness = mkaul::clamp(roundf(curve_thickness * 10.f) * 0.1f, 0.1f, 10.f);
		}

		void Config::set_curve_resolution(uint32_t curve_resolution) noexcept {
			pref_.curve_resolution = mkaul::clamp(curve_resolution, 100u, 1000u);
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
			if (mkaul::in_range((uint32_t)edit_mode, 0u, (uint32_t)EditMode::NumEditMode - 1u)) {
				edit_mode_ = edit_mode;
				return true;
			}
			else return false;
		}

		void Config::set_separator_pos(double separator_pos) noexcept {
			separator_pos_ = mkaul::clamp(separator_pos, 0., 1.);
		}

		void Config::set_preset_size(int) noexcept {

		}

#define GET_KEY(var) std::string{ #var, 0u, strlen(#var) - 1u}.c_str()

		// jsonファイルを読み込み
		bool Config::load_json() {
			std::ifstream ifs{ dir_plugin_ / CONFIG_FILE_NAME };
			if (!ifs) return false;
			json data;
			try {
				data = json::parse(ifs);
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
				if (data.contains(GET_KEY(show_x_label_))) set_show_x_label(data[GET_KEY(show_x_label_)]);
				if (data.contains(GET_KEY(show_y_label_))) set_show_y_label(data[GET_KEY(show_y_label_)]);
				if (data.contains(GET_KEY(align_handle_))) set_align_handle(data[GET_KEY(align_handle_)]);
				if (data.contains(GET_KEY(ignore_autosaver_warning_))) set_ignore_autosaver_warning(data[GET_KEY(ignore_autosaver_warning_)]);
				if (data.contains(GET_KEY(separator_pos_))) set_separator_pos(data[GET_KEY(separator_pos_)]);
				if (data.contains(GET_KEY(preset_size_))) set_preset_size(data[GET_KEY(preset_size_)]);
			}
			catch (const json::exception&) {
				return false;
			}
			return true;
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
				{GET_KEY(show_x_label_), show_x_label_},
				{GET_KEY(show_y_label_), show_y_label_},
				{GET_KEY(show_velocity_graph_), show_velocity_graph_},
				{GET_KEY(align_handle_), align_handle_},
				{GET_KEY(ignore_autosaver_warning_), ignore_autosaver_warning_},
				{GET_KEY(separator_pos_), separator_pos_},
				{GET_KEY(preset_size_), preset_size_}
			};

			std::ofstream ofs{ dir_plugin_ / CONFIG_FILE_NAME };
			if (!ofs) return false;
			ofs << data.dump(4);
			return true;
		}

#undef GET_KEY
	}
}