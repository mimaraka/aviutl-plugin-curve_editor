#include "config.hpp"
#include <fstream>
#include "constants.hpp"
#include "curve_editor.hpp"
#include "string_table.hpp"



using json = nlohmann::json;

namespace cved {
	namespace global {
		void Config::init(HINSTANCE hinst) noexcept {
			language_ = Language::Automatic;
			theme_id_ = ThemeId::Dark;
			edit_mode_ = EditMode::Normal;
			layout_mode_ = LayoutMode::Vertical;
			apply_mode_ = ApplyMode::Normal;
			current_theme_ = THEME_DARK;
			curve_color_ = mkaul::ColorF(148, 158, 197);
			curve_thickness_ = 1.2f;
			curve_drawing_interval_ = 1.f;
			curve_code_bezier_ = 145674282;
			curve_code_elastic_ = 2554290;
			curve_code_bounce_ = 10612242;
			show_popup_ = true;
			show_trace_ = true;
			auto_copy_ = false;
			auto_apply_ = true;
			show_handle_ = true;
			show_library_ = true;
			show_velocity_graph_ = false;
			align_handle_ = true;
			reverse_wheel_ = false;
			notify_update_ = true;
			ignore_autosaver_warning_ = false;
			separator_ = global::SEPARATOR_WIDTH;
			preset_size_ = 50;

			// AviUtlのディレクトリの取得
			char path_str[MAX_PATH];
			::GetModuleFileNameA(NULL, path_str, MAX_PATH);
			std::filesystem::path aviutl_path{ path_str };
			aviutl_directory_ = aviutl_path.parent_path();
			// jsonファイルのパスの設定
			::GetModuleFileNameA(hinst, path_str, MAX_PATH);
			std::filesystem::path dll_path{ path_str };
			json_path_ = dll_path.parent_path() / "curve_editor.json";
		}

		bool Config::set_language(Language language) noexcept {
			switch (language) {
			case Language::Automatic:
			case Language::Japanese:
			case Language::English:
			case Language::Korean:
				language_ = language;
				return true;

			default:
				return false;
			}
		}

		bool Config::set_theme_id(ThemeId theme_id) noexcept {
			switch (theme_id) {
			case ThemeId::Dark:
				theme_id_ = ThemeId::Dark;
				current_theme_ = THEME_DARK;
				return true;

			case ThemeId::Light:
				theme_id_ = ThemeId::Light;
				current_theme_ = THEME_LIGHT;
				return true;

			default:
				return false;
			}
		}

		bool Config::set_graphic_engine(mkaul::graphics::Factory::GraphicEngine graphic_engine) noexcept {
			using GraphicEngine = mkaul::graphics::Factory::GraphicEngine;
			switch (graphic_engine) {
			case GraphicEngine::Gdiplus:
			case GraphicEngine::Directx:
				graphic_engine_ = graphic_engine;
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

			case EditMode::Step:
				return string_table[StringId::LabelEditModeStep];

			case EditMode::Script:
				return string_table[StringId::LabelEditModeScript];

			default:
				return nullptr;
			}
		}

		const char* Config::get_edit_mode_str() const noexcept {
			return get_edit_mode_str(edit_mode_);
		}

		bool Config::set_apply_mode(ApplyMode apply_mode) noexcept {
			switch (apply_mode) {
			case ApplyMode::Normal:
			case ApplyMode::IgnoreMidPoint:
				apply_mode_ = apply_mode;
				return true;

			default:
				return false;
			}
		}

		void Config::set_curve_thickness(float curve_thickness) noexcept {
			curve_thickness_ = mkaul::clamp(roundf(curve_thickness * 10.f) * 0.1f, 0.1f, 10.f);
		}

		void Config::set_curve_drawing_interval(float curve_drawing_interval) noexcept {
			curve_drawing_interval_ = mkaul::clamp(roundf(curve_drawing_interval * 10.f) * 0.1f, 0.1f, 10.f);
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
			std::ifstream file{ json_path_ };
			if (file.fail()) return false;
			
			try {
				json data = json::parse(file);
				if (data.contains(GET_KEY(theme_id_))) set_theme_id(data[GET_KEY(theme_id_)]);
				if (data.contains(GET_KEY(edit_mode_))) set_edit_mode(data[GET_KEY(edit_mode_)]);
				if (data.contains(GET_KEY(layout_mode_))) set_layout_mode(data[GET_KEY(layout_mode_)]);
				if (data.contains(GET_KEY(apply_mode_))) set_apply_mode(data[GET_KEY(apply_mode_)]);
				if (data.contains(GET_KEY(curve_color_))) set_curve_color(mkaul::ColorF{ data[GET_KEY(curve_color_)] });
				if (data.contains(GET_KEY(curve_thickness_))) set_curve_thickness(data[GET_KEY(curve_thickness_)]);
				if (data.contains(GET_KEY(curve_drawing_interval_))) set_curve_drawing_interval(data[GET_KEY(curve_drawing_interval_)]);
				if (data.contains(GET_KEY(graphic_engine_))) set_graphic_engine(data[GET_KEY(graphic_engine_)]);
				if (data.contains(GET_KEY(curve_code_bezier_))) curve_code_bezier_ = data[GET_KEY(curve_code_bezier_)];
				if (data.contains(GET_KEY(curve_code_elastic_))) curve_code_elastic_ = data[GET_KEY(curve_code_elastic_)];
				if (data.contains(GET_KEY(curve_code_bounce_))) curve_code_bounce_ = data[GET_KEY(curve_code_bounce_)];
				if (data.contains(GET_KEY(show_popup_))) set_show_popup(data[GET_KEY(show_popup_)]);
				if (data.contains(GET_KEY(show_trace_))) set_show_trace(data[GET_KEY(show_trace_)]);
				if (data.contains(GET_KEY(show_library_))) set_show_library(data[GET_KEY(show_library_)]);
				if (data.contains(GET_KEY(auto_copy_))) set_auto_copy(data[GET_KEY(auto_copy_)]);
				if (data.contains(GET_KEY(auto_apply_))) set_auto_apply(data[GET_KEY(auto_apply_)]);
				if (data.contains(GET_KEY(show_velocity_graph_))) set_show_velocity_graph(data[GET_KEY(show_velocity_graph_)]);
				if (data.contains(GET_KEY(align_handle_))) set_align_handle(data[GET_KEY(align_handle_)]);
				if (data.contains(GET_KEY(reverse_wheel_))) set_reverse_wheel(data[GET_KEY(reverse_wheel_)]);
				if (data.contains(GET_KEY(notify_update_))) set_notify_update(data[GET_KEY(notify_update_)]);
				if (data.contains(GET_KEY(ignore_autosaver_warning_))) set_ignore_autosaver_warning(data[GET_KEY(ignore_autosaver_warning_)]);
				if (data.contains(GET_KEY(separator_))) set_separator(data[GET_KEY(separator_)]);
				if (data.contains(GET_KEY(preset_size_))) set_preset_size(data[GET_KEY(preset_size_)]);

				return true;
			}
			catch (json::parse_error&) {
				return false;
			}
		}

		// jsonファイルを保存
		bool Config::save_json() {
			json data = {
				{GET_KEY(theme_id_), theme_id_},
				{GET_KEY(edit_mode_), edit_mode_},
				{GET_KEY(layout_mode_), layout_mode_},
				{GET_KEY(apply_mode_), apply_mode_},
				{GET_KEY(curve_color_), curve_color_.colorref()},
				{GET_KEY(curve_thickness_), curve_thickness_},
				{GET_KEY(curve_drawing_interval_), curve_drawing_interval_},
				{GET_KEY(graphic_engine_), graphic_engine_},
				{"curve_code_bezier", editor.editor_graph().curve_bezier()->encode()},
				{"curve_code_elastic", editor.editor_graph().curve_elastic()->encode()},
				{"curve_code_bounce", editor.editor_graph().curve_bounce()->encode()},
				{GET_KEY(show_popup_), show_popup_},
				{GET_KEY(show_trace_), show_trace_},
				{GET_KEY(show_library_), show_library_},
				{GET_KEY(auto_copy_), auto_copy_},
				{GET_KEY(auto_apply_), auto_apply_},
				{GET_KEY(show_velocity_graph_), show_velocity_graph_},
				{GET_KEY(align_handle_), align_handle_},
				{GET_KEY(reverse_wheel_), reverse_wheel_},
				{GET_KEY(notify_update_), notify_update_},
				{GET_KEY(ignore_autosaver_warning_), ignore_autosaver_warning_},
				{GET_KEY(separator_), separator_},
				{GET_KEY(preset_size_), preset_size_}
			};

			std::ofstream file{ json_path_ };
			if (file.fail()) return false;

			file << data.dump(4);
			file.close();
			return true;
		}

#undef GET_KEY
	}
}