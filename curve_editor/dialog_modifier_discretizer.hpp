#pragma once

#include "dialog.hpp"
#include "discretizer.hpp"



namespace cved {
	class DiscretizerModifierDialog : public Dialog {
		HWND hwnd_slider_sampling_ = NULL;
		HWND hwnd_slider_quantization_ = NULL;
		HWND hwnd_static_sampling_ = NULL;
		HWND hwnd_static_quantization_ = NULL;

		int i_resource() const noexcept override;
		INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;
		void init_controls(HWND hwnd, const Discretizer* p_discretizer) noexcept;
	};
}