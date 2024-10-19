#include "config.hpp"
#include "constants.hpp"
#include "dialog_about.hpp"
#include "dialog_pref.hpp"
#include "global.hpp"
#include "menu_others.hpp"
#include "my_webview2_reference.hpp"
#include "resource.h"
#include "string_table.hpp"
#include "util.hpp"
#include <format>



namespace cved {
	OthersMenu::OthersMenu(HINSTANCE hinst) {
		menu_ = ::GetSubMenu(::LoadMenuA(hinst, MAKEINTRESOURCEA(IDR_MENU_MORE)), 0);
		MENUITEMINFOA minfo_tmp;
		minfo_tmp.cbSize = sizeof(MENUITEMINFOA);
		minfo_tmp.fMask = MIIM_FTYPE;
		minfo_tmp.fType = MFT_RADIOCHECK;

		::SetMenuItemInfoA(menu_, ID_LAYOUT_VERTICAL, FALSE, &minfo_tmp);
		::SetMenuItemInfoA(menu_, ID_LAYOUT_HORIZONTAL, FALSE, &minfo_tmp);
	}

	void OthersMenu::update_state() noexcept {
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

	bool OthersMenu::callback(uint16_t id) noexcept {
		using StringId = global::StringTable::StringId;

		switch (id) {
		case ID_LAYOUT_VERTICAL:
			global::config.set_layout_mode(LayoutMode::Vertical);
			//global::window_main.send_command((WPARAM)WindowCommand::MoveWindow);
			break;

		case ID_LAYOUT_HORIZONTAL:
			global::config.set_layout_mode(LayoutMode::Horizontal);
			//global::window_main.send_command((WPARAM)WindowCommand::MoveWindow);
			break;
			
		case ID_MORE_SHOWLIBRARY:
			global::config.set_show_library(!global::config.get_show_library());
			//global::window_main.send_command((WPARAM)WindowCommand::Update);
			break;

		case ID_MORE_RELOAD:
			if (global::webview) global::webview->reload();
			break;

		case ID_MORE_PREFERENCES:
		{
			PrefDialog dialog;
			dialog.show(global::fp->hwnd);
			break;
		}

		case ID_MORE_HELP:
			::ShellExecuteA(NULL, "open", std::format("{}/wiki", global::PLUGIN_GITHUB_URL).c_str(), NULL, NULL, SW_SHOWNORMAL);
			break;

		case ID_MORE_ABOUT:
		{
			AboutDialog dialog;
			dialog.show(global::fp->hwnd);
			break;
		}

		default:
			return false;
		}
		return true;
	}
}