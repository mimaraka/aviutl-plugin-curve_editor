#include "menu.hpp"



namespace cved {
	HMENU Menu::get_handle() noexcept {
		update_state();
		return menu_;
	}

	bool Menu::show(
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
		int ret = ::TrackPopupMenu(
			menu_,
			flags | TPM_RETURNCMD | TPM_NONOTIFY,
			tmp.x,
			tmp.y,
			0, hwnd, NULL
		);
		return callback(ret);
	}
}