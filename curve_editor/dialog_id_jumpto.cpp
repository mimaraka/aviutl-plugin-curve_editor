#include "config.hpp"
#include "curve_editor.hpp"
#include "dialog_id_jumpto.hpp"
#include "global.hpp"
#include "message_box.hpp"
#include "resource.h"
#include "string_table.hpp"



namespace cved {
	int IdJumptoDialog::resource_id() const noexcept { return IDD_ID_JUMPTO; }


	void IdJumptoDialog::init_controls(HWND hwnd) noexcept {
		hwnd_edit_ = ::GetDlgItem(hwnd, IDC_EDIT_ID_JUMPTO);
		hwnd_list_ = ::GetDlgItem(hwnd, IDC_LIST_ID_JUMPTO);
		hwnd_button_back_ = ::GetDlgItem(hwnd, IDC_BUTTON_BACK);
		hwnd_button_forward_ = ::GetDlgItem(hwnd, IDC_BUTTON_FORWARD);
	}


	INT_PTR IdJumptoDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM) {
		using StringId = global::StringTable::StringId;

		switch (message) {
		case WM_INITDIALOG:
			return TRUE;

		case WM_DESTROY:
		case WM_CLOSE:
			break;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case IDOK:
			case IDCANCEL:
				::EndDialog(hwnd, 1);
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
} // namespace cved