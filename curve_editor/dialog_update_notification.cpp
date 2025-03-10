#include <strconv2.h>

#include "config.hpp"
#include "dialog_update_notification.hpp"
#include "string_table.hpp"
#include "update_checker.hpp"
#include "resource.h"



namespace curve_editor {
	int UpdateNotificationDialog::resource_id() const noexcept { return IDD_UPDATE_NOTIFICATION; }

	void UpdateNotificationDialog::init_controls(HWND hwnd) noexcept {
		hwnd_static_version_ = ::GetDlgItem(hwnd, IDC_STATIC_VERSION);
		hwnd_edit_release_notes_ = ::GetDlgItem(hwnd, IDC_EDIT_RELEASE_NOTES);
		hwnd_check_dontshowagain_ = ::GetDlgItem(hwnd, IDC_CHECK_DONTSHOWAGAIN);
		::SetWindowTextW(
			hwnd_static_version_,
			std::format(
				L"{}:\t{}  →  {}",
				global::string_table[global::StringTable::StringId::WordVersion],
				::sjis_to_wide(global::PLUGIN_VERSION.str()),
				::sjis_to_wide(global::update_checker.latest_version().str())
			).c_str()
		);
		::SetWindowTextW(hwnd_edit_release_notes_, global::update_checker.release_notes().c_str());
		::SendMessageA(hwnd_check_dontshowagain_, BM_SETCHECK, !global::config.get_notify_update(), NULL);
	}


	INT_PTR UpdateNotificationDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM) {
		switch (message) {
		case WM_INITDIALOG:
			init_controls(hwnd);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case IDOK:
				global::config.set_notify_update(
					!static_cast<bool>(::SendMessageA(hwnd_check_dontshowagain_, BM_GETCHECK, NULL, NULL))
				);
				::ShellExecuteW(nullptr, L"open", std::format(L"{}/releases/latest", global::PLUGIN_GITHUB_URL).c_str(), nullptr, nullptr, SW_SHOWNORMAL);
				::EndDialog(hwnd, IDOK);
				return TRUE;

			case IDCANCEL:
				global::config.set_notify_update(
					!static_cast<bool>(::SendMessageA(hwnd_check_dontshowagain_, BM_GETCHECK, NULL, NULL))
				);
				::EndDialog(hwnd, IDCANCEL);
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
} // namespace curve_editor