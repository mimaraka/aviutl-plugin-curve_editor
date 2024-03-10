#pragma once

#include "dialog.hpp"



namespace cved {
	class GeneralPrefDialog : public Dialog {
		HWND hwnd_combo_language_ = NULL;
		HWND hwnd_check_notify_update_ = NULL;

		int i_resource() const noexcept override;
		INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;
		void init_controls(HWND hwnd) noexcept override;
	};
}