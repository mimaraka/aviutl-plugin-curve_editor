#include "dialog_pref_behavior.hpp"
#include <mkaul/include/graphics.hpp>
#include "config.hpp"
#include "enum.hpp"
#include "my_messagebox.hpp"
#include "string_table.hpp"
#include "resource.h"



namespace cved {
	int BehaviorPrefDialog::i_resource() const noexcept { return IDD_PREF_BEHAVIOR; }

	void BehaviorPrefDialog::init_controls(HWND hwnd) noexcept {
		hwnd_combo_graphic_method_ = ::GetDlgItem(hwnd, IDC_COMBO_GRAPHIC_METHOD);
		hwnd_check_show_popup_ = ::GetDlgItem(hwnd, IDC_CHECK_SHOW_POPUP);
		hwnd_check_enable_hotkeys_ = ::GetDlgItem(hwnd, IDC_CHECK_ENABLE_HOTKEYS);

		::SendMessageA(hwnd_combo_graphic_method_, CB_ADDSTRING, NULL, (LPARAM)"GDI+");
		::SendMessageA(hwnd_combo_graphic_method_, CB_ADDSTRING, NULL, (LPARAM)"DirectX");
	}

	INT_PTR BehaviorPrefDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		using StringId = global::StringTable::StringId;

		switch (message) {
		case WM_INITDIALOG:
			init_controls(hwnd);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case (WPARAM)WindowCommand::LoadConfig:
				::SendMessageA(hwnd_combo_graphic_method_, CB_SETCURSEL, (WPARAM)global::config.get_graphic_method(), NULL);
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
				global::config.set_graphic_method(
					(mkaul::graphics::Factory::GraphicEngine)::SendMessageA(hwnd_combo_graphic_method_, CB_GETCURSEL, NULL, NULL)
				);
				global::config.set_show_popup(
					::SendMessageA(hwnd_check_show_popup_, BM_GETCHECK, NULL, NULL)
				);
				global::config.set_enable_hotkeys(
					::SendMessageA(hwnd_check_enable_hotkeys_, BM_GETCHECK, NULL, NULL)
				);
				return TRUE;
			}
			switch (HIWORD(wparam)) {
			case CBN_SELCHANGE:
				my_messagebox(global::string_table[StringId::InfoRestartAviutl], hwnd);
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
}