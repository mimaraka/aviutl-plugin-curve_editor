#include "dlgproc_config.hpp"
#include "dlgproc_config_general.hpp"
#include "dlgproc_config_appearance.hpp"
#include "dlgproc_config_behavior.hpp"
#include "dlgproc_config_editing.hpp"
#include "dialog_config_contents.hpp"
#include "global.hpp"
#include "string_table.hpp"
#include "resource.h"



namespace cved {
	INT_PTR CALLBACK dlgproc_config(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		using StringId = global::StringTable::StringId;
		static ConfigDialogContents contents;

		switch (message) {
		case WM_INITDIALOG:
			contents.init(hwnd);
			break;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case IDOK:
				::EndDialog(hwnd, 1);
				break;

			case IDCANCEL:
				::EndDialog(hwnd, 1);
				break;
			}
			switch (HIWORD(wparam)) {
			case LBN_SELCHANGE:
				contents.update();
				break;
			}
			break;
		}
		return 0;
	}
}