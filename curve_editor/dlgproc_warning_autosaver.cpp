#include "dlgproc_warning_autosaver.hpp"
#include "config.hpp"
#include "resource.h"



namespace cved {
	INT_PTR CALLBACK dlgproc_warning_autosaver(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		constexpr char URL_GITHUB_AUTOSAVER[] = "https://github.com/ePi5131/autosaver/releases/latest";

		switch (message) {
		case WM_CLOSE:
			global::config.set_ignore_autosaver_warning(
				::SendMessageA(
					::GetDlgItem(hwnd, IDC_DONTSHOWAGAIN),
					BM_GETCHECK, 0, 0
				)
			);
			::EndDialog(hwnd, 1);
			break;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case IDOK:
				global::config.set_ignore_autosaver_warning(
					::SendMessageA(
						::GetDlgItem(hwnd, IDC_DONTSHOWAGAIN),
						BM_GETCHECK, 0, 0
					)
				);
				::ShellExecuteA(NULL, "open", URL_GITHUB_AUTOSAVER, NULL, NULL, SW_SHOWNORMAL);
				::EndDialog(hwnd, 1);
				break;

			case IDCANCEL:
				global::config.set_ignore_autosaver_warning(
					::SendMessageA(
						::GetDlgItem(hwnd, IDC_DONTSHOWAGAIN),
						BM_GETCHECK, 0, 0
					)
				);
				::EndDialog(hwnd, 1);
				break;
			}
			break;
		}
		return 0;
	}
}