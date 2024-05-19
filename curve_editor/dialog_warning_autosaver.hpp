#pragma once

#include "dialog.hpp"



namespace cved {
	class AutosaverWarningDialog : public Dialog {
		HWND hwnd_check_dontshowagain_ = NULL;

		int i_resource() const noexcept override;
		INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;
		void init_controls(HWND hwnd) noexcept override;
	};
}