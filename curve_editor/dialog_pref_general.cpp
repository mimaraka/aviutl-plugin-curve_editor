#include "config.hpp"
#include "dialog_pref_general.hpp"
#include "enum.hpp"
#include "message_box.hpp"
#include "resource.h"
#include "string_table.hpp"



namespace curve_editor {
	int GeneralPrefDialog::resource_id() const noexcept { return IDD_PREF_GENERAL; }

	void GeneralPrefDialog::init_controls(HWND hwnd) noexcept {
		using StringId = global::StringTable::StringId;

		hwnd_combo_language_ = ::GetDlgItem(hwnd, IDC_COMBO_LANGUAGE);
		hwnd_check_notify_update_ = ::GetDlgItem(hwnd, IDC_CHECK_NOTIFY_UPDATE);

		::SendMessageW(hwnd_combo_language_, CB_ADDSTRING, NULL, (LPARAM)global::string_table[StringId::WordAutomatic]);
		::SendMessageW(hwnd_combo_language_, CB_ADDSTRING, NULL, (LPARAM)L"日本語");
		::SendMessageW(hwnd_combo_language_, CB_ADDSTRING, NULL, (LPARAM)L"English");
		::SendMessageW(hwnd_combo_language_, CB_ADDSTRING, NULL, (LPARAM)L"Deutsch");
		::SendMessageW(hwnd_combo_language_, CB_ADDSTRING, NULL, (LPARAM)L"Bahasa Indonesia");
		::SendMessageW(hwnd_combo_language_, CB_ADDSTRING, NULL, (LPARAM)L"한국어");
		::SendMessageW(hwnd_combo_language_, CB_ADDSTRING, NULL, (LPARAM)L"简体中文");
	}

	INT_PTR GeneralPrefDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM) {
		using StringId = global::StringTable::StringId;

		switch (message) {
		case WM_INITDIALOG:
			init_controls(hwnd);
			return TRUE;

		case WM_COMMAND:
			switch (HIWORD(wparam)) {
			case CBN_SELCHANGE:
				util::message_box(global::string_table[StringId::InfoRestartAviutl], hwnd, util::MessageBoxIcon::Information);
				return TRUE;
			}
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
} // namespace curve_editor