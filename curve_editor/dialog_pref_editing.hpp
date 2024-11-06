#pragma once

#include "dialog.hpp"



namespace cved {
	class EditingPrefDialog : public Dialog {
		HWND hwnd_check_reverse_wheel_ = NULL;
		HWND hwnd_check_auto_copy_ = NULL;
		HWND hwnd_check_auto_apply_ = NULL;
		HWND hwnd_check_word_wrap_ = NULL;

		int resource_id() const noexcept override;
		INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;
		void init_controls(HWND hwnd) noexcept override;
	};
} // namespace cved