#pragma once

#include "theme.hpp"
#include "enum.hpp"
#include <mkaul/include/color.hpp>
#include <mkaul/include/graphics.hpp>
#include <nlohmann/json.hpp>



namespace cved {
	namespace global {
		inline class Config {
		private:
			static constexpr Theme THEME_DARK = {
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

			static constexpr Theme THEME_LIGHT = {
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

			Language language_ = Language::Automatic;
			ThemeId theme_id_ = ThemeId::Dark;
			EditMode edit_mode_ = EditMode::Normal;
			LayoutMode layout_mode_ = LayoutMode::Vertical;
			ApplyMode apply_mode_ = ApplyMode::Normal;
			Theme current_theme_;
			mkaul::ColorF curve_color_ = mkaul::ColorF{};
			float curve_thickness_ = 0.f;
			float curve_drawing_interval_ = 0.f;
			mkaul::graphics::Factory::GraphicEngine graphic_engine_ = mkaul::graphics::Factory::GraphicEngine::Directx;
			int curve_code_bezier_ = 0;
			int curve_code_elastic_ = 0;
			int curve_code_bounce_ = 0;
			bool show_popup_= false;
			bool show_trace_ = false;
			bool auto_copy_ = false;
			bool auto_apply_ = false;
			bool show_handle_ = false;
			bool show_library_ = false;
			bool show_velocity_graph_ = false;
			bool align_handle_ = false;
			bool reverse_wheel_ = false;
			bool notify_update_ = false;
			bool ignore_autosaver_warning_ = false;
			int separator_ = 0;
			int preset_size_ = 0;
			std::filesystem::path aviutl_directory_;
			std::filesystem::path json_path_;

		public:
			void init(HINSTANCE hinst) noexcept;

			auto get_language() const noexcept { return language_; }
			bool set_language(Language language) noexcept;

			auto get_theme_id() const noexcept { return theme_id_; }
			bool set_theme_id(ThemeId theme_id) noexcept;

			const auto& get_theme() const noexcept { return current_theme_; }

			auto get_edit_mode() const noexcept { return edit_mode_; }
			bool set_edit_mode(EditMode edit_mode) noexcept;

			const char* get_edit_mode_str(EditMode edit_mode) const noexcept;
			const char* get_edit_mode_str() const noexcept;

			auto get_layout_mode() const noexcept { return layout_mode_; }
			bool set_layout_mode(LayoutMode layout_mode) noexcept;

			auto get_apply_mode() const noexcept { return apply_mode_; }
			bool set_apply_mode(ApplyMode apply_mode) noexcept;

			const auto& get_curve_color() const noexcept { return curve_color_; }
			void set_curve_color(mkaul::ColorF curve_color) noexcept { curve_color_ = curve_color; }

			auto get_curve_thickness() const noexcept { return curve_thickness_; }
			void set_curve_thickness(float curve_thickness) noexcept;

			auto get_curve_drawing_interval() const noexcept { return curve_drawing_interval_; }
			void set_curve_drawing_interval(float curve_drawing_interval) noexcept;

			auto get_graphic_engine() const noexcept { return graphic_engine_; }
			bool set_graphic_engine(mkaul::graphics::Factory::GraphicEngine graphic_engine) noexcept;

			auto get_curve_code_bezier() const noexcept { return curve_code_bezier_; }
			auto get_curve_code_elastic() const noexcept { return curve_code_elastic_; }
			auto get_curve_code_bounce() const noexcept { return curve_code_bounce_; }

			auto get_show_popup() const noexcept { return show_popup_; }
			void set_show_popup(bool show_popup) noexcept { show_popup_ = show_popup; }

			auto get_show_trace() const noexcept { return show_trace_; }
			void set_show_trace(bool show_trace) noexcept { show_trace_ = show_trace; }

			auto get_auto_copy() const noexcept { return auto_copy_; }
			void set_auto_copy(bool auto_copy) noexcept { auto_copy_ = auto_copy; }

			auto get_auto_apply() const noexcept { return auto_apply_; }
			void set_auto_apply(bool auto_apply) noexcept { auto_apply_ = auto_apply; }

			auto get_show_handle() const noexcept { return show_handle_; }
			void set_show_handle(bool show_handle) noexcept { show_handle_ = show_handle; }

			auto get_show_library() const noexcept { return show_library_; }
			void set_show_library(bool show_library) noexcept { show_library_ = show_library; }

			auto get_show_velocity_graph() const noexcept { return show_velocity_graph_; }
			void set_show_velocity_graph(bool show_velocity_graph) noexcept { show_velocity_graph_ = show_velocity_graph; }

			auto get_align_handle() const noexcept { return align_handle_; }
			void set_align_handle(bool align_handle) noexcept { align_handle_ = align_handle; }

			auto get_reverse_wheel() const noexcept { return reverse_wheel_; }
			void set_reverse_wheel(bool reverse_wheel) noexcept { reverse_wheel_ = reverse_wheel; }

			auto get_notify_update() const noexcept { return notify_update_; }
			void set_notify_update(bool notify_update) noexcept { notify_update_ = notify_update; }

			auto get_ignore_autosaver_warning() const noexcept { return ignore_autosaver_warning_; }
			void set_ignore_autosaver_warning(bool ignore_autosaver_warning) noexcept { ignore_autosaver_warning_ = ignore_autosaver_warning; }

			auto get_separator() const noexcept { return separator_; }
			void set_separator(int separator) noexcept;

			auto get_preset_size() const noexcept { return preset_size_; }
			void set_preset_size(int preset_size) noexcept;

			const auto& get_aviutl_directory() const noexcept { return aviutl_directory_; }

			bool load_json();
			bool save_json();
		} config;
	}
}