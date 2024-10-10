#include "dialog_modifier_square_wave.hpp"
#include "global.hpp"
#include "resource.h"
#include <Commctrl.h>
#include <format>



namespace cved {
	int SquareWaveModifierDialog::resource_id() const noexcept { return IDD_MODIFIER_SQUARE_WAVE; }

	void SquareWaveModifierDialog::init_controls(HWND hwnd, const SquareWaveModifier* p_mod_square_wave) noexcept {
		hwnd_slider_amplitude_ = ::GetDlgItem(hwnd, IDC_SLIDER_AMP);
		hwnd_slider_frequency_ = ::GetDlgItem(hwnd, IDC_SLIDER_FREQ);
		hwnd_slider_phase_ = ::GetDlgItem(hwnd, IDC_SLIDER_PHASE);
		hwnd_static_amplitude_ = ::GetDlgItem(hwnd, IDC_STATIC_AMP);
		hwnd_static_frequency_ = ::GetDlgItem(hwnd, IDC_STATIC_FREQ);
		hwnd_static_phase_ = ::GetDlgItem(hwnd, IDC_STATIC_PHASE);

		::SendMessageA(hwnd_slider_amplitude_, TBM_SETRANGE, TRUE, MAKELPARAM(0, 1000));
		::SendMessageA(hwnd_slider_frequency_, TBM_SETRANGE, TRUE, MAKELPARAM(1, 1000));
		::SendMessageA(hwnd_slider_phase_, TBM_SETRANGE, TRUE, MAKELPARAM(0, 2000));

		::SendMessageA(hwnd_slider_amplitude_, TBM_SETPOS, TRUE, static_cast<LPARAM>(p_mod_square_wave->amplitude() * 100));
		::SendMessageA(hwnd_slider_frequency_, TBM_SETPOS, TRUE, static_cast<LPARAM>(p_mod_square_wave->frequency() * 10));
		::SendMessageA(hwnd_slider_phase_, TBM_SETPOS, TRUE, static_cast<LPARAM>(p_mod_square_wave->phase() * 100 + 1000));

		::SendMessageA(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_amplitude_);
		::SendMessageA(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_frequency_);
		::SendMessageA(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_phase_);
	}

	INT_PTR SquareWaveModifierDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		static SquareWaveModifier* p_mod_square_wave = nullptr;
		static SquareWaveModifier mod_square_wave_prev;

		switch (message) {
		case WM_INITDIALOG:
			p_mod_square_wave = reinterpret_cast<SquareWaveModifier*>(lparam);
			if (!p_mod_square_wave) {
				::EndDialog(hwnd, IDCANCEL);
				return TRUE;
			}
			mod_square_wave_prev = *p_mod_square_wave;
			init_controls(hwnd, p_mod_square_wave);
			return TRUE;

		case WM_HSCROLL:
			if (lparam == (LPARAM)hwnd_slider_amplitude_) {
				double value = static_cast<double>(::SendMessageA(hwnd_slider_amplitude_, TBM_GETPOS, NULL, NULL)) * 0.01;
				if (p_mod_square_wave) {
					p_mod_square_wave->set_amplitude(value);
				}
				::SetWindowTextA(hwnd_static_amplitude_, std::format("{:.2f}", value).c_str());
				global::webview_main.post_message(L"editor-graph", L"updateCurvePath");
			}
			else if (lparam == (LPARAM)hwnd_slider_frequency_) {
				double value = static_cast<double>(::SendMessageA(hwnd_slider_frequency_, TBM_GETPOS, NULL, NULL)) * 0.1;
				if (p_mod_square_wave) {
					p_mod_square_wave->set_frequency(value);
				}
				::SetWindowTextA(hwnd_static_frequency_, std::format("{:.1f}", value).c_str());
				global::webview_main.post_message(L"editor-graph", L"updateCurvePath");
			}
			else if (lparam == (LPARAM)hwnd_slider_phase_) {
				double value = static_cast<double>(::SendMessageA(hwnd_slider_phase_, TBM_GETPOS, NULL, NULL) - 1000) * 0.01;
				if (p_mod_square_wave) {
					p_mod_square_wave->set_phase(value);
				}
				::SetWindowTextA(hwnd_static_phase_, std::format("{:.2f}", value).c_str());
				global::webview_main.post_message(L"editor-graph", L"updateCurvePath");
			}
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case IDCANCEL:
				if (p_mod_square_wave) {
					*p_mod_square_wave = mod_square_wave_prev;
					global::webview_main.post_message(L"editor-graph", L"updateCurvePath");
				}
				[[fallthrough]];
			case IDOK:
				::EndDialog(hwnd, LOWORD(wparam));
				return TRUE;
			}
			break;
		}

		return FALSE;
	}
} // namespace cved