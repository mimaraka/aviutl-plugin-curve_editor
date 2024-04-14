#include "dialog_id_jumpto.hpp"
#include "config.hpp"
#include "curve_editor.hpp"
#include "global.hpp"
#include "my_messagebox.hpp"
#include "string_table.hpp"
#include "resource.h"



namespace cved {
	int IdJumptoDialog::i_resource() const noexcept { return IDD_ID_JUMPTO; }


	void IdJumptoDialog::init_controls(HWND hwnd) noexcept {
		hwnd_edit_ = ::GetDlgItem(hwnd, IDC_EDIT_CURVE_CODE);
		::SendMessageA(hwnd_edit_, EM_SETLIMITTEXT, MAX_TEXT, NULL);
		::SetFocus(hwnd_edit_);
	}


	INT_PTR IdJumptoDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		using StringId = global::StringTable::StringId;

		switch (message) {
		case WM_INITDIALOG:
			init_controls(hwnd);
			return TRUE;

		case WM_KEYDOWN:
			if (wparam == VK_RETURN) {
				::SendMessageA(hwnd, WM_COMMAND, MAKEWPARAM(IDOK, 0), 0);
				return TRUE;
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case IDOK:
			{
				char buffer[MAX_TEXT + 1u];
				::GetDlgItemTextA(hwnd, IDC_EDIT_CURVE_CODE, buffer, MAX_TEXT);
				try {
					if (global::editor.set_idx((size_t)(std::stoi(buffer) - 1))) {
						::EndDialog(hwnd, IDOK);
					}
					// 入力値が範囲外の場合
					else {
						if (global::config.get_show_popup()) {
							my_messagebox(global::string_table[StringId::ErrorOutOfRange], hwnd, MessageBoxIcon::Error);
						}
					}
				}
				catch (std::invalid_argument&) {}
				catch (std::out_of_range&) {}

				return TRUE;
			}

			case IDCANCEL:
				::EndDialog(hwnd, IDCANCEL);
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
}