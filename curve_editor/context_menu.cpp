#include "context_menu.hpp"
#include <stdexcept>



namespace curve_editor {
	void ContextMenu::init() noexcept {
		menu_ = ::CreatePopupMenu();
		for (size_t i = 1; auto & item : items_) {
			MENUITEMINFOW minfo;
			item.get_menu_item_info(minfo, i);
			::InsertMenuItemW(menu_, minfo.wID, TRUE, &minfo);
			i++;
		}
	}

	bool ContextMenu::show(
		HWND hwnd,
		UINT flags,
		const mkaul::Point<LONG>* p_custom_pt_screen
	) noexcept {
		POINT tmp;
		::GetCursorPos(&tmp);
		if (p_custom_pt_screen) {
			tmp = p_custom_pt_screen->to<POINT>();
		}
		int32_t ret = ::TrackPopupMenu(
			menu_,
			flags | TPM_RETURNCMD | TPM_NONOTIFY,
			tmp.x,
			tmp.y,
			0, hwnd, NULL
		);

		if (ret == 0) {
			return false;
		}
		auto root_idx = static_cast<size_t>(ret);
		while (MenuItem::MAX_SUB_MENU_ITEMS <= root_idx) {
			root_idx = root_idx / MenuItem::MAX_SUB_MENU_ITEMS;
		}
		try {
			return items_.at(root_idx - 1).callback(static_cast<uint32_t>(ret));
		}
		catch (const std::out_of_range&) {
			return false;
		}
	}
} // namespace curve_editor