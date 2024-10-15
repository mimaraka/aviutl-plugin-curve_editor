#include "curve_editor.hpp"
#include "dialog_id_jumpto.hpp"
#include "global.hpp"
#include "menu_id.hpp"
#include "my_messagebox.hpp"
#include "resource.h"
#include "string_table.hpp"



namespace cved {
	IdMenu::IdMenu(HINSTANCE hinst) {
		menu_ = ::GetSubMenu(::LoadMenuA(hinst, MAKEINTRESOURCEA(IDR_MENU_ID)), 0);
	}

	void IdMenu::update_state() noexcept {
		MENUITEMINFOA minfo_tmp;
		minfo_tmp.cbSize = sizeof(MENUITEMINFOA);
		minfo_tmp.fMask = MIIM_STATE;
		minfo_tmp.fState = global::editor.is_idx_first() ? MFS_DISABLED : MFS_ENABLED;
		::SetMenuItemInfoA(menu_, ID_ID_JUMPTOFIRST, FALSE, &minfo_tmp);
		minfo_tmp.fState = global::editor.is_idx_last() ? MFS_DISABLED : MFS_ENABLED;
		::SetMenuItemInfoA(menu_, ID_ID_JUMPTOLAST, FALSE, &minfo_tmp);
		minfo_tmp.fState = global::editor.is_idx_last() and !global::editor.is_idx_first() ? MFS_ENABLED : MFS_DISABLED;
		::SetMenuItemInfoA(menu_, ID_ID_DELETE, FALSE, &minfo_tmp);
	}

	bool IdMenu::callback(uint16_t id) noexcept {
		using StringId = global::StringTable::StringId;
		switch (id) {
		case ID_ID_JUMP:
		{
			IdJumptoDialog dialog;
			if (dialog.show(global::fp->hwnd) == IDOK) {
				//global::window_main.send_command((WPARAM)WindowCommand::Update);
			}
			break;
		}

		case ID_ID_DELETE:
		{
			int resp = my_messagebox(
				global::string_table[StringId::WarningDeleteId],
				global::fp->hwnd,
				MessageBoxIcon::Warning,
				MessageBoxButton::OkCancel
			);
			if (resp == IDOK) {
				global::editor.pop_idx();
				//global::window_main.send_command((WPARAM)WindowCommand::Update);
			}
			break;
		}
		
		case ID_ID_JUMPTOFIRST:
			global::editor.set_idx(0);
			//global::window_main.send_command((WPARAM)WindowCommand::Update);
			break;

		case ID_ID_JUMPTOLAST:
			global::editor.jump_to_last_idx();
			//global::window_main.send_command((WPARAM)WindowCommand::Update);
			break;

		default:
			return false;
		}
		return true;
	}
}