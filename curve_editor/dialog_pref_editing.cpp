#include "config.hpp"
#include "dialog_pref_editing.hpp"
#include "enum.hpp"
#include "message_box.hpp"
#include "resource.h"
#include "string_table.hpp"



namespace curve_editor {
	int EditingPrefDialog::resource_id() const noexcept { return IDD_PREF_EDITING; }

	void EditingPrefDialog::init_controls(HWND hwnd) noexcept {
		hwnd_check_reverse_wheel_ = ::GetDlgItem(hwnd, IDC_CHECK_REVERSE_WHEEL);
		hwnd_check_auto_copy_ = ::GetDlgItem(hwnd, IDC_CHECK_AUTO_COPY);
		hwnd_check_auto_apply_ = ::GetDlgItem(hwnd, IDC_CHECK_AUTO_APPLY);
		hwnd_check_word_wrap_ = ::GetDlgItem(hwnd, IDC_CHECK_WORD_WRAP);
	}

	INT_PTR EditingPrefDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM) {
		switch (message) {
		case WM_INITDIALOG:
			init_controls(hwnd);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case (WPARAM)WindowCommand::LoadConfig:
				::SendMessageA(
					hwnd_check_reverse_wheel_,
					BM_SETCHECK,
					(WPARAM)global::config.get_invert_wheel(),
					NULL
				);
				::SendMessageA(
					hwnd_check_auto_copy_,
					BM_SETCHECK,
					(WPARAM)global::config.get_auto_copy(),
					NULL
				);
				::SendMessageA(
					hwnd_check_auto_apply_,
					BM_SETCHECK,
					(WPARAM)global::config.get_auto_apply(),
					NULL
				);
				::SendMessageA(
					hwnd_check_word_wrap_,
					BM_SETCHECK,
					(WPARAM)global::config.get_word_wrap(),
					NULL
				);
				return TRUE;

			case (WPARAM)WindowCommand::SaveConfig:
				global::config.set_invert_wheel(::SendMessageA(hwnd_check_reverse_wheel_, BM_GETCHECK, NULL, NULL));
				global::config.set_auto_copy(::SendMessageA(hwnd_check_auto_copy_, BM_GETCHECK, NULL, NULL));
				global::config.set_auto_apply(::SendMessageA(hwnd_check_auto_apply_, BM_GETCHECK, NULL, NULL));
				global::config.set_word_wrap(::SendMessageA(hwnd_check_word_wrap_, BM_GETCHECK, NULL, NULL));
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
} // namespace curve_editor