#pragma once

#include "dialog.hpp"



namespace cved {
	class CurveCodeDialog : public Dialog {
		static constexpr size_t MAX_TEXT = 12u;

		HWND hwnd_edit_ = NULL;

		int i_resource() const noexcept override;
		INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;
		void init_controls(HWND hwnd) noexcept override;
	};
}