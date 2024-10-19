#include "dialog_modifier_discretization.hpp"
#include "my_webview2_reference.hpp"
#include "resource.h"
#include <Commctrl.h>



namespace cved {
	int DiscretizationModifierDialog::resource_id() const noexcept { return IDD_MODIFIER_DISCRETIZATION; }


	void DiscretizationModifierDialog::init_controls(HWND hwnd, const DiscretizationModifier* p_mod_discretization) noexcept {
		hwnd_slider_sampling_ = ::GetDlgItem(hwnd, IDC_SLIDER_SAMPLING);
		hwnd_slider_quantization_ = ::GetDlgItem(hwnd, IDC_SLIDER_QUANTIZATION);
		hwnd_static_sampling_ = ::GetDlgItem(hwnd, IDC_STATIC_SAMPLING);
		hwnd_static_quantization_ = ::GetDlgItem(hwnd, IDC_STATIC_QUANTIZATION);
		
		::SendMessageA(hwnd_slider_sampling_, TBM_SETRANGE, TRUE, MAKELPARAM(1, 100));
		::SendMessageA(hwnd_slider_quantization_, TBM_SETRANGE, TRUE, MAKELPARAM(1, 100));
		::SendMessageA(hwnd_slider_sampling_, TBM_SETPOS, TRUE, (LPARAM)(p_mod_discretization->sampling_resolution()));
		::SendMessageA(hwnd_slider_quantization_, TBM_SETPOS, TRUE, (LPARAM)(p_mod_discretization->quantization_resolution()));
		::SendMessageA(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_sampling_);
		::SendMessageA(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_quantization_);
	}


	INT_PTR DiscretizationModifierDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		static DiscretizationModifier* p_mod_discretization = nullptr;
		static DiscretizationModifier mod_discretization_prev;

		switch (message) {
		case WM_INITDIALOG:
			p_mod_discretization = std::bit_cast<DiscretizationModifier*>(lparam);
			if (!p_mod_discretization) {
				::EndDialog(hwnd, IDCANCEL);
				return TRUE;
			}
			mod_discretization_prev = *p_mod_discretization;
			init_controls(hwnd, p_mod_discretization);
			return TRUE;

		case WM_HSCROLL:
			if (lparam == (LPARAM)hwnd_slider_sampling_) {
				int value = (int)::SendMessageA(hwnd_slider_sampling_, TBM_GETPOS, NULL, NULL);
				if (p_mod_discretization) {
					p_mod_discretization->set_sampling_resolution((uint32_t)value);
				}
				::SetWindowTextA(hwnd_static_sampling_, std::to_string(value).c_str());
				if (global::webview) global::webview->post_message(L"editor-graph", L"updateCurvePath");
			}
			else if (lparam == (LPARAM)hwnd_slider_quantization_) {
				int value = (int)::SendMessageA(hwnd_slider_quantization_, TBM_GETPOS, NULL, NULL);
				if (p_mod_discretization) {
					p_mod_discretization->set_quantization_resolution((uint32_t)value);
				}
				::SetWindowTextA(hwnd_static_quantization_, std::to_string(value).c_str());
				if (global::webview) global::webview->post_message(L"editor-graph", L"updateCurvePath");
			}
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case IDOK:
				::EndDialog(hwnd, IDOK);
				return TRUE;

			case IDCANCEL:
				if (p_mod_discretization) {
					*p_mod_discretization = mod_discretization_prev;
					if (global::webview) global::webview->post_message(L"editor-graph", L"updateCurvePath");
				}
				::EndDialog(hwnd, IDCANCEL);
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
}