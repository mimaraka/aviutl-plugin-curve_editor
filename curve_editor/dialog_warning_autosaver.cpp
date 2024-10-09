#include "config.hpp"
#include "dialog_warning_autosaver.hpp"
#include "resource.h"



namespace cved {
	int AutosaverWarningDialog::resource_id() const noexcept { return IDD_WARNING_AUTOSAVER; }

	void AutosaverWarningDialog::init_controls(HWND hwnd) noexcept {
		hwnd_check_dontshowagain_ = ::GetDlgItem(hwnd, IDC_CHECK_DONTSHOWAGAIN);
		::SendMessageA(hwnd_check_dontshowagain_, BM_SETCHECK, global::config.get_ignore_autosaver_warning(), NULL);
	}


	INT_PTR AutosaverWarningDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM) {
		constexpr char URL_GITHUB_AUTOSAVER[] = "https://github.com/ePi5131/autosaver/releases/latest";

		switch (message) {
		/*case WM_CLOSE:
			global::config.set_ignore_autosaver_warning(
				::SendMessageA(hwnd_check_dontshowagain_, BM_GETCHECK, NULL, NULL)
			);
			::EndDialog(hwnd, IDCANCEL);
			return TRUE;*/

		case WM_INITDIALOG:
			init_controls(hwnd);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case IDOK:
				global::config.set_ignore_autosaver_warning(
					::SendMessageA(hwnd_check_dontshowagain_, BM_GETCHECK, NULL, NULL)
				);
				::ShellExecuteA(NULL, "open", URL_GITHUB_AUTOSAVER, NULL, NULL, SW_SHOWNORMAL);
				::EndDialog(hwnd, IDOK);
				return TRUE;

			case IDCANCEL:
				global::config.set_ignore_autosaver_warning(
					::SendMessageA(hwnd_check_dontshowagain_, BM_GETCHECK, NULL, NULL)
				);
				::EndDialog(hwnd, IDCANCEL);
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
}