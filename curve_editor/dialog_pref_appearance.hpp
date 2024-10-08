#pragma once

#include "dialog.hpp"



namespace cved {
    class AppearancePrefDialog : public Dialog {
		HWND hwnd_combo_theme_ = NULL;
		HWND hwnd_button_curve_color_ = NULL;
		HWND hwnd_slider_curve_thickness_ = NULL;
		HWND hwnd_slider_curve_quality_ = NULL;
		HWND hwnd_static_curve_thickness_ = NULL;
		HWND hwnd_static_curve_quality_ = NULL;
		HWND hwnd_check_set_bg_image_ = NULL;
		HWND hwnd_static_bg_image_1_ = NULL;
		HWND hwnd_static_bg_image_2_ = NULL;
		HWND hwnd_edit_bg_image_path_ = NULL;
		HWND hwnd_button_bg_image_path_ = NULL;
		HWND hwnd_static_bg_image_opacity_ = NULL;
		HWND hwnd_slider_bg_image_opacity_ = NULL;
		HWND hwnd_check_show_trace_ = NULL;

        int resource_id() const noexcept override;
        INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;
        void init_controls(HWND hwnd) noexcept override;
    };
}