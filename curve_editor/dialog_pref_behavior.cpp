#include "config.hpp"
#include "dialog_pref_behavior.hpp"
#include "enum.hpp"
#include "message_box.hpp"
#include "resource.h"
#include "string_table.hpp"
#include <mkaul/graphics.hpp>



namespace cved {
	int BehaviorPrefDialog::resource_id() const noexcept { return IDD_PREF_BEHAVIOR; }

	void BehaviorPrefDialog::init_controls(HWND hwnd) noexcept {
		hwnd_check_show_popup_ = ::GetDlgItem(hwnd, IDC_CHECK_SHOW_POPUP);
		hwnd_check_enable_hotkeys_ = ::GetDlgItem(hwnd, IDC_CHECK_ENABLE_HOTKEYS);
	}

	INT_PTR BehaviorPrefDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM) {
		using StringId = global::StringTable::StringId;

		switch (message) {
		case WM_INITDIALOG:
			init_controls(hwnd);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case (WPARAM)WindowCommand::LoadConfig:
				::SendMessageA(
					hwnd_check_show_popup_,
					BM_SETCHECK,
					(WPARAM)global::config.get_show_popup(),
					NULL
				);
				::SendMessageA(
					hwnd_check_enable_hotkeys_,
					BM_SETCHECK,
					(WPARAM)global::config.get_enable_hotkeys(),
					NULL
				);
				return TRUE;

			case (WPARAM)WindowCommand::SaveConfig:
				global::config.set_show_popup(
					::SendMessageA(hwnd_check_show_popup_, BM_GETCHECK, NULL, NULL)
				);
				global::config.set_enable_hotkeys(
					::SendMessageA(hwnd_check_enable_hotkeys_, BM_GETCHECK, NULL, NULL)
				);
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
} // namespace cved