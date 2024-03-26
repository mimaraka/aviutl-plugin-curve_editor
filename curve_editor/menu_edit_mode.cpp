#include "menu_edit_mode.hpp"
#include "enum.hpp"
#include "config.hpp"
#include "global.hpp"



namespace cved {
	EditModeMenu::EditModeMenu() {
		menu_ = ::CreatePopupMenu();
		// 編集モード選択メニューの作成
		MENUITEMINFOA minfo_tmp;
		minfo_tmp.cbSize = sizeof(MENUITEMINFOA);
		minfo_tmp.fMask = MIIM_TYPE | MIIM_ID;
		minfo_tmp.fType = MFT_RADIOCHECK;

		for (uint32_t i = 0u; i < (uint32_t)EditMode::NumEditMode; i++) {
			minfo_tmp.dwTypeData = const_cast<char*>(global::config.get_edit_mode_str((EditMode)i));
			minfo_tmp.wID = (uint32_t)WindowCommand::EditModeNormal + i;
			::InsertMenuItemA(menu_, i, TRUE, &minfo_tmp);
		}
	}

	void EditModeMenu::update_state() noexcept {
		MENUITEMINFOA minfo_tmp;
		minfo_tmp.cbSize = sizeof(MENUITEMINFOA);
		minfo_tmp.fMask = MIIM_STATE;

		for (uint32_t i = 0u; i < (uint32_t)EditMode::NumEditMode; i++) {
			minfo_tmp.fState = (EditMode)i == global::config.get_edit_mode() ? MFS_CHECKED : MFS_UNCHECKED;
			// TODO: モードを実装し次第削除する
			if (i == (uint32_t)EditMode::Value or i == (uint32_t)EditMode::Step or i == (uint32_t)EditMode::Script) {
				minfo_tmp.fState |= MFS_DISABLED;
			}
			::SetMenuItemInfoA(menu_, i, TRUE, &minfo_tmp);
		}
	}

	bool EditModeMenu::callback(uint16_t id) noexcept {
		if (mkaul::in_range(
			id,
			(uint16_t)WindowCommand::EditModeNormal,
			(uint16_t)WindowCommand::EditModeNormal + (uint16_t)EditMode::NumEditMode - 1u,
			true
		)) {
			EditMode new_mode = (EditMode)(id - (uint16_t)WindowCommand::EditModeNormal);
			// 編集モードが変更された場合
			if (new_mode != global::config.get_edit_mode()) {
				global::config.set_edit_mode((EditMode)(id - (uint16_t)WindowCommand::EditModeNormal));
				global::window_main.send_command((WPARAM)WindowCommand::Update);
				global::window_grapheditor.send_command((WPARAM)WindowCommand::Fit);
			}
			return true;
		}
		else return false;
	}
}