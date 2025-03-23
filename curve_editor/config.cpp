#include "config.hpp"
#include "curve_editor.hpp"
#include "global.hpp"
#include "preset_manager.hpp"
#include "string_table.hpp"
#include "util.hpp"
#include <fstream>



namespace curve_editor::global {
	Config::Config() noexcept :
		edit_mode_{ EditMode::Normal },
		layout_mode_{ LayoutMode::Horizontal },
		apply_mode_{},
		curve_code_bezier_{ 145674282 },
		curve_code_elastic_{ 2554290 },
		curve_code_bounce_{ 10612242 },
		show_x_label_{ false },
		show_y_label_{ true },
		show_handle_{ true },
		show_library_{ true },
		show_velocity_graph_{ false },
		align_handle_{ true },
		ignore_autosaver_warning_{ false },
		separator_pos_{ 0.56 },
		preset_size_{ 64 },
		preset_simple_view_{ false },
		select_window_size_{ 300, 450 }
	{
		pref_.reset();
		apply_mode_.fill(ApplyMode::Normal);
		set_apply_mode(EditMode::Value, ApplyMode::IgnoreMidPoint);
		set_apply_mode(EditMode::Script, ApplyMode::IgnoreMidPoint);

		// AviUtlのディレクトリの取得
		char path_str[MAX_PATH];
		::GetModuleFileNameA(NULL, path_str, MAX_PATH);
		std::filesystem::path path_aviutl{ path_str };
		dir_aviutl_ = path_aviutl.parent_path();
		::GetModuleFileNameA(util::get_hinst(), path_str, MAX_PATH);
		std::filesystem::path path_plugin{ path_str };
		dir_plugin_ = path_plugin.parent_path() / global::PLUGIN_NAME;

		load_json();
	}

	bool Config::set_language(Language language) noexcept {
		if (mkaul::in_range(static_cast<uint32_t>(language), 0u, static_cast<uint32_t>(Language::NumLanguage) - 1u)) {
			pref_.language = language;
			return true;
		}
		else return false;
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

	std::wstring_view Config::get_edit_mode_dispname(EditMode edit_mode) const noexcept {
		using StringId = StringTable::StringId;
		switch (edit_mode) {
		case EditMode::Normal:
			return string_table[StringId::CurveTypeNormal];

		case EditMode::Value:
			return string_table[StringId::CurveTypeValue];

		case EditMode::Bezier:
			return string_table[StringId::CurveTypeBezier];

		case EditMode::Elastic:
			return string_table[StringId::CurveTypeElastic];

		case EditMode::Bounce:
			return string_table[StringId::CurveTypeBounce];

		case EditMode::Script:
			return string_table[StringId::CurveTypeScript];

		default:
			return L"";
		}
	}

	std::wstring_view Config::get_apply_mode_dispname(ApplyMode apply_mode) const noexcept {
		using StringId = StringTable::StringId;
		switch (apply_mode) {
		case ApplyMode::Normal:
			return string_table[StringId::ApplyModeNormal];
		case ApplyMode::IgnoreMidPoint:
			return string_table[StringId::ApplyModeIgnoreMidPoint];
		case ApplyMode::Interpolate:
			return string_table[StringId::ApplyModeInterpolate];
		default:
			return L"";
		}
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

#define GET_KEY(var) std::string{ #var, 0u, strlen(#var) - 1u}.c_str()

	// jsonファイルを読み込み
	bool Config::load_json() {
		std::ifstream ifs{ dir_plugin_ / CONFIG_FILE_NAME };
		if (!ifs) return false;
		nlohmann::json data;
		try {
			data = nlohmann::json::parse(ifs);
			if (data.contains("preferences")) {
				pref_.from_json(data["preferences"]);
			}
			if (data.contains("preset_list_config")) {
				global::preset_manager.list_config_from_json(data["preset_list_config"]);
			}
			util::set_from_json(this, data, GET_KEY(edit_mode_), &Config::set_edit_mode);
			util::set_from_json(this, data, GET_KEY(layout_mode_), &Config::set_layout_mode);
			if (data[GET_KEY(apply_mode_)].is_array()) {
				size_t count = 0u;
				for (const auto& el : data[GET_KEY(apply_mode_)]) {
					if (count >= apply_mode_.size()) break;
					set_apply_mode((EditMode)count++, (ApplyMode)el);
				}
			}
			util::set_from_json(data, GET_KEY(curve_code_bezier_), curve_code_bezier_);
			util::set_from_json(data, GET_KEY(curve_code_elastic_), curve_code_elastic_);
			util::set_from_json(data, GET_KEY(curve_code_bounce_), curve_code_bounce_);
			util::set_from_json(this, data, GET_KEY(show_library_), &Config::set_show_library);
			util::set_from_json(this, data, GET_KEY(show_velocity_graph_), &Config::set_show_velocity_graph);
			util::set_from_json(this, data, GET_KEY(show_x_label_), &Config::set_show_x_label);
			util::set_from_json(this, data, GET_KEY(show_y_label_), &Config::set_show_y_label);
			util::set_from_json(this, data, GET_KEY(align_handle_), &Config::set_align_handle);
			util::set_from_json(this, data, GET_KEY(ignore_autosaver_warning_), &Config::set_ignore_autosaver_warning);
			util::set_from_json(this, data, GET_KEY(separator_pos_), &Config::set_separator_pos);
			util::set_from_json(this, data, GET_KEY(preset_size_), &Config::set_preset_size);
			util::set_from_json(this, data, GET_KEY(preset_simple_view_), &Config::set_preset_simple_view);
			util::set_from_json(data, "select_window_width", select_window_size_.width);
			util::set_from_json(data, "select_window_height", select_window_size_.height);
		}
		catch (const nlohmann::json::exception&) {
			return false;
		}
		return true;
	}

	// jsonファイルを保存
	bool Config::save_json() {
		nlohmann::json json_pref;
		pref_.to_json(json_pref);
		nlohmann::json json_preset_list_config;
		global::preset_manager.list_config_to_json(json_preset_list_config);

		nlohmann::json json = {
			{"preferences", json_pref},
			{"preset_list_config", json_preset_list_config},
			{GET_KEY(edit_mode_), edit_mode_},
			{GET_KEY(layout_mode_), layout_mode_},
			{GET_KEY(apply_mode_), apply_mode_},
			{"curve_code_bezier", editor.editor_graph().curve_bezier().encode()},
			{"curve_code_elastic", editor.editor_graph().curve_elastic().encode()},
			{"curve_code_bounce", editor.editor_graph().curve_bounce().encode()},
			{GET_KEY(show_library_), show_library_},
			{GET_KEY(show_x_label_), show_x_label_},
			{GET_KEY(show_y_label_), show_y_label_},
			{GET_KEY(show_velocity_graph_), show_velocity_graph_},
			{GET_KEY(align_handle_), align_handle_},
			{GET_KEY(ignore_autosaver_warning_), ignore_autosaver_warning_},
			{GET_KEY(separator_pos_), separator_pos_},
			{GET_KEY(preset_size_), preset_size_},
			{GET_KEY(preset_simple_view_), preset_simple_view_},
			{"select_window_width", select_window_size_.width},
			{"select_window_height", select_window_size_.height}
		};

		std::ofstream ofs{ dir_plugin_ / CONFIG_FILE_NAME };
		if (!ofs) return false;
		ofs << json.dump(2);
		return true;
	}

#undef GET_KEY
} // namespace curve_editor::global