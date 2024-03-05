#include "menu.hpp"



namespace cved {
	HMENU Menu::get_handle() noexcept {
		update_state();
		return menu_;
	}

	int Menu::show(
		HWND hwnd,
		UINT flags,
		const mkaul::Point<LONG>* p_custom_point_screen
	) noexcept {
		POINT tmp;
		::GetCursorPos(&tmp);
		if (p_custom_point_screen) {
			tmp = p_custom_point_screen->to<POINT>();
		}
		update_state();
		return ::TrackPopupMenu(
			menu_,
			flags,
			tmp.x,
			tmp.y,
			0, hwnd, NULL
		);
	}
}