#pragma once

#include "dialog.hpp"
#include "modifier_discretization.hpp"



namespace cved {
	class DiscretizationModifierDialog : public Dialog {
		HWND hwnd_slider_sampling_ = NULL;
		HWND hwnd_slider_quantization_ = NULL;
		HWND hwnd_static_sampling_ = NULL;
		HWND hwnd_static_quantization_ = NULL;

		int resource_id() const noexcept override;
		INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;
		void init_controls(HWND hwnd, const DiscretizationModifier* p_mod_discretization) noexcept;
	};
}