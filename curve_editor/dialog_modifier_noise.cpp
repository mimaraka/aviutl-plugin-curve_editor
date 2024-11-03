#include "dialog_modifier_noise.hpp"
#include "my_webview2_reference.hpp"
#include "resource.h"
#include <Commctrl.h>



namespace cved {
	int NoiseModifierDialog::resource_id() const noexcept { return IDD_MODIFIER_NOISE; }


	void NoiseModifierDialog::init_controls(HWND hwnd, const NoiseModifier* p_mod_noise) noexcept {
		hwnd_edit_seed_ = ::GetDlgItem(hwnd, IDC_EDIT_SEED);
		hwnd_slider_amplitude_ = ::GetDlgItem(hwnd, IDC_SLIDER_AMP);
		hwnd_slider_frequency_ = ::GetDlgItem(hwnd, IDC_SLIDER_FREQ);
		hwnd_slider_phase_ = ::GetDlgItem(hwnd, IDC_SLIDER_PHASE);
		hwnd_slider_octaves_ = ::GetDlgItem(hwnd, IDC_SLIDER_OCTAVES);
		hwnd_slider_decay_sharpness_ = ::GetDlgItem(hwnd, IDC_SLIDER_DECAY_SHARPNESS);
		hwnd_static_amplitude_ = ::GetDlgItem(hwnd, IDC_STATIC_AMP);
		hwnd_static_frequency_ = ::GetDlgItem(hwnd, IDC_STATIC_FREQ);
		hwnd_static_phase_ = ::GetDlgItem(hwnd, IDC_STATIC_PHASE);
		hwnd_static_octaves_ = ::GetDlgItem(hwnd, IDC_STATIC_OCTAVES);
		hwnd_static_decay_sharpness_ = ::GetDlgItem(hwnd, IDC_STATIC_DECAY_SHARPNESS);

		::SendMessageA(hwnd_edit_seed_, EM_SETLIMITTEXT, 15, NULL);
		::SetWindowTextA(hwnd_edit_seed_, std::to_string(p_mod_noise->seed()).c_str());
		::SendMessageA(hwnd_slider_amplitude_, TBM_SETRANGE, TRUE, MAKELPARAM(0, 1000));
		::SendMessageA(hwnd_slider_frequency_, TBM_SETRANGE, TRUE, MAKELPARAM(1, 1000));
		::SendMessageA(hwnd_slider_phase_, TBM_SETRANGE, TRUE, MAKELPARAM(0, 2000));
		::SendMessageA(hwnd_slider_octaves_, TBM_SETRANGE, TRUE, MAKELPARAM(1, 20));
		::SendMessageA(hwnd_slider_decay_sharpness_, TBM_SETRANGE, TRUE, MAKELPARAM(0, 1000));

		::SendMessageA(hwnd_slider_amplitude_, TBM_SETPOS, TRUE, static_cast<LPARAM>(p_mod_noise->amplitude() * 100));
		::SendMessageA(hwnd_slider_frequency_, TBM_SETPOS, TRUE, static_cast<LPARAM>(p_mod_noise->frequency() * 10));
		::SendMessageA(hwnd_slider_phase_, TBM_SETPOS, TRUE, static_cast<LPARAM>(p_mod_noise->phase() * 100 + 1000));
		::SendMessageA(hwnd_slider_octaves_, TBM_SETPOS, TRUE, static_cast<LPARAM>(p_mod_noise->octaves()));
		::SendMessageA(hwnd_slider_decay_sharpness_, TBM_SETPOS, TRUE, static_cast<LPARAM>(p_mod_noise->decay_sharpness() * 10));

		::SendMessageA(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_amplitude_);
		::SendMessageA(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_frequency_);
		::SendMessageA(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_phase_);
		::SendMessageA(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_octaves_);
		::SendMessageA(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_decay_sharpness_);
	}


	INT_PTR NoiseModifierDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		static NoiseModifier* p_mod_noise = nullptr;
		static NoiseModifier mod_noise_prev;

		switch (message) {
		case WM_INITDIALOG:
			p_mod_noise = std::bit_cast<NoiseModifier*>(lparam);
			if (!p_mod_noise) {
				::EndDialog(hwnd, IDCANCEL);
				return TRUE;
			}
			mod_noise_prev = *p_mod_noise;
			init_controls(hwnd, p_mod_noise);
			return TRUE;

		case WM_HSCROLL:
			if (lparam == (LPARAM)hwnd_slider_amplitude_) {
				double value = static_cast<double>(::SendMessageA(hwnd_slider_amplitude_, TBM_GETPOS, NULL, NULL)) * 0.01;
				if (p_mod_noise) {
					p_mod_noise->set_amplitude(value);
				}
				::SetWindowTextA(hwnd_static_amplitude_, std::format("{:.2f}", value).c_str());
				if (global::webview) global::webview->send_command(MessageCommand::UpdateCurvePath);
			}
			else if (lparam == (LPARAM)hwnd_slider_frequency_) {
				double value = static_cast<double>(::SendMessageA(hwnd_slider_frequency_, TBM_GETPOS, NULL, NULL)) * 0.1;
				if (p_mod_noise) {
					p_mod_noise->set_frequency(value);
					p_mod_noise->update();
				}
				::SetWindowTextA(hwnd_static_frequency_, std::format("{:.1f}", value).c_str());
				if (global::webview) global::webview->send_command(MessageCommand::UpdateCurvePath);
			}
			else if (lparam == (LPARAM)hwnd_slider_phase_) {
				double value = static_cast<double>(::SendMessageA(hwnd_slider_phase_, TBM_GETPOS, NULL, NULL) - 1000) * 0.01;
				if (p_mod_noise) {
					p_mod_noise->set_phase(value);
				}
				::SetWindowTextA(hwnd_static_phase_, std::format("{:.2f}", value).c_str());
				if (global::webview) global::webview->send_command(MessageCommand::UpdateCurvePath);
			}
			else if (lparam == (LPARAM)hwnd_slider_octaves_) {
				int32_t value = ::SendMessageA(hwnd_slider_octaves_, TBM_GETPOS, NULL, NULL);
				if (p_mod_noise) {
					p_mod_noise->set_octaves(value);
					p_mod_noise->update();
				}
				::SetWindowTextA(hwnd_static_octaves_, std::to_string(value).c_str());
				if (global::webview) global::webview->send_command(MessageCommand::UpdateCurvePath);
			}
			else if (lparam == (LPARAM)hwnd_slider_decay_sharpness_) {
				double value = static_cast<double>(::SendMessageA(hwnd_slider_decay_sharpness_, TBM_GETPOS, NULL, NULL)) * 0.1;
				if (p_mod_noise) {
					p_mod_noise->set_decay_sharpness(value);
				}
				::SetWindowTextA(hwnd_static_decay_sharpness_, std::format("{:.1f}", value).c_str());
				if (global::webview) global::webview->send_command(MessageCommand::UpdateCurvePath);
			}
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case IDCANCEL:
				if (p_mod_noise) {
					*p_mod_noise = mod_noise_prev;
					if (global::webview) global::webview->send_command(MessageCommand::UpdateCurvePath);
				}
				[[fallthrough]];
			case IDOK:
				::EndDialog(hwnd, IDCANCEL);
				return TRUE;
			}
			switch (HIWORD(wparam)) {
			case EN_CHANGE:
				if (LOWORD(wparam) == IDC_EDIT_SEED) {
					char buffer[16];
					::GetWindowTextA(hwnd_edit_seed_, buffer, sizeof(buffer));
					if (p_mod_noise) {
						try {
							p_mod_noise->set_seed(std::stoul(buffer));
							p_mod_noise->update();
						}
						catch (std::invalid_argument&) {}
						catch (std::out_of_range&) {}
					}
					if (global::webview) global::webview->send_command(MessageCommand::UpdateCurvePath);
				}
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
}