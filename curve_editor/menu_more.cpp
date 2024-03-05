#include "menu_more.hpp"
#include <format>
#include "config.hpp"
#include "constants.hpp"
#include "global.hpp"
#include "dlgproc_config.hpp"
#include "string_table.hpp"
#include "util.hpp"
#include "resource.h"



namespace cved {
	MoreMenu::MoreMenu(HINSTANCE hinst) {
		menu_ = ::GetSubMenu(::LoadMenuA(hinst, MAKEINTRESOURCEA(IDR_MENU_MORE)), 0);
		MENUITEMINFOA minfo_tmp;
		minfo_tmp.cbSize = sizeof(MENUITEMINFOA);
		minfo_tmp.fMask = MIIM_FTYPE;
		minfo_tmp.fType = MFT_RADIOCHECK;

		::SetMenuItemInfoA(menu_, ID_LAYOUT_VERTICAL, FALSE, &minfo_tmp);
		::SetMenuItemInfoA(menu_, ID_LAYOUT_HORIZONTAL, FALSE, &minfo_tmp);
	}

	void MoreMenu::update_state() noexcept {
		// レイアウトのチェックを変更
		MENUITEMINFOA minfo_tmp;
		minfo_tmp.cbSize = sizeof(MENUITEMINFOA);
		minfo_tmp.fMask = MIIM_STATE;
		if (global::config.get_layout_mode() == LayoutMode::Vertical) {
			minfo_tmp.fState = MFS_CHECKED;
			::SetMenuItemInfoA(menu_, ID_LAYOUT_VERTICAL, FALSE, &minfo_tmp);
			minfo_tmp.fState = MFS_UNCHECKED;
			::SetMenuItemInfoA(menu_, ID_LAYOUT_HORIZONTAL, FALSE, &minfo_tmp);
		}
		else {
			minfo_tmp.fState = MFS_UNCHECKED;
			::SetMenuItemInfoA(menu_, ID_LAYOUT_VERTICAL, FALSE, &minfo_tmp);
			minfo_tmp.fState = MFS_CHECKED;
			::SetMenuItemInfoA(menu_, ID_LAYOUT_HORIZONTAL, FALSE, &minfo_tmp);
		}
		// 「ライブラリを表示」のチェックを変更
		minfo_tmp.fState = global::config.get_show_library() ? MFS_CHECKED : MFS_UNCHECKED;
		::SetMenuItemInfoA(menu_, ID_MORE_SHOWLIBRARY, FALSE, &minfo_tmp);
	}

	bool MoreMenu::callback(WPARAM wparam, LPARAM lparam) noexcept {
		using StringId = global::StringTable::StringId;

		switch (LOWORD(wparam)) {
		case ID_MORE_SHOWLIBRARY:
			global::config.set_show_library(!global::config.get_show_library());
			global::window_main.send_command((WPARAM)WindowCommand::Update);
			break;

		case ID_MORE_PREFERENCES:
			::DialogBoxA(global::fp->dll_hinst, MAKEINTRESOURCEA(IDD_CONFIG), global::fp->hwnd, dlgproc_config);
			break;

		case ID_MORE_HELP:
			::ShellExecuteA(NULL, "open", std::format("{}/wiki", global::PLUGIN_GITHUB_URL).c_str(), NULL, NULL, SW_SHOWNORMAL);
			break;

		case ID_MORE_ABOUT:
			util::show_popup(
				std::format(
					"{} ({})\n\nDeveloped by {}\nTranslated by {}",
					global::PLUGIN_NAME,
					global::PLUGIN_VERSION.str(),
					global::PLUGIN_DEVELOPER,
					global::PLUGIN_TRANSLATOR
				).c_str()
			);
			break;

		case ID_MORE_UPDATES:
			break;

		case ID_MORE_REPORTABUG:
			::ShellExecuteA(NULL, "open", "https://forms.gle/LMPcpJTvXvw3fWRe7", NULL, NULL, SW_SHOWNORMAL);
			break;

		default:
			return false;
		}
		return true;
	}
}