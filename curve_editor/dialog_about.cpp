#include "constants.hpp"
#include "dialog_about.hpp"
#include "resource.h"
#include "string_table.hpp"
#include <CommCtrl.h>
#include <format>



namespace curve_editor {
	int AboutDialog::resource_id() const noexcept { return IDD_ABOUT; }


	void AboutDialog::init_controls(HWND hwnd) noexcept {
		using StringId = global::StringTable::StringId;
		hwnd_static_info_ = ::GetDlgItem(hwnd, IDC_STATIC_PLUGIN_INFO);
		::SetWindowTextA(hwnd_static_info_, std::format(
			"{} : {}\n\nDeveloped by {}\nTranslated by {}",
			global::string_table[StringId::WordVersion],
			global::PLUGIN_VERSION.str(),
			global::PLUGIN_DEVELOPER,
			global::PLUGIN_TRANSLATOR
		).c_str());

		// TODO: ロゴ画像が容易でき次第削除する
		HWND hwnd_static_logo = ::GetDlgItem(hwnd, IDC_STATIC_LOGO);
		font_title_ = ::CreateFontA(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Yu Gothic UI");
		::SendMessageA(hwnd_static_logo, WM_SETFONT, (WPARAM)font_title_, TRUE);
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
			case IDCANCEL:
				::DeleteObject(font_title_);
				::EndDialog(hwnd, 1);
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
} // namespace curve_editor