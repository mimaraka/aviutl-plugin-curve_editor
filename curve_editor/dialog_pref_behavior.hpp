#pragma once

#include "dialog.hpp"



namespace cved {
	class BehaviorPrefDialog : public Dialog {
		HWND hwnd_check_show_popup_ = NULL;
		HWND hwnd_check_enable_hotkeys_ = NULL;

		int resource_id() const noexcept override;
		INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;
		void init_controls(HWND hwnd) noexcept override;
	};
}