#pragma once

#include "dialog.hpp"



namespace cved {
    class AppearancePrefDialog : public Dialog {
		HWND hwnd_combo_theme_ = NULL;
		HWND hwnd_button_curve_color_ = NULL;
		HWND hwnd_slider_curve_thickness_ = NULL;
		HWND hwnd_slider_curve_drawing_interval_ = NULL;
		HWND hwnd_static_curve_thickness_ = NULL;
		HWND hwnd_static_curve_drawing_interval_ = NULL;
		HWND hwnd_check_show_trace_ = NULL;

        int i_resource() const noexcept override;
        INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;
        void init_controls(HWND hwnd) noexcept override;
    };
}