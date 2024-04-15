#include "dialog_warning_autosaver.hpp"
#include "config.hpp"
#include "resource.h"



namespace cved {
	int AutosaverWarningDialog::i_resource() const noexcept { return IDD_WARNING_AUTOSAVER; }


	INT_PTR AutosaverWarningDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		constexpr char URL_GITHUB_AUTOSAVER[] = "https://github.com/ePi5131/autosaver/releases/latest";

		switch (message) {
		/*case WM_CLOSE:
			global::config.set_ignore_autosaver_warning(
				::SendMessageA(hwnd_check_dontshowagain_, BM_GETCHECK, NULL, NULL)
			);
			::EndDialog(hwnd, IDCANCEL);
			return TRUE;*/

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