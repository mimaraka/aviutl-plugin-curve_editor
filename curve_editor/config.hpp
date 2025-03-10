#pragma once

#include "enum.hpp"
#include "preferences.hpp"
#include "util_json.hpp"
#include <mkaul/color.hpp>
#include <mkaul/size.hpp>
#include <mkaul/util.hpp>



namespace curve_editor::global {
	inline class Config {
	private:
		static constexpr char CONFIG_FILE_NAME[] = "config.json";

		Preferences pref_;

		EditMode edit_mode_ = EditMode::Normal;
		LayoutMode layout_mode_ = LayoutMode::Vertical;
		std::array<ApplyMode, (size_t)EditMode::NumEditMode> apply_mode_ = {};

		int curve_code_bezier_;
		int curve_code_elastic_;
		int curve_code_bounce_;

		bool show_x_label_;
		bool show_y_label_;
		bool show_handle_;
		bool show_library_;
		bool show_velocity_graph_;
		bool align_handle_;
		bool ignore_autosaver_warning_;
		double separator_pos_;
		int preset_size_;
		mkaul::Size<uint32_t> select_window_size_;

		std::filesystem::path dir_aviutl_;
		std::filesystem::path dir_plugin_;

		bool load_json();

	public:
		Config() noexcept;

		auto get_language() const noexcept { return pref_.language; }
		bool set_language(Language language) noexcept;

		auto get_theme() const noexcept { return pref_.theme; }
		bool set_theme(ThemeId theme) noexcept;

		auto get_edit_mode() const noexcept { return edit_mode_; }
		bool set_edit_mode(EditMode edit_mode) noexcept;

		std::wstring_view get_edit_mode_dispname(EditMode edit_mode) const noexcept;
		std::wstring_view get_edit_mode_dispname() const noexcept { return get_edit_mode_dispname(edit_mode_); };

		auto get_layout_mode() const noexcept { return layout_mode_; }
		bool set_layout_mode(LayoutMode layout_mode) noexcept;

		auto get_apply_mode(EditMode edit_mode) const noexcept { return apply_mode_[(size_t)edit_mode]; }
		auto get_apply_mode() const noexcept { return get_apply_mode(get_edit_mode()); }
		bool set_apply_mode(EditMode edit_mode, ApplyMode apply_mode) noexcept;
		bool set_apply_mode(ApplyMode apply_mode) noexcept { return set_apply_mode(get_edit_mode(), apply_mode); }

		std::wstring_view get_apply_mode_dispname(ApplyMode apply_mode) const noexcept;
		std::wstring_view get_apply_mode_dispname() const noexcept { return get_apply_mode_dispname(get_apply_mode()); }

		const auto& get_curve_color() const noexcept { return pref_.curve_color; }
		void set_curve_color(mkaul::ColorF curve_color) noexcept { pref_.curve_color = curve_color; }

		auto get_curve_thickness() const noexcept { return pref_.curve_thickness; }
		void set_curve_thickness(float curve_thickness) noexcept;

		auto get_curve_resolution() const noexcept { return pref_.curve_resolution; }
		void set_curve_resolution(uint32_t curve_resolution) noexcept;

		auto get_curve_code_bezier() const noexcept { return curve_code_bezier_; }
		auto get_curve_code_elastic() const noexcept { return curve_code_elastic_; }
		auto get_curve_code_bounce() const noexcept { return curve_code_bounce_; }

		auto get_show_popup() const noexcept { return pref_.show_popup; }
		void set_show_popup(bool show_popup) noexcept { pref_.show_popup = show_popup; }

		auto get_show_trace() const noexcept { return pref_.show_trace; }
		void set_show_trace(bool show_trace) noexcept { pref_.show_trace = show_trace; }

		auto get_auto_copy() const noexcept { return pref_.auto_copy; }
		void set_auto_copy(bool auto_copy) noexcept { pref_.auto_copy = auto_copy; }

		auto get_auto_apply() const noexcept { return pref_.auto_apply; }
		void set_auto_apply(bool auto_apply) noexcept { pref_.auto_apply = auto_apply; }

		auto get_show_x_label() const noexcept { return show_x_label_; }
		void set_show_x_label(bool show_x_label) noexcept { show_x_label_ = show_x_label; }

		auto get_show_y_label() const noexcept { return show_y_label_; }
		void set_show_y_label(bool show_y_label) noexcept { show_y_label_ = show_y_label; }

		auto get_show_handle() const noexcept { return show_handle_; }
		void set_show_handle(bool show_handle) noexcept { show_handle_ = show_handle; }

		auto get_show_library() const noexcept { return show_library_; }
		void set_show_library(bool show_library) noexcept { show_library_ = show_library; }

		auto get_show_velocity_graph() const noexcept { return show_velocity_graph_; }
		void set_show_velocity_graph(bool show_velocity_graph) noexcept { show_velocity_graph_ = show_velocity_graph; }

		auto get_align_handle() const noexcept { return align_handle_; }
		void set_align_handle(bool align_handle) noexcept { align_handle_ = align_handle; }

		auto get_invert_wheel() const noexcept { return pref_.invert_wheel; }
		void set_invert_wheel(bool invert_wheel) noexcept { pref_.invert_wheel = invert_wheel; }

		auto get_notify_update() const noexcept { return pref_.notify_update; }
		void set_notify_update(bool notify_update) noexcept { pref_.notify_update = notify_update; }

		auto get_show_bg_image() const noexcept { return pref_.show_bg_image; }
		void set_show_bg_image(bool show_bg_image) noexcept { pref_.show_bg_image = show_bg_image; }

		auto get_bg_image_path() const noexcept { return pref_.bg_image_path; }
		void set_bg_image_path(const std::filesystem::path& bg_image_path) noexcept { pref_.bg_image_path = bg_image_path; }

		auto get_bg_image_opacity() const noexcept { return pref_.bg_image_opacity; }
		void set_bg_image_opacity(float bg_image_opacity) noexcept { pref_.bg_image_opacity = mkaul::clamp(bg_image_opacity, 0.f, 1.f); }

		auto get_apply_button_height() const noexcept { return pref_.apply_button_height; }
		void set_apply_button_height(uint32_t apply_button_height) noexcept { pref_.apply_button_height = mkaul::clamp(apply_button_height, 30u, 200u); }

		auto get_enable_hotkeys() const noexcept { return pref_.enable_hotkeys; }
		void set_enable_hotkeys(bool enable_hotkeys) noexcept { pref_.enable_hotkeys = enable_hotkeys; }

		auto get_enable_animation() const noexcept { return pref_.enable_animation; }
		void set_enable_animation(bool enable_animation) noexcept { pref_.enable_animation = enable_animation; }

		auto get_word_wrap() const noexcept { return pref_.word_wrap; }
		void set_word_wrap(bool word_wrap) noexcept { pref_.word_wrap = word_wrap; }

		auto get_ignore_autosaver_warning() const noexcept { return ignore_autosaver_warning_; }
		void set_ignore_autosaver_warning(bool ignore_autosaver_warning) noexcept { ignore_autosaver_warning_ = ignore_autosaver_warning; }

		auto get_separator_pos() const noexcept { return separator_pos_; }
		void set_separator_pos(double separator_pos) noexcept { separator_pos_ = mkaul::clamp(separator_pos, 0., 1.); }

		auto get_preset_size() const noexcept { return preset_size_; }
		void set_preset_size(int preset_size) noexcept { preset_size_ = mkaul::clamp(preset_size, 20, 200); }

		const auto& get_select_window_size() const noexcept { return select_window_size_; }
		void set_select_window_size(const mkaul::Size<uint32_t>& select_window_size) noexcept { select_window_size_ = select_window_size; }

		const auto& get_dir_aviutl() const noexcept { return dir_aviutl_; }
		const auto& get_dir_plugin() const noexcept { return dir_plugin_; }

		void reset_pref() noexcept { pref_.reset(); }

		bool save_json();
	} config;
} // namespace curve_editor::global