#include "dialog_pref_general.hpp"
#include "config.hpp"
#include "enum.hpp"
#include "string_table.hpp"
#include "resource.h"



namespace cved {
	int GeneralPrefDialog::i_resource() const noexcept { return IDD_PREF_GENERAL; }

	void GeneralPrefDialog::init_controls(HWND hwnd) noexcept {
		using StringId = global::StringTable::StringId;

		hwnd_combo_language_ = ::GetDlgItem(hwnd, IDC_COMBO_LANGUAGE);
		hwnd_check_notify_update_ = ::GetDlgItem(hwnd, IDC_CHECK_NOTIFY_UPDATE);

		::SendMessageA(hwnd_combo_language_, CB_ADDSTRING, NULL, (LPARAM)global::string_table[StringId::WordLanguageAutomatic]);
		::SendMessageA(hwnd_combo_language_, CB_ADDSTRING, NULL, (LPARAM)"日本語 (Japanese)");
		::SendMessageA(hwnd_combo_language_, CB_ADDSTRING, NULL, (LPARAM)"English");
		::SendMessageA(hwnd_combo_language_, CB_ADDSTRING, NULL, (LPARAM)" (Korean)");
	}

	INT_PTR GeneralPrefDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		using StringId = global::StringTable::StringId;

		switch (message) {
		case WM_INITDIALOG:
			init_controls(hwnd);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case (WPARAM)WindowCommand::LoadConfig:
				::SendMessageA(hwnd_combo_language_, CB_SETCURSEL, (WPARAM)global::config.get_language(), NULL);

				::SendMessageA(
					hwnd_check_notify_update_,
					BM_SETCHECK,
					(WPARAM)global::config.get_notify_update(),
					NULL
				);
				return TRUE;

			case (WPARAM)WindowCommand::SaveConfig:
				global::config.set_language((Language)::SendMessageA(hwnd_combo_language_, CB_GETCURSEL, NULL, NULL));
				global::config.set_notify_update(::SendMessageA(hwnd_check_notify_update_, BM_GETCHECK, NULL, NULL));
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
}