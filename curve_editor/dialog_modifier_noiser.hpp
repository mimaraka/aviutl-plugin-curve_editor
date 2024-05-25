#pragma once

#include "dialog.hpp"
#include "noiser.hpp"


namespace cved {
	class NoiserModifierDialog : public Dialog {
		HWND hwnd_edit_seed_ = NULL;
		HWND hwnd_slider_amplitude_ = NULL;
		HWND hwnd_slider_frequency_ = NULL;
		HWND hwnd_slider_phase_ = NULL;
		HWND hwnd_slider_octaves_ = NULL;
		HWND hwnd_slider_decay_sharpness_ = NULL;
		HWND hwnd_static_amplitude_ = NULL;
		HWND hwnd_static_frequency_ = NULL;
		HWND hwnd_static_phase_ = NULL;
		HWND hwnd_static_octaves_ = NULL;
		HWND hwnd_static_decay_sharpness_ = NULL;

		int i_resource() const noexcept override;
		INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;
		void init_controls(HWND hwnd, const Noiser* p_noiser) noexcept;
	};
}