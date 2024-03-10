#include "dialog_about.hpp"
#include <format>
#include <CommCtrl.h>
#include "constants.hpp"
#include "string_table.hpp"
#include "resource.h"



namespace cved {
	int AboutDialog::i_resource() const noexcept { return IDD_ABOUT; }


	void AboutDialog::init_controls(HWND hwnd) noexcept {
		using StringId = global::StringTable::StringId;
		hwnd_static_info_ = ::GetDlgItem(hwnd, IDC_STATIC_PLUGIN_INFO);
		::SetWindowTextA(hwnd_static_info_, std::format(
			"{} : {}\nDeveloped by {}\nTranslated by {}",
			global::string_table[StringId::WordVersion],
			global::PLUGIN_VERSION.str(),
			global::PLUGIN_DEVELOPER,
			global::PLUGIN_TRANSLATOR
		).c_str());
	}


	INT_PTR AboutDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		using StringId = global::StringTable::StringId;

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
				::EndDialog(hwnd, 1);
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
}