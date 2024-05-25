#include "dialog_modifier_discretizer.hpp"
#include <Commctrl.h>
#include "global.hpp"
#include "resource.h"



namespace cved {
	int DiscretizerModifierDialog::i_resource() const noexcept { return IDD_MODIFIER_DISCRETIZATION; }


	void DiscretizerModifierDialog::init_controls(HWND hwnd, const Discretizer* p_discretizer) noexcept {
		hwnd_slider_sampling_ = ::GetDlgItem(hwnd, IDC_SLIDER_SAMPLING);
		hwnd_slider_quantization_ = ::GetDlgItem(hwnd, IDC_SLIDER_QUANTIZATION);
		hwnd_static_sampling_ = ::GetDlgItem(hwnd, IDC_STATIC_SAMPLING);
		hwnd_static_quantization_ = ::GetDlgItem(hwnd, IDC_STATIC_QUANTIZATION);
		
		::SendMessageA(hwnd_slider_sampling_, TBM_SETRANGE, TRUE, MAKELPARAM(1, 100));
		::SendMessageA(hwnd_slider_quantization_, TBM_SETRANGE, TRUE, MAKELPARAM(1, 100));
		::SendMessageA(hwnd_slider_sampling_, TBM_SETPOS, TRUE, (LPARAM)(p_discretizer->sampling_resolution()));
		::SendMessageA(hwnd_slider_quantization_, TBM_SETPOS, TRUE, (LPARAM)(p_discretizer->quantization_resolution()));
		::SendMessageA(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_sampling_);
		::SendMessageA(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_quantization_);
	}


	INT_PTR DiscretizerModifierDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		static Discretizer* p_discretizer = nullptr;
		static Discretizer discretizer_prev{nullptr, nullptr};

		switch (message) {
		case WM_INITDIALOG:
			p_discretizer = reinterpret_cast<Discretizer*>(lparam);
			if (!p_discretizer) {
				::EndDialog(hwnd, IDCANCEL);
				return TRUE;
			}
			discretizer_prev = *p_discretizer;
			init_controls(hwnd, p_discretizer);
			return TRUE;

		case WM_HSCROLL:
			if (lparam == (LPARAM)hwnd_slider_sampling_) {
				int value = (int)::SendMessageA(hwnd_slider_sampling_, TBM_GETPOS, NULL, NULL);
				if (p_discretizer) {
					p_discretizer->set_sampling_resolution((uint32_t)value);
				}
				::SetWindowTextA(hwnd_static_sampling_, std::to_string(value).c_str());
				global::window_grapheditor.redraw();
			}
			else if (lparam == (LPARAM)hwnd_slider_quantization_) {
				int value = (int)::SendMessageA(hwnd_slider_quantization_, TBM_GETPOS, NULL, NULL);
				if (p_discretizer) {
					p_discretizer->set_quantization_resolution((uint32_t)value);
				}
				::SetWindowTextA(hwnd_static_quantization_, std::to_string(value).c_str());
				global::window_grapheditor.redraw();
			}
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case IDOK:
				::EndDialog(hwnd, IDOK);
				return TRUE;

			case IDCANCEL:
				if (p_discretizer) {
					*p_discretizer = discretizer_prev;
					global::window_grapheditor.redraw();
				}
				::EndDialog(hwnd, IDCANCEL);
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
}