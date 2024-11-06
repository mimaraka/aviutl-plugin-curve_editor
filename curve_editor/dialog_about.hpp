#pragma once

#include "dialog.hpp"



namespace cved {
	class AboutDialog : public Dialog {
		HWND hwnd_static_info_ = NULL;
		HFONT font_title_ = NULL;

		int resource_id() const noexcept override;
		INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;
		void init_controls(HWND hwnd) noexcept override;
	};
} // namespace cved