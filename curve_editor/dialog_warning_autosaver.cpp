#include "config.hpp"
#include "dialog_warning_autosaver.hpp"
#include "resource.h"
#include <CommCtrl.h>



namespace curve_editor {
	int AutosaverWarningDialog::resource_id() const noexcept { return IDD_WARNING_AUTOSAVER; }

	void AutosaverWarningDialog::init_controls(HWND hwnd) noexcept {
		hwnd_check_dontshowagain_ = ::GetDlgItem(hwnd, IDC_CHECK_DONTSHOWAGAIN);
		::SendMessageA(hwnd_check_dontshowagain_, BM_SETCHECK, global::config.get_ignore_autosaver_warning(), NULL);
	}


	INT_PTR AutosaverWarningDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		switch (message) {
		case WM_INITDIALOG:
			init_controls(hwnd);
			return TRUE;

		case WM_NOTIFY:
			switch (((LPNMHDR)lparam)->code) {
				// SysLinkクリック時
			case NM_CLICK:
			case NM_RETURN:
			{
				PNMLINK p_nmlink = (PNMLINK)lparam;
				LITEM& item = p_nmlink->item;
				::ShellExecuteW(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOWNORMAL);
				return TRUE;
			}
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case IDOK:
				global::config.set_ignore_autosaver_warning(
					::SendMessageA(hwnd_check_dontshowagain_, BM_GETCHECK, NULL, NULL)
				);
				[[fallthrough]];

			case IDCANCEL:
				::EndDialog(hwnd, IDOK);
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
} // namespace curve_editor