#pragma once

#include "enum.hpp"
#include "preferences.hpp"
#include <mkaul/color.hpp>
#include <mkaul/util.hpp>



namespace cved {
	namespace global {
		inline class Config {
		private:
			static constexpr char CONFIG_FILE_NAME[] = "config.json";

			Preferences pref_;

			EditMode edit_mode_ = EditMode::Normal;
			LayoutMode layout_mode_ = LayoutMode::Vertical;
			std::array<ApplyMode, (size_t)EditMode::NumEditMode> apply_mode_ = {};

			int curve_code_bezier_ = 0;
			int curve_code_elastic_ = 0;
			int curve_code_bounce_ = 0;

			bool show_x_label_ = false;
			bool show_y_label_ = false;
			bool show_handle_ = false;
			bool show_library_ = false;
			bool show_velocity_graph_ = false;
			bool align_handle_ = false;
			bool ignore_autosaver_warning_ = false;
			int separator_ = 0;
			int preset_size_ = 0;

			std::filesystem::path dir_aviutl_;
			std::filesystem::path dir_plugin_;

		public:
			void init(HINSTANCE hinst) noexcept;

			auto get_language() const noexcept { return pref_.language; }
			bool set_language(Language language) noexcept;

			auto get_theme_id() const noexcept { return pref_.theme_id; }
			bool set_theme_id(ThemeId theme_id) noexcept;

			auto get_edit_mode() const noexcept { return edit_mode_; }
			bool set_edit_mode(EditMode edit_mode) noexcept;

			const char* get_edit_mode_str(EditMode edit_mode) const noexcept;
			const char* get_edit_mode_str() const noexcept;

			auto get_layout_mode() const noexcept { return layout_mode_; }
			bool set_layout_mode(LayoutMode layout_mode) noexcept;

			auto get_apply_mode(EditMode edit_mode) const noexcept { return apply_mode_[(size_t)edit_mode]; }
			auto get_apply_mode() const noexcept { return get_apply_mode(get_edit_mode()); }
			bool set_apply_mode(EditMode edit_mode, ApplyMode apply_mode) noexcept;
			bool set_apply_mode(ApplyMode apply_mode) noexcept { return set_apply_mode(get_edit_mode(), apply_mode); }

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

			auto get_set_bg_image() const noexcept { return pref_.set_bg_image; }
			void set_set_bg_image(bool set_bg_image) noexcept { pref_.set_bg_image = set_bg_image; }

			auto get_bg_image_path() const noexcept { return pref_.bg_image_path; }
			void set_bg_image_path(const std::filesystem::path& bg_image_path) noexcept { pref_.bg_image_path = bg_image_path; }

			auto get_bg_image_opacity() const noexcept { return pref_.bg_image_opacity; }
			void set_bg_image_opacity(float bg_image_opacity) noexcept { pref_.bg_image_opacity = mkaul::clamp(bg_image_opacity, 0.f, 1.f); }

			auto get_enable_hotkeys() const noexcept { return pref_.enable_hotkeys; }
			void set_enable_hotkeys(bool enable_hotkeys) noexcept { pref_.enable_hotkeys = enable_hotkeys; }

			auto get_enable_animation() const noexcept { return pref_.enable_animation; }
			void set_enable_animation(bool enable_animation) noexcept { pref_.enable_animation = enable_animation; }

			auto get_ignore_autosaver_warning() const noexcept { return ignore_autosaver_warning_; }
			void set_ignore_autosaver_warning(bool ignore_autosaver_warning) noexcept { ignore_autosaver_warning_ = ignore_autosaver_warning; }

			auto get_separator() const noexcept { return separator_; }
			void set_separator(int separator) noexcept;

			auto get_preset_size() const noexcept { return preset_size_; }
			void set_preset_size(int preset_size) noexcept;

			const auto& get_dir_aviutl() const noexcept { return dir_aviutl_; }
			const auto& get_dir_plugin() const noexcept { return dir_plugin_; }

			void reset_pref() noexcept { pref_.reset(); }

			bool load_json();
			bool save_json();
		} config;
	}
}