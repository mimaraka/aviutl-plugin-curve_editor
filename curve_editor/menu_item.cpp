#include "menu_item.hpp"
#include <stdexcept>



namespace curve_editor {
	bool MenuItem::callback(uint32_t id) const noexcept {
		// サブメニューを持つ場合
		if (!sub_menu_items_.empty()) {
			auto tmp = id;
			while ((id_ + 1) * MAX_SUB_MENU_ITEMS <= tmp) {
				tmp = tmp / MAX_SUB_MENU_ITEMS;
			}
			auto idx = tmp - id_ * MAX_SUB_MENU_ITEMS - 1;
			try {
				return sub_menu_items_.at(idx).callback(id);
			}
			catch (const std::out_of_range&) {
				return false;
			}
		}
		else if (callback_) {
			callback_();
			return true;
		}
		return false;
	}

	void MenuItem::get_menu_item_info(MENUITEMINFOW& minfo, uint32_t id) noexcept {
		minfo.cbSize = sizeof(MENUITEMINFOW);
		minfo.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_STRING;
		minfo.fType = static_cast<UINT>(type_);
		minfo.fState = static_cast<UINT>(state_);
		minfo.dwTypeData = const_cast<LPWSTR>(label_.c_str());
		minfo.wID = static_cast<UINT>(id_ = id);
		if (!sub_menu_items_.empty()) {
			minfo.fMask |= MIIM_SUBMENU;
			minfo.hSubMenu = sub_menu_ = ::CreatePopupMenu();
			// サブメニューの追加
			for (size_t i = 1; auto & item : sub_menu_items_) {
				uint32_t sub_id = id * MAX_SUB_MENU_ITEMS + i;
				MENUITEMINFOW sub_minfo;
				item.get_menu_item_info(sub_minfo, sub_id);
				sub_minfo.fMask |= MIIM_ID;
				sub_minfo.wID = static_cast<UINT>(sub_id);
				::InsertMenuItemW(sub_menu_, sub_id, FALSE, &sub_minfo);
				i++;
			}
		}
	}
} // namespace curve_editor