#include "dialog_modifier_sine_wave.hpp"
#include "global.hpp"
#include "resource.h"
#include <Commctrl.h>
#include <format>



namespace cved {
	int SineWaveModifierDialog::resource_id() const noexcept { return IDD_MODIFIER_SINE_WAVE; }

	void SineWaveModifierDialog::init_controls(HWND hwnd, const SineWaveModifier* p_mod_sinewave) noexcept {
		hwnd_slider_amplitude_ = ::GetDlgItem(hwnd, IDC_SLIDER_AMP);
		hwnd_slider_frequency_ = ::GetDlgItem(hwnd, IDC_SLIDER_FREQ);
		hwnd_slider_phase_ = ::GetDlgItem(hwnd, IDC_SLIDER_PHASE);
		hwnd_static_amplitude_ = ::GetDlgItem(hwnd, IDC_STATIC_AMP);
		hwnd_static_frequency_ = ::GetDlgItem(hwnd, IDC_STATIC_FREQ);
		hwnd_static_phase_ = ::GetDlgItem(hwnd, IDC_STATIC_PHASE);

		::SendMessageA(hwnd_slider_amplitude_, TBM_SETRANGE, TRUE, MAKELPARAM(0, 1000));
		::SendMessageA(hwnd_slider_frequency_, TBM_SETRANGE, TRUE, MAKELPARAM(1, 1000));
		::SendMessageA(hwnd_slider_phase_, TBM_SETRANGE, TRUE, MAKELPARAM(0, 2000));

		::SendMessageA(hwnd_slider_amplitude_, TBM_SETPOS, TRUE, static_cast<LPARAM>(p_mod_sinewave->amplitude() * 100));
		::SendMessageA(hwnd_slider_frequency_, TBM_SETPOS, TRUE, static_cast<LPARAM>(p_mod_sinewave->frequency() * 10));
		::SendMessageA(hwnd_slider_phase_, TBM_SETPOS, TRUE, static_cast<LPARAM>(p_mod_sinewave->phase() * 100 + 1000));

		::SendMessageA(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_amplitude_);
		::SendMessageA(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_frequency_);
		::SendMessageA(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_phase_);
	}

	INT_PTR SineWaveModifierDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		static SineWaveModifier* p_mod_sinewave = nullptr;
		static SineWaveModifier mod_sinewave_prev;

		switch (message) {
		case WM_INITDIALOG:
			p_mod_sinewave = std::bit_cast<SineWaveModifier*>(lparam);
			if (!p_mod_sinewave) {
				::EndDialog(hwnd, IDCANCEL);
				return TRUE;
			}
			mod_sinewave_prev = *p_mod_sinewave;
			init_controls(hwnd, p_mod_sinewave);
			return TRUE;

		case WM_HSCROLL:
			if (lparam == (LPARAM)hwnd_slider_amplitude_) {
				double value = static_cast<double>(::SendMessageA(hwnd_slider_amplitude_, TBM_GETPOS, NULL, NULL)) * 0.01;
				if (p_mod_sinewave) {
					p_mod_sinewave->set_amplitude(value);
				}
				::SetWindowTextA(hwnd_static_amplitude_, std::format("{:.2f}", value).c_str());
				global::webview_main.post_message(L"editor-graph", L"updateCurvePath");
			}
			else if (lparam == (LPARAM)hwnd_slider_frequency_) {
				double value = static_cast<double>(::SendMessageA(hwnd_slider_frequency_, TBM_GETPOS, NULL, NULL)) * 0.1;
				if (p_mod_sinewave) {
					p_mod_sinewave->set_frequency(value);
				}
				::SetWindowTextA(hwnd_static_frequency_, std::format("{:.1f}", value).c_str());
				global::webview_main.post_message(L"editor-graph", L"updateCurvePath");
			}
			else if (lparam == (LPARAM)hwnd_slider_phase_) {
				double value = static_cast<double>(::SendMessageA(hwnd_slider_phase_, TBM_GETPOS, NULL, NULL) - 1000) * 0.01;
				if (p_mod_sinewave) {
					p_mod_sinewave->set_phase(value);
				}
				::SetWindowTextA(hwnd_static_phase_, std::format("{:.2f}", value).c_str());
				global::webview_main.post_message(L"editor-graph", L"updateCurvePath");
			}
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case IDCANCEL:
				if (p_mod_sinewave) {
					*p_mod_sinewave = mod_sinewave_prev;
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