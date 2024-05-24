#include "dialog_modifier_discretization.hpp"
#include <Commctrl.h>
#include "global.hpp"
#include "resource.h"



namespace cved {
	int CurveDiscretizationDialog::i_resource() const noexcept { return IDD_MODIFIER_DISCRETIZATION; }


	void CurveDiscretizationDialog::init_controls(HWND hwnd, const GraphCurve* p_curve) noexcept {
		hwnd_slider_sampling_ = ::GetDlgItem(hwnd, IDC_SLIDER_SAMPLING);
		hwnd_slider_quantization_ = ::GetDlgItem(hwnd, IDC_SLIDER_QUANTIZATION);
		hwnd_static_sampling_ = ::GetDlgItem(hwnd, IDC_STATIC_SAMPLING);
		hwnd_static_quantization_ = ::GetDlgItem(hwnd, IDC_STATIC_QUANTIZATION);
		
		::SendMessageA(hwnd_slider_sampling_, TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
		::SendMessageA(hwnd_slider_quantization_, TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
		::SendMessageA(hwnd_slider_sampling_, TBM_SETPOS, TRUE, (LPARAM)(p_curve->get_sampling_resolution()));
		::SendMessageA(hwnd_slider_quantization_, TBM_SETPOS, TRUE, (LPARAM)(p_curve->get_quantization_resolution()));
		::SendMessageA(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_sampling_);
		::SendMessageA(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_quantization_);
	}


	INT_PTR CurveDiscretizationDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		static GraphCurve* p_curve = nullptr;
		static uint32_t sampling_resolution_prev = 0u;
		static uint32_t quantization_resolution_prev = 0u;

		switch (message) {
		case WM_INITDIALOG:
			p_curve = reinterpret_cast<GraphCurve*>(lparam);
			if (!p_curve) return FALSE;
			sampling_resolution_prev = p_curve->get_sampling_resolution();
			quantization_resolution_prev = p_curve->get_quantization_resolution();
			init_controls(hwnd, p_curve);
			return TRUE;

		case WM_HSCROLL:
			if (lparam == (LPARAM)hwnd_slider_sampling_) {
				int value = (int)::SendMessageA(hwnd_slider_sampling_, TBM_GETPOS, NULL, NULL);
				if (p_curve) {
					p_curve->set_sampling_resolution((uint32_t)value);
				}
				::SetWindowTextA(hwnd_static_sampling_, std::to_string(value).c_str());
				global::window_grapheditor.redraw();
			}
			else if (lparam == (LPARAM)hwnd_slider_quantization_) {
				int value = (int)::SendMessageA(hwnd_slider_quantization_, TBM_GETPOS, NULL, NULL);
				if (p_curve) {
					p_curve->set_quantization_resolution((uint32_t)value);
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
				if (p_curve) {
					p_curve->set_sampling_resolution(sampling_resolution_prev);
					p_curve->set_quantization_resolution(quantization_resolution_prev);
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