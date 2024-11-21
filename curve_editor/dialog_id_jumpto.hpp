#pragma once

#include "dialog.hpp"



namespace curve_editor {
	class IdJumptoDialog : public Dialog {
		HWND hwnd_edit_ = NULL;
		HWND hwnd_list_ = NULL;
		HWND hwnd_button_back_ = NULL;
		HWND hwnd_button_forward_ = NULL;
		HWND hwnd_static_ = NULL;

		int resource_id() const noexcept override;
		INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;
		void init_controls(HWND hwnd) noexcept override;
	};
} // namespace curve_editor