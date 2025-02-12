#pragma once

#include "dialog.hpp"



namespace curve_editor {
	class GeneralPrefDialog : public Dialog {
		HWND hwnd_combo_language_ = NULL;
		HWND hwnd_check_notify_update_ = NULL;

		int resource_id() const noexcept override;
		INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;
		void init_controls(HWND hwnd) noexcept override;
	};
} // namespace curve_editor