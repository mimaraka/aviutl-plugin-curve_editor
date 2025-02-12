#pragma once

#include "dialog.hpp"



namespace curve_editor {
	class PresetListSettingDialog: public Dialog {
		HWND hwnd_combo_sort_by_ = NULL;
		HWND hwnd_radio_sort_asc_ = NULL;
		HWND hwnd_radio_sort_desc_ = NULL;
		HWND hwnd_check_filter_type_normal_ = NULL;
		HWND hwnd_check_filter_type_value_ = NULL;
		HWND hwnd_check_filter_type_bezier_ = NULL;
		HWND hwnd_check_filter_type_elastic_ = NULL;
		HWND hwnd_check_filter_type_bounce_ = NULL;
		HWND hwnd_check_filter_type_script_ = NULL;

		int resource_id() const noexcept override;
		INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;
		void init_controls(HWND hwnd) noexcept override;
	};
} // namespace curve_editor